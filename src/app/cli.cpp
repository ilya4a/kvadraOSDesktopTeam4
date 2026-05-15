#include "app/cli.h"

#include <csignal>
#include <iostream>
#include <thread>

#include "scanner/ScanOptions.h"
#include "scanner/Scanner.h"
#include "storage/json_writer.h"
#include <nlohmann/json.hpp>

#include <cctype>
#include <filesystem>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "CLI/CLI.hpp"
#include "app/AppConfig.h"
#include "storage/MediaIndexStore.h"
#include "utils/MediaHttpServer.h"

std::atomic<bool> stop_flag { false };

std::filesystem::path get_home_dir() {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (!pw || !pw->pw_dir) {
        throw std::runtime_error("Cannot determine home directory");
    }
    return pw->pw_dir;
}

inline std::chrono::seconds parse_interval(const std::string &input) {
    if (input.empty()) {
        throw std::invalid_argument("Empty interval string");
    }

    std::size_t pos = 0;
    long value = std::stol(input, &pos);
    std::string suffix = input.substr(pos);

    if (suffix.empty()) {
        return std::chrono::seconds(value);
    }

    for (char &c : suffix) {
        c = static_cast<char>(std::tolower(c));
    }

    if (suffix == "s" || suffix == "sec" || suffix == "seconds") {
        return std::chrono::seconds(value);
    } else if (suffix == "m" || suffix == "min" || suffix == "minutes") {
        return std::chrono::minutes(value);
    } else if (suffix == "h" || suffix == "hour" || suffix == "hours") {
        return std::chrono::hours(value);
    } else if (suffix == "d" || suffix == "day" || suffix == "days") {
        return std::chrono::hours(value * 24);
    } else {
        throw std::invalid_argument("Unknown interval suffix: " + suffix);
    }
}

void run_scan(
    ScanOptions scan_options,
    bool file_result,
    MediaIndexStore &store,
    const std::filesystem::path &out_path,
    std::string str_delay
) {
    using clock = std::chrono::steady_clock;

    if (file_result && out_path.empty()) {
        throw std::runtime_error("Output path is empty");
    }

    std::chrono::seconds delay;
    try {
        delay = parse_interval(str_delay);
    } catch (const std::exception &) {
        std::cerr << "Invalid interval format: " << str_delay << std::endl;
        return;
    }

    auto next_scan = clock::now();

    while (!stop_flag.load()) {
        Scanner scanner(scan_options);
        MediaIndex index = scanner.scan();

        if (file_result) {
            try {
                write_media_index(index, out_path / ".media_files", true);
            } catch (const std::exception &e) {
                std::cerr << "Error writing JSON: " << e.what() << std::endl;
            }
        } else {
            store.update(index);
        }

        next_scan = clock::now() + delay;

        while (clock::now() < next_scan && !stop_flag.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void run_scan_server_result(ScanOptions scan_options, MediaIndexStore &store, std::string str_delay) {
    run_scan(scan_options, false, store, "", str_delay);
}

void run_scan_file_result(ScanOptions scan_options, std::filesystem::path out_path, std::string str_delay) {
    MediaIndexStore temp_store;
    run_scan(scan_options, true, temp_store, out_path, str_delay);
}

void signal_handler(int) {
    stop_flag.store(true);
}

int parse_cli(int argc, char **argv, const std::string &default_root, AppConfig &config) {
    CLI::App app { "Media file scanner - periodically scans a directory and creates .media_files JSON" };

    config.root_path = default_root;

    app.add_option("-p,--path", config.root_path, "Root directory to scan")->capture_default_str();
    app.add_option("-i,--interval", config.interval_spec, "Scan interval (e.g. 10s, 5min, 2h, 1d)")
        ->capture_default_str();
    app.add_flag("!-r,--recursive", config.recursive, "Disable recursive scan");
    app.add_flag("-s,--symlinks", config.follow_symlinks, "Follow symbolic links");
    app.add_flag(
        "-f,--file",
        config.run_with_file_result,
        "Save result to .media_files instead of serving it over HTTP."
    );
    app.add_option("-o,--output", config.output_dir, "Directory to save .media_files (defaults to --path)");
    app.add_option("--allow-ext", config.allowed_ext, "Allow only these extensions (e.g. .mp3 .wav)")->expected(-1);
    app.add_option("--block-ext", config.blocked_ext, "Exclude these extensions (e.g. .tmp .txt)")->expected(-1);

    CLI11_PARSE(app, argc, argv);

    if (config.output_dir.empty()) {
        config.output_dir = config.root_path;
    }

    return 0;
}

ScanOptions build_scan_options(const AppConfig &config) {
    ScanOptions::Builder builder = ScanOptions::Builder()
                                       .set_root_path(config.root_path)
                                       .set_follow_symlinks(config.follow_symlinks)
                                       .set_recursive(config.recursive);

    if (!config.blocked_ext.empty()) {
        builder.remove_ext(config.blocked_ext);
    }
    if (!config.allowed_ext.empty()) {
        builder.select_ext(config.allowed_ext);
    }

    return builder.build();
}

void run_file_mode(const ScanOptions &scan_opts, const AppConfig &config) {
    run_scan_file_result(scan_opts, config.output_dir, config.interval_spec);
}

void run_http_mode(const ScanOptions &scan_opts, const AppConfig &config) {
    MediaIndexStore store;
    MediaHttpServer http_server(store);

    std::thread http_thread([&]() { http_server.run(); });

    std::thread scan_thread([&]() { run_scan_server_result(scan_opts, store, config.interval_spec); });

    scan_thread.join();
    http_server.stop();
    http_thread.join();
}

int run_app(int argc, char **argv) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::string default_root;
    try {
        default_root = get_home_dir().string();
    } catch (const std::exception &e) {
        std::cerr << "Error getting home directory: " << e.what() << std::endl;
        return 1;
    }

    try {
        AppConfig config;
        int cli_res = parse_cli(argc, argv, default_root, config);
        ScanOptions scan_opts = build_scan_options(config);

        std::cout << "Starting scan:\n"
                  << "  Path: " << config.root_path << "\n"
                  << "  Interval: " << config.interval_spec << "\n"
                  << "  Recursive: " << (config.recursive ? "yes" : "no") << "\n"
                  << "  Symlinks: " << (config.follow_symlinks ? "yes" : "no") << "\n";

        if (config.run_with_file_result) {
            std::cout << "  Output JSON: " << config.output_dir << "/.media_files\n";
            std::cout << "Press Ctrl+C to stop." << std::endl;
            run_file_mode(scan_opts, config);
        } else {
            std::cout << "  Output JSON: http://localhost:1234/media_files\n";
            std::cout << "Press Ctrl+C to stop." << std::endl;
            run_http_mode(scan_opts, config);
        }

        return cli_res;

    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
