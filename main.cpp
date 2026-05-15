#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include "Scanner.h"
#include "SearchOptions.h"
#include "json_writer.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdexcept>
#include <string>
#include <filesystem>

bool stop_flag = false;

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


void run_scan(ScanOptions scan_options, std::string str_delay = "30s", std::filesystem::path out_path = "") {

    using clock = std::chrono::steady_clock;

    if (out_path.empty()) out_path = get_home_dir();

    auto next_scan = clock::now();
    std::chrono::seconds delay = parse_interval(str_delay);

    while (!stop_flag) {
        Scanner scanner(scan_options);

        MediaIndex index = scanner.scan();
        write_media_index(index, out_path/".media_files");

        next_scan += delay;
        std::this_thread::sleep_until(next_scan);
    }
}


int main() {
    ScanOptions scan_options = ScanOptions::Builder().
    set_root_path("/home/ilya/Music/music_flac").
    set_follow_symlinks(false).
    set_recursive(true).build();

    run_scan(scan_options);

    return 0;
}
