#include <iostream>
#include "json_writer.h"
#include  "cli.h"

int main(int argc, char** argv) {

    try {
        run_app(argc, argv);
    }catch (std::exception&e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}