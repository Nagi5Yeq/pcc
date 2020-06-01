#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>

#include "Driver.hh"
#include "Log.hh"
#include "Version.hh"

void compile(llvm::Module* module, llvm::raw_pwrite_stream& output,
             llvm::CodeGenFileType type, llvm::TargetMachine* machine);

int main(int argc, char* argv[]) {
    int rv;
    int level = pcc::PCC_WARNING;
    std::string OutputFileName;
    enum { IR, ASSEMBLY, OBJECT } CompileType = IR;
    int OptLevel = 2;

    pcc::SetExecutableName(argv[0]);
    while ((rv = getopt(argc, argv, "vVhiSco:O:")) != -1) {
        switch (rv) {
        case 'v':
            level = (level == 0 ? 0 : level - 1);
            break;
        case 'V':
            pcc::ShowVersion();
        case 'h':
            pcc::ShowHelp();
        case 'i':
            CompileType = IR;
            break;
        case 'S':
            CompileType = ASSEMBLY;
            break;
        case 'c':
            CompileType = OBJECT;
            break;
        case 'o':
            OutputFileName = optarg;
            break;
        case 'O':
            OptLevel = optarg[0] - '0';
            if (OptLevel > 3 || OptLevel < 0) {
                pcc::Log(pcc::PCC_ERROR,
                         "{0} is not a valid optimization level", optarg[0]);
                pcc::ShowHelp();
            }
            break;
        default:
            pcc::Log(pcc::PCC_ERROR, "Unknown option");
            pcc::ShowHelp();
        }
    }

    pcc::SetLogLevel(static_cast<pcc::LogLevel>(level));
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeNativeTargetAsmPrinter();
    std::string triple = llvm::sys::getDefaultTargetTriple();
    std::string error;
    const llvm::Target* target =
        llvm::TargetRegistry::lookupTarget(triple, error);
    if (!error.empty()) {
        pcc::Log(pcc::PCC_ERROR,
                 "failed to find target with triple {0}, reason {1}",
                 triple.c_str(), error.c_str());
        return -1;
    }
    const llvm::CodeGenOpt::Level optimization[] = {
        llvm::CodeGenOpt::None, llvm::CodeGenOpt::Less,
        llvm::CodeGenOpt::Default, llvm::CodeGenOpt::Aggressive};
    std::string cpu = "generic";
    std::string features = "";
    llvm::TargetOptions options;
    llvm::TargetMachine* machine =
        target->createTargetMachine(triple, cpu, features, options, llvm::None,
                                    llvm::None, optimization[OptLevel]);
    llvm::DataLayout layout = machine->createDataLayout();

    if (optind >= argc) {
        pcc::Log(pcc::PCC_ERROR, "No input file specified");
        return -1;
    }
    if (!OutputFileName.empty() && optind != argc - 1) {
        pcc::Log(pcc::PCC_WARNING,
                 "more than one source file specified, the output file name "
                 "will be ignored");
        OutputFileName.clear();
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
            std::string msg;
            llvm::raw_string_ostream MsgStream(msg);
            llvm::verifyModule(*module, &MsgStream);
            MsgStream.flush();
            if (!msg.empty()) {
                pcc::Log(pcc::PCC_WARNING, "{0}", msg.c_str());
            }
        } else {
            continue;
        }
        if (OutputFileName.empty()) {
            const char* postfix[] = {".ll", ".s", ".o"};
            size_t PostfixPos = filename.rfind(".pas");
            if (PostfixPos == std::string::npos) {
                OutputFileName = filename + postfix[CompileType];
            } else {
                OutputFileName =
                    filename.substr(0, PostfixPos) + postfix[CompileType];
            }
        }
        std::error_code ErrorCode;
        llvm::raw_fd_ostream output(OutputFileName, ErrorCode);
        switch (CompileType) {
        case IR:
            module->print(output, nullptr);
            break;
        case ASSEMBLY:
            compile(module, output, llvm::CGFT_AssemblyFile, machine);
            break;
        case OBJECT:
            compile(module, output, llvm::CGFT_ObjectFile, machine);
            break;
        default:
            break;
        }
        pcc::Log(pcc::PCC_INFO, "{0} compiled to {1}", argv[i],
                 OutputFileName.c_str());
        OutputFileName.clear();
    }
    return 0;
}

void compile(llvm::Module* module, llvm::raw_pwrite_stream& output,
             llvm::CodeGenFileType type, llvm::TargetMachine* machine) {
    llvm::legacy::PassManager manager;
    machine->addPassesToEmitFile(manager, output, nullptr, type);
    manager.run(*module);
}
