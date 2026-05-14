#ifndef KVADRAOSDESKTOPTEAM4_JSON_WRITER_H
#define KVADRAOSDESKTOPTEAM4_JSON_WRITER_H

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include "MediaIndex.h"

inline void write_media_index(const MediaIndex& index, const std::filesystem::path& output_path) {
    using json = nlohmann::json;

    json j;
    j["audio"]  = index.audio;
    j["video"]  = index.video;
    j["images"] = index.images;

    std::ofstream file(output_path);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + output_path.string());
    }
    file << j.dump(4);
}

#endif // KVADRAOSDESKTOPTEAM4_JSON_WRITER_H
