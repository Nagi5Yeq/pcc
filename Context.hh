#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace pcc {
using Value = llvm::Value*;
class Context {
  public:
    Context(const char* name);
    llvm::Module* GetModule();
    llvm::IRBuilder<>* GetBuilder();

  private:
    llvm::IRBuilder<> builder_;
    llvm::Module module_;
};

extern llvm::LLVMContext GlobalContext;
} // namespace pcc

#endif
