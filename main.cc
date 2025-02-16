#include <fstream>
#include <iostream>

#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Scalar.h>

#include "AsciiTraveler.hh"
#include "DotTraveler.hh"
#include "Driver.hh"
#include "Log.hh"
#include "Version.hh"

void compile(llvm::Module* module, llvm::raw_pwrite_stream& output,
             llvm::CodeGenFileType type, llvm::TargetMachine* machine);

int main(int argc, char* argv[]) {
    int rv;
    int level = pcc::ToUnderlying(pcc::PCC_WARNING);
    std::string OutputFileName;
    enum { IR, ASSEMBLY, OBJECT } CompileType = IR;
    int OptLevel = 2;
    bool AsciiTravel = false, DotTravel = false;

    pcc::SetExecutableName(argv[0]);
    while ((rv = getopt(argc, argv, "vVhiScado:O:")) != -1) {
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
        case 'a':
            AsciiTravel = true;
            break;
        case 'd':
            DotTravel = true;
            break;
        case 'o':
            OutputFileName = optarg;
            break;
        case 'O':
            OptLevel = optarg[0] - '0';
            if (OptLevel > 3 || OptLevel < 0) {
                pcc::Log(pcc::PCC_ERROR, "%c is not a valid optimization level",
                         optarg[0]);
                pcc::ShowHelp();
            }
            break;
        default:
            pcc::Log(pcc::PCC_ERROR, "unknown option");
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
                 "failed to find target with triple %s, reason %s",
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
        pcc::Log(pcc::PCC_ERROR, "no input file specified");
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
                pcc::Log(pcc::PCC_WARNING, "%s", msg.c_str());
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
        llvm::legacy::PassManager manager;
        manager.add(llvm::createCFGSimplificationPass());
        manager.add(llvm::createMergedLoadStoreMotionPass());
        manager.add(llvm::createReassociatePass());
        manager.add(llvm::createLoopSimplifyCFGPass());
        switch (CompileType) {
        case IR:
            manager.add(llvm::createPrintModulePass(output));
            break;
        case ASSEMBLY:
            machine->addPassesToEmitFile(
                manager, output, nullptr,
                llvm::CodeGenFileType::CGFT_AssemblyFile);
            break;
        case OBJECT:
            machine->addPassesToEmitFile(
                manager, output, nullptr,
                llvm::CodeGenFileType::CGFT_ObjectFile);
            break;
        default:
            break;
        }
        manager.run(*module);
        pcc::Log(pcc::PCC_INFO, "%s compiled to %s", argv[i],
                 OutputFileName.c_str());
        OutputFileName.clear();
        if (AsciiTravel == true) {
            pcc::Log(pcc::PCC_INFO, "dumping abstract syntax tree of %s",
                     argv[i]);
            pcc::AsciiTraveler traveler(std::cerr);
            traveler.Travel(root);
        }
        if (DotTravel == true) {
            std::string DotOutputName;
            size_t PostfixPos = filename.rfind(".pas");
            if (PostfixPos == std::string::npos) {
                DotOutputName = filename + ".dot";
            } else {
                DotOutputName = filename.substr(0, PostfixPos) + ".dot";
            }
            std::ofstream DotOut(DotOutputName);
            if (DotOut.is_open() == false) {
                pcc::Log(pcc::PCC_ERROR, "failed to open dot outpt file %s",
                         DotOutputName.c_str());
            }
            pcc::DotTraveler traveler(DotOut);
            traveler.Travel(root);
            DotOut.close();
        }
    }
    return 0;
}
