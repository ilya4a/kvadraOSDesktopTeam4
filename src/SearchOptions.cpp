
#include "SearchOptions.h"

ScanOptions::Builder::Builder(std::filesystem::path root_path, bool follow_symlinks, bool recursive)  : root_path_(std::move(root_path)),
            follow_symlinks_(follow_symlinks), recursive_(recursive){}

ScanOptions::Builder &ScanOptions::Builder::set_audio_ext(const std::vector<std::string> &audio_ext) {
    audio_ext_ = std::unordered_set<std::string>(audio_ext.begin(), audio_ext.end());
    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::remove_audio_ext(std::vector<std::string> const &audio_ext)  {
    for (auto const &i: audio_ext) audio_ext_.erase(i);
    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::set_video_ext(std::vector<std::string> const &video_ext)  {
    video_ext_ = std::unordered_set<std::string>(video_ext.begin(), video_ext.end());
    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::remove_video_ext(std::vector<std::string> const &video_ext)  {
    for (auto const &i: video_ext) video_ext_.erase(i);
    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::set_image_ext(std::vector<std::string> const &image_ext)  {
    image_ext_ = std::unordered_set<std::string>(image_ext.begin(), image_ext.end());
    return *this;
}

ScanOptions::Builder &ScanOptions::Builder::remove_image_ext(std::vector<std::string> const &image_ext)  {
    for (auto const &i: image_ext) image_ext_.erase(i);
    return *this;
}

ScanOptions ScanOptions::Builder::build()  {
    return ScanOptions(std::move(root_path_),
        follow_symlinks_,
        recursive_,
        std::move(audio_ext_),
        std::move(video_ext_),
        std::move(image_ext_));
}
ScanOptions::ScanOptions(std::filesystem::path &&root_path, bool follow_symlinks, bool recursive,
                         std::unordered_set<std::string> &&audio_ext, std::unordered_set<std::string> &&video_ext,
                         std::unordered_set<std::string> &&image_ext)  :
    root_path_(std::move(root_path)),
    follow_symlinks_(follow_symlinks),
    recursive_(recursive),
    audio_ext_(std::move(audio_ext)),
    video_ext_(std::move(video_ext)),
    image_ext_(std::move(image_ext))
{}
