#ifndef KVADRAOSDESKTOPTEAM4_APPCONFIG_H
#define KVADRAOSDESKTOPTEAM4_APPCONFIG_H
#include <string>
#include <vector>

struct AppConfig {
    std::string root_path;
    std::string output_dir;
    std::string interval_spec = "30s";
    bool recursive = true;
    bool follow_symlinks = false;
    bool run_with_file_result = false;
    std::vector<std::string> allowed_ext;
    std::vector<std::string> blocked_ext;
};

#endif // KVADRAOSDESKTOPTEAM4_APPCONFIG_H
