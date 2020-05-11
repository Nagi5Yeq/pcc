#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

namespace pcc {
class Driver;
class Context {
  public:
    Context(const char* name);

  private:
    llvm::IRBuilder<> builder_;
    llvm::Module module_;
    friend class Driver;
};

extern llvm::LLVMContext GlobalContext;
} // namespace pcc

#endif
