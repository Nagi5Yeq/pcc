#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>

#include "Driver.hh"
#include "Log.hh"
#include "Version.hh"

int main(int argc, char* argv[]) {
    int rv;
    int level = pcc::ToUnderlying(pcc::PCC_WARNING);
    std::string triple = "";
    while ((rv = getopt(argc, argv, "vVht:")) != -1) {
        switch (rv) {
        case 'v':
            level = (level == 0 ? 0 : level - 1);
            break;
        case 'V':
            pcc::ShowVersion();
        case 'h':
            pcc::ShowHelp();
        case 't':
            triple = optarg;
        default:
            pcc::Log(pcc::PCC_ERROR, "Unknown option \"%c\".", rv);
            pcc::ShowHelp();
        }
    }
    pcc::SetLogLevel(static_cast<pcc::LogLevel>(level));
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    if (triple.empty()) {
        triple = llvm::sys::getDefaultTargetTriple();
    }
    std::string error;
    const llvm::Target* target =
        llvm::TargetRegistry::lookupTarget(triple, error);
    if (!error.empty()) {
        pcc::Log(pcc::PCC_ERROR,
                 "failed to find target with triple %s, reason %s",
                 triple.c_str(), error.c_str());
        return -1;
    }
    std::string cpu = "generic";
    std::string features = "";
    llvm::TargetOptions options;
    llvm::TargetMachine* machine =
        target->createTargetMachine(triple, cpu, features, options, llvm::None);
    llvm::DataLayout layout = machine->createDataLayout();
    if (optind >= argc) {
        pcc::Log(pcc::PCC_ERROR, "No input file specified");
        return -1;
    }
    for (int i = optind; i < argc; i++) {
        pcc::Driver driver(argv[i]);
        llvm::Module* module = driver.GetContext()->GetModule();
        module->setTargetTriple(triple);
        module->setDataLayout(layout);
        std::string filename(argv[i]);
        std::shared_ptr<pcc::ProgramNode> root = driver.Parse(filename);
        if (root != nullptr) {
            root->CodeGen();
        }
        size_t PostfixPos = filename.rfind(".pas");
        std::string OutputFileName;
        if (PostfixPos == std::string::npos) {
            OutputFileName = filename + ".ll";
        } else {
            OutputFileName = filename.substr(0, PostfixPos) + ".ll";
        }
        std::error_code ErrorCode;
        llvm::raw_fd_ostream Output(OutputFileName, ErrorCode);
        driver.GetContext()->GetModule()->print(Output, nullptr);
    }
    return 0;
}
