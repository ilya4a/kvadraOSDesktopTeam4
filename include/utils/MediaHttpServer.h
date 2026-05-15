#ifndef KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H
#define KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H

#include <httplib.h>

#include "storage/MediaIndexStore.h"

class MediaHttpServer {
  public:
    explicit MediaHttpServer(MediaIndexStore &store);

    void run();

    void stop();

  private:
    httplib::Server server_;
    MediaIndexStore &store_;
};

#endif // KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H
