#include "storage/MediaIndexStore.h"
void MediaIndexStore::update(const MediaIndex &index) {
    std::lock_guard<std::mutex> lock(mutex_);
    json_ = write_media_index(index);
}
std::string MediaIndexStore::get() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return json_;
}
