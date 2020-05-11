#ifndef PCC_DRIVER_H
#define PCC_DRIVER_H

#include <cstdio>

#include "Context.hh"
#include "Parser.hh"

#define YY_DECL pcc::Parser::symbol_type yylex(pcc::Driver& driver)

YY_DECL;

namespace pcc {
class Driver {
  public:
    Driver(Context* context);
    ~Driver();
    int StartLexer(const std::string& filename);
    void StopLexer();
    void Parse(const std::string& filename);
    llvm::Module* GetModule();
    llvm::IRBuilder<>* GetBuilder();

    Context* context_;
};
} // namespace pcc

#endif
