#include "utils/MediaHttpServer.h"

MediaHttpServer::MediaHttpServer(MediaIndexStore &store) : store_(store) {
    server_.Get("/media_files", [this](const httplib::Request &, httplib::Response &res) {
        res.set_content(store_.get(), "application/json; charset=utf-8");
        res.status = 200;
    });
}
void MediaHttpServer::run() {
    server_.listen("127.0.0.1", 1234);
}

void MediaHttpServer::stop() {
    server_.stop();
}
