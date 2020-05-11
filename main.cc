#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

#include "Driver.hh"
#include "Log.hh"
#include "Version.hh"

int main(int argc, char* argv[]) {
    int rv;
    pcc::LogLevel level = pcc::PCC_WARNING;
    while ((rv = getopt(argc, argv, "vVh")) != -1) {
        switch (rv) {
        case 'v':
            level = pcc::PCC_DEBUG;
            break;
        case 'V':
            pcc::ShowVersion();
        case 'h':
            pcc::ShowHelp();
        default:
            pcc::Log(pcc::PCC_ERROR, "Unknown option \"%c\".", rv);
            pcc::ShowHelp();
        }
    }
    if (optind >= argc) {
        pcc::Log(pcc::PCC_ERROR, "No input file specified");
        return 0;
    }
    for (int i = optind; i < argc; i++) {
        pcc::Context context(argv[i]);
        pcc::Driver driver(&context);
        driver.Parse(argv[i]);
    }
    return 0;
}
