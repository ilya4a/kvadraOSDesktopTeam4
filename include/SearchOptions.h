#ifndef KVADRAOSDESKTOPTEAM4_SEARCHOPTIONS_H
#define KVADRAOSDESKTOPTEAM4_SEARCHOPTIONS_H
#include <filesystem>
#include <unordered_set>
#include <vector>


struct ScanOptions {
private:
    std::filesystem::path root_path_;
    bool follow_symlinks_ = false;
    bool recursive_ = true;

    std::unordered_set<std::string> audio_ext_;
    std::unordered_set<std::string> video_ext_;
    std::unordered_set<std::string> image_ext_;

public:

    const std::filesystem::path& root_path() const { return root_path_; }
    bool follow_symlinks() const { return follow_symlinks_; }
    bool recursive() const { return recursive_;}

    const std::unordered_set<std::string>& audio_ext() const { return audio_ext_; }
    const std::unordered_set<std::string>& video_ext() const { return video_ext_; }
    const std::unordered_set<std::string>& image_ext() const { return image_ext_; }

    class Builder {
        std::filesystem::path root_path_ = "./";
        bool follow_symlinks_ = false;
        bool recursive_ = true;

        std::unordered_set<std::string> audio_ext_ = {
            ".mp3", ".wav", ".flac", ".aac", ".ogg", ".m4a"
        };

        std::unordered_set<std::string> video_ext_ = {
            ".mp4", ".mkv", ".avi", ".mov", ".mpg", ".mpeg", ".webm"
        };

        std::unordered_set<std::string> image_ext_ = {
            ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tiff", ".webp"
        };

    public:
        Builder(std::filesystem::path root_path = "./", bool follow_symlinks = false,
        bool recursive = true);

        Builder& set_audio_ext(const std::vector<std::string> & audio_ext);

        Builder& remove_audio_ext(std::vector<std::string> const& audio_ext);

        Builder& set_video_ext(std::vector<std::string> const& video_ext);

        Builder& remove_video_ext(std::vector<std::string> const& video_ext);

        Builder& set_image_ext(std::vector<std::string> const& image_ext);

        Builder& remove_image_ext(std::vector<std::string> const& image_ext);

        Builder& root_path(std::filesystem::path path) { root_path_ = std::move(path); return *this; }
        Builder& follow_symlinks(bool v) { follow_symlinks_ = v; return *this; }
        Builder& recursive(bool v) { recursive_ = v; return *this; }


        ScanOptions build();
    };

private:
    ScanOptions(std::filesystem::path&& root_path,
        bool follow_symlinks,
        bool recursive,
        std::unordered_set<std::string>&& audio_ext,
        std::unordered_set<std::string>&& video_ext,
        std::unordered_set<std::string>&& image_ext
        );
};




#endif // KVADRAOSDESKTOPTEAM4_SEARCHOPTIONS_H
