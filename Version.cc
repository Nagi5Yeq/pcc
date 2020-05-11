#include "Version.hh"

namespace pcc {

PCC_NORETURN void ShowVersion() {
    std::cerr
        << PCC_NAME " version " PCC_VERSION_STRING "\n"
        << "Copyright (C) 2020 Hanjie Wu <nagi@zju.edu.cn>\n"
           "Platform: " PCC_OSNAME "\n"
           "Built on " __DATE__ " " __TIME__ "\n\n"
           "This program is free software; you can redistribute it and/or "
           "modify\n"
           "it under the terms of the GNU General Public License as published "
           "by\n"
           "the Free Software Foundation; either version 2 of the License, or\n"
           "(at your option) any later version."
        << std::endl;
    std::exit(0);
}

PCC_NORETURN void ShowHelp() {
    std::cerr << PCC_NAME " version " PCC_VERSION_STRING "\n"
              << "Usage: " PCC_NAME " [-vVh] file...\n"
              << "    -v   Verbose mode.\n"
                 "    -V   Show version and exit.\n"
                 "    -h   Show this help and exit."
              << std::endl;
    std::exit(0);
}

} // namespace pcc
