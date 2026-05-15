
#ifndef KVADRAOSDESKTOPTEAM4_CLI_H
#define KVADRAOSDESKTOPTEAM4_CLI_H

#include "../ScanOptions.h"

void run_scan(ScanOptions scan_options, std::filesystem::path out_path, std::string str_delay = "30s");
int run_app(int argc, char** argv);


#endif // KVADRAOSDESKTOPTEAM4_CLI_H
