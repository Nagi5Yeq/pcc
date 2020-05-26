#include <llvm/Config/llvm-config.h>

#include "Version.hh"

namespace pcc {

PCC_NORETURN void ShowVersion() {
    std::cerr
        << PCC_NAME " version " PCC_VERSION_STRING "\n"
        << "Copyright (C) 2020 Hanjie Wu <nagi@zju.edu.cn>\n\n"
           "This program is free software; you can redistribute it and/or "
           "modify\n"
           "it under the terms of the GNU General Public License as published "
           "by\n"
           "the Free Software Foundation; either version 2 of the License, or\n"
           "(at your option) any later version.\n\n"
           "Configurations:\n"
           "  LLVM Version:   " LLVM_VERSION_STRING "\n"
           "  Host Platform:  " PCC_OSNAME "\n"
           "  Default Target: " LLVM_DEFAULT_TARGET_TRIPLE "\n"
           "  Built on:       " __DATE__ " " __TIME__
        << std::endl;
    std::exit(0);
}

PCC_NORETURN void ShowHelp() {
    std::cerr << PCC_NAME " version " PCC_VERSION_STRING "\n"
              << "Usage: " PCC_NAME " [-vVh] file...\n"
              << "    -v   Verbose mode.\n"
                 "    -vv  Very verbose mode.\n"
                 "    -V   Show version and exit.\n"
                 "    -h   Show this help and exit."
              << std::endl;
    std::exit(0);
}

} // namespace pcc
