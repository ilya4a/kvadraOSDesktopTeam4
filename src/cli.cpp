
#include "cli.h"

#include <csignal>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include "ScanOptions.h"
#include "Scanner.h"
#include "json_writer.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <cctype>

#include "CLI/CLI.hpp"
#include "MediaHttpServer.h"
#include "MediaIndexStore.h"

std::atomic<bool> stop_flag{false};

std::filesystem::path get_home_dir() {
    uid_t uid = getuid();
    struct passwd* pw = getpwuid(uid);
    if (!pw || !pw->pw_dir) {
        throw std::runtime_error("Cannot determine home directory");
    }
    return pw->pw_dir;
}

inline std::chrono::seconds parse_interval(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Empty interval string");
    }

    std::size_t pos = 0;
    long value = std::stol(input, &pos);
    std::string suffix = input.substr(pos);

    if (suffix.empty()) {
        return std::chrono::seconds(value);
    }

    for (char& c : suffix) c = static_cast<char>(std::tolower(c));

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


void run_scan(ScanOptions scan_options, bool file_result, MediaIndexStore& store, std::filesystem::path out_path, std::string str_delay) {

    using clock = std::chrono::steady_clock;

    if (file_result) {
        if (out_path.empty()) throw std::runtime_error("Output path is empty");
    }

    auto next_scan = clock::now();
    std::chrono::seconds delay;

    try {
        delay = parse_interval(str_delay);
    } catch (const std::exception& e) {
        std::cerr << "Invalid interval format: " << str_delay << std::endl;
        return;
    }

    while (!stop_flag.load()) {
        Scanner scanner(scan_options);

        MediaIndex index = scanner.scan();
        if (file_result) {
            try {
                write_media_index(index, out_path / ".media_files", file_result);
            } catch (const std::exception& e) {
                std::cerr << "Error writing JSON: " << e.what() << std::endl;
            }
        }else {
            store.update(index);
        }

        next_scan += delay;

        while (clock::now() < next_scan && !stop_flag.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    }
}

void run_scan_server_result(ScanOptions scan_options, MediaIndexStore& store, std::string str_delay) {
    run_scan(scan_options, false, store, "",  str_delay);
}


void run_scan_file_result(ScanOptions scan_options, std::filesystem::path out_path, std::string str_delay) {
    MediaIndexStore temp_store;
    run_scan(scan_options, true, temp_store, out_path, str_delay);
}

void signal_handler(int) {
    _exit(0);
}

void console_listener() {
    std::string line;
    while (!stop_flag.load()) {
        std::getline(std::cin, line);
        if (line == "q" || line == "quit") {
            stop_flag.store(true);
            break;
        }
    }
}

int run_app(int argc, char** argv) {
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::thread input_thread(console_listener);
    input_thread.detach();

    CLI::App app{"Media file scanner - periodically scans a directory and creates .media_files JSON"};

    std::string root_path;

    try {
        root_path = get_home_dir().string();
    } catch (const std::exception& e) {
        std::cerr << "Error getting home directory: " << e.what() << std::endl;
        return 1;
    }

    std::string output_dir = "";

    std::string interval_spec = "30s";
    bool recursive = true;
    bool follow_symlinks = false;

    bool run_with_file_result = false;


    std::vector<std::string> allowed_ext;
    std::vector<std::string> blocked_ext;

    app.add_option("-p,--path", root_path, "Root directory to scan")
       ->capture_default_str();
    app.add_option("-i,--interval", interval_spec, "Scan interval (e.g. 10s, 5min, 2h, 1d)")
       ->capture_default_str();
    app.add_flag("-r,--recursive", recursive, "Recursive scan (default true)");
    app.add_flag("-s,--symlinks", follow_symlinks, "Follow symbolic links");

    app.add_flag("-f,--file", run_with_file_result, "Get result as file");

    app.add_option("-o,--output", output_dir, "Directory to save .media_files (defaults to --path)");

    app.add_option("--allow-ext", allowed_ext, "Allow only these extensions (e.g. .mp3 .wav)")
   ->expected(-1);

    app.add_option("--block-ext", blocked_ext, "Exclude these extensions (e.g. .tmp .txt)")
    ->expected(-1);

    CLI11_PARSE(app, argc, argv);

    if (output_dir.empty()) {
        output_dir = root_path;
    }

    ScanOptions::Builder builder = ScanOptions::Builder()
    .set_root_path(root_path)
    .set_follow_symlinks(follow_symlinks)
    .set_recursive(recursive);

    if (!blocked_ext.empty()) {
        builder.remove_ext(blocked_ext);
    }
    if (!allowed_ext.empty()) {
        builder.select_ext(allowed_ext);
    }

    ScanOptions scan_opts = builder.build();

    std::cout << "Starting scan:\n"
              << "  Path: " << root_path << "\n"
              << "  Interval: " << interval_spec << "\n"
              << "  Output JSON: " << output_dir << "/.media_files\n"
              << "  Recursive: " << (recursive ? "yes" : "no") << "\n"
              << "  Symlinks: " << (follow_symlinks ? "yes" : "no") << "\n"
              << "Press Ctrl+C or q to stop." << std::endl;

    try {

        if (run_with_file_result) {
            run_scan_file_result(scan_opts, output_dir, interval_spec);
        }else {
            MediaIndexStore store;
            MediaHttpServer http_server(store);

            std::thread http_thread([&]() {
                http_server.run();
            });

            run_scan_server_result(scan_opts, store, interval_spec);

            http_server.stop();
            if (http_thread.joinable()) {
                http_thread.join();
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }

    std::exit(0);
}
