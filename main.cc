#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

#include <llvm/Support/raw_ostream.h>

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
        pcc::Driver driver(argv[i]);
        std::string filename(argv[i]);
        std::shared_ptr<pcc::ProgramNode> root = driver.Parse(filename);
        if (root != nullptr) {
            root->CodeGen();
        }
        driver.GetContext()->GetModule()->print(llvm::errs(), nullptr);
    }

    return 0;
}
