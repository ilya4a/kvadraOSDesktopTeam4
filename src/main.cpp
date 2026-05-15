#include <iostream>
#include "../include/app/cli.h"
#include "../include/storage/json_writer.h"

int main(int argc, char** argv) {

    try {
        run_app(argc, argv);
    }catch (std::exception&e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}