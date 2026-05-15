#ifndef KVADRAOSDESKTOPTEAM4_SCANNER_H
#define KVADRAOSDESKTOPTEAM4_SCANNER_H

#include "MediaIndex.h"
#include "ScanOptions.h"

class Scanner {
public:
    explicit Scanner(ScanOptions options);
    MediaIndex scan() const;

private:
    ScanOptions options_;
};


#endif // KVADRAOSDESKTOPTEAM4_SCANNER_H
