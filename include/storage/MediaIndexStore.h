
#ifndef KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H
#define KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H

#include <mutex>
#include <string>

#include "../MediaIndex.h"
#include "json_writer.h"

class MediaIndexStore {

public:
    void update(const MediaIndex& index) {
        std::lock_guard<std::mutex> lock(mutex_);
        json_ = write_media_index(index);
    }

    std::string get() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return json_;
    }

private:
    mutable std::mutex mutex_;
    std::string json_ = R"({"audio":[],"video":[],"images":[]})";
};


#endif // KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H
