
#include "Scanner.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

Scanner::Scanner(ScanOptions options) : options_(std::move(options)) {}

bool has_extension(const fs::path& path, const std::unordered_set<std::string>& exts) {
    if (!path.has_extension()) return false;
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return exts.count(ext);
}

void sort_and_unique(std::vector<std::string>& v) {
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
};

MediaIndex Scanner::scan() const {
    MediaIndex index;

    auto iter_opt = fs::directory_options::skip_permission_denied;
    if (options_.follow_symlinks())
        iter_opt |= fs::directory_options::follow_directory_symlink;

    const fs::path root = options_.root_path();

    if (options_.recursive()) {
        for (const auto& entry : fs::recursive_directory_iterator(root, iter_opt)) {
            if (!entry.is_regular_file()) continue;

            if (!options_.follow_symlinks() && entry.is_symlink()) continue;

            const auto& path = entry.path();
            if (has_extension(path, options_.audio_ext())) {
                index.audio.push_back(path.filename().string());
            } else if (has_extension(path, options_.video_ext())) {
                index.video.push_back(path.filename().string());
            } else if (has_extension(path, options_.image_ext())) {
                index.images.push_back(path.filename().string());
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(root, iter_opt)) {
            if (!entry.is_regular_file()) continue;
            if (!options_.follow_symlinks() && entry.is_symlink()) continue;

            const auto& path = entry.path();
            if (has_extension(path, options_.audio_ext())) {
                index.audio.push_back(path.filename().string());
            } else if (has_extension(path, options_.video_ext())) {
                index.video.push_back(path.filename().string());
            } else if (has_extension(path, options_.image_ext())) {
                index.images.push_back(path.filename().string());
            }
        }
    }

    sort_and_unique(index.audio);
    sort_and_unique(index.video);
    sort_and_unique(index.images);

    return index;
}
