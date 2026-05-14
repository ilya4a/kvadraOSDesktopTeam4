

#ifndef KVADRAOSDESKTOPTEAM4_MEDIAINDEX_H
#define KVADRAOSDESKTOPTEAM4_MEDIAINDEX_H
#include <string>
#include <vector>

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
#endif // KVADRAOSDESKTOPTEAM4_MEDIAINDEX_H
