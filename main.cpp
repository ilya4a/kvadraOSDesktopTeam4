#include <iostream>

#include "Scanner.h"
#include "SearchOptions.h"
#include  "json_writer.h"
#include <nlohmann/json.hpp>

int main() {

    ScanOptions scan_options = ScanOptions::Builder()
    .set_root_path("/home/ilya/Music/music_flac").build();

    Scanner scanner(scan_options);
    try {
        MediaIndex index = scanner.scan();
        // for (auto&i: index.audio) {
        //     std::cout << i << std::endl;
        // }
        write_media_index(index, "/home/ilya/.media_files");

    }catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
