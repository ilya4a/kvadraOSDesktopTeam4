#ifndef KVADRAOSDESKTOPTEAM4_JSON_WRITER_H
#define KVADRAOSDESKTOPTEAM4_JSON_WRITER_H

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "utils/MediaIndex.h"

inline std::string write_media_index(
    const MediaIndex &index,
    const std::filesystem::path &output_path = "",
    bool file_result = false
) {
    using json = nlohmann::json;

    json j;
    j["audio"] = index.audio;
    j["video"] = index.video;
    j["images"] = index.images;

    std::string dumb = j.dump(4);

    if (file_result) {
        std::ofstream file(output_path);
        if (!file) {
            throw std::runtime_error("Cannot open file for writing: " + output_path.string());
        }
        file << dumb;
    }

    return dumb;
}

#endif // KVADRAOSDESKTOPTEAM4_JSON_WRITER_H
