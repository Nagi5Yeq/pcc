#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "TypeManager.hh"

namespace pcc {
class Context {
  public:
    Context(const char* name);
    TypeManager* GetTypeManager();
    llvm::Module* GetModule();
    llvm::IRBuilder<>* GetBuilder();

  private:
    TypeManager TypeManager_;
    llvm::IRBuilder<> builder_;
    llvm::Module module_;
};

extern llvm::LLVMContext GlobalLLVMContext;
} // namespace pcc

#endif
