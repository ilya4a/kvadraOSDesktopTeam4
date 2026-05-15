#ifndef KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H
#define KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H

#include <mutex>
#include <string>

#include "json_writer.h"
#include "utils/MediaIndex.h"

class MediaIndexStore {
  public:
    void update(const MediaIndex &index);
    std::string get() const;

  private:
    mutable std::mutex mutex_;
    std::string json_ = R"({"audio":[],"video":[],"images":[]})";
};

#endif // KVADRAOSDESKTOPTEAM4_MEDIAINDEXSTORE_H
