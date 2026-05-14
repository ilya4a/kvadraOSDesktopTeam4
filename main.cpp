#include <iostream>

#include "Scanner.h"
#include "SearchOptions.h"
int main() {

    ScanOptions scan_options = ScanOptions::Builder()
    .set_root_path("/home/ilya/Music/music_flac").build();

    Scanner scanner(scan_options);
    try {
        MediaIndex index = scanner.scan();
        for (auto&i: index.audio) {
            std::cout << i << std::endl;
        }

    }catch (...) {
        std::cerr << "scanner.scan();" << std::endl;
    }

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
