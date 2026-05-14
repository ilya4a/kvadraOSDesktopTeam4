#ifndef KVADRAOSDESKTOPTEAM4_SCANNER_H
#define KVADRAOSDESKTOPTEAM4_SCANNER_H
#include <filesystem>
#include <string>
#include <vector>

#include "SearchOptions.h"


struct ScanOptions;
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

    void clean_index() {
        audio.clear();
        video.clear();
        images.clear();
    }

};

class Scanner {
public:
    explicit Scanner(ScanOptions options);
    MediaIndex scan() const;

private:
    ScanOptions options_;
};


#endif // KVADRAOSDESKTOPTEAM4_SCANNER_H
