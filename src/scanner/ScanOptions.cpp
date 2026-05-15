#include "scanner/ScanOptions.h"

ScanOptions::Builder::Builder(std::filesystem::path root_path, bool follow_symlinks, bool recursive)
    : root_path_(std::move(root_path)),
      follow_symlinks_(follow_symlinks),
      recursive_(recursive) { }

ScanOptions::Builder &ScanOptions::Builder::select_ext(const std::vector<std::string> &ext) {
    std::unordered_set<std::string> allowed(ext.begin(), ext.end());

    std::erase_if(audio_ext_, [&](const std::string &e) { return !allowed.contains(e); });
    std::erase_if(video_ext_, [&](const std::string &e) { return !allowed.contains(e); });
    std::erase_if(image_ext_, [&](const std::string &e) { return !allowed.contains(e); });

    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::remove_ext(const std::vector<std::string> &ext) {
    std::unordered_set<std::string> to_remove(ext.begin(), ext.end());

    auto remove_from = [&](std::unordered_set<std::string> &s) {
        std::erase_if(s, [&](const std::string &e) { return to_remove.contains(e); });
    };

    remove_from(audio_ext_);
    remove_from(video_ext_);
    remove_from(image_ext_);

    return *this;
}

ScanOptions ScanOptions::Builder::build() {
    return ScanOptions(
        std::move(root_path_),
        follow_symlinks_,
        recursive_,
        std::move(audio_ext_),
        std::move(video_ext_),
        std::move(image_ext_)
    );
}
ScanOptions::ScanOptions(
    std::filesystem::path &&root_path,
    bool follow_symlinks,
    bool recursive,
    std::unordered_set<std::string> &&audio_ext,
    std::unordered_set<std::string> &&video_ext,
    std::unordered_set<std::string> &&image_ext
)
    : scan_path_(std::move(root_path)),
      follow_symlinks_(follow_symlinks),
      recursive_(recursive),
      audio_ext_(std::move(audio_ext)),
      video_ext_(std::move(video_ext)),
      image_ext_(std::move(image_ext)) { }
