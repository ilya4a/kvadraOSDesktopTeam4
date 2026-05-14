#ifndef KVADRAOSDESKTOPTEAM4_SCANNER_H
#define KVADRAOSDESKTOPTEAM4_SCANNER_H
#include <filesystem>
#include <string>
#include <vector>


enum class MediaType {
    Audio,
    Video,
    Image,
    Unknown
};

struct MediaIndex {
    std::vector<std::string> audio;
    std::vector<std::string> video;
    std::vector<std::string> images;
};

struct ScanOptions {
    std::filesystem::path root_path;
    bool follow_symlinks = false;
    bool recursive = true;
};


class Scanner {
public:
    explicit Scanner(ScanOptions options);
    MediaIndex scan() const;

private:
    ScanOptions options_;
};


#endif // KVADRAOSDESKTOPTEAM4_SCANNER_H
