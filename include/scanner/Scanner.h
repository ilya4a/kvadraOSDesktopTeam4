#ifndef KVADRAOSDESKTOPTEAM4_SCANNER_H
#define KVADRAOSDESKTOPTEAM4_SCANNER_H

#include "ScanOptions.h"
#include "utils/MediaIndex.h"

class Scanner {
  public:
    explicit Scanner(ScanOptions options);
    MediaIndex scan() const;

  private:
    ScanOptions options_;
};

#endif // KVADRAOSDESKTOPTEAM4_SCANNER_H
