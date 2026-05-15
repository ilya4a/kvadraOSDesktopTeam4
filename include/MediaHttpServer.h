
#ifndef KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H
#define KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H

#include <httplib.h>

#include "MediaIndexStore.h"

class MediaHttpServer {
public:
    explicit MediaHttpServer(MediaIndexStore& store)
        : store_(store) {
        server_.Get("/media_files", [this](const httplib::Request&, httplib::Response& res) {
            res.set_content(store_.get(), "application/json; charset=utf-8");
            res.status = 200;
        });
    }

    void run() {
        server_.listen("127.0.0.1", 1234);
    }

    void stop() {
        server_.stop();
    }

private:
    httplib::Server server_;
    MediaIndexStore& store_;
};


#endif // KVADRAOSDESKTOPTEAM4_MEDIAHTTPSERVER_H
