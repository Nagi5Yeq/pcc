#include <llvm/IR/LLVMContext.h>

#include "Context.hh"

namespace pcc {
llvm::LLVMContext GlobalContext;

Context::Context(const char* name)
    : builder_(GlobalContext)
    , module_(name, GlobalContext) {}

llvm::Module* Context::GetModule() { return &module_; }

llvm::IRBuilder<>* Context::GetBuilder() { return &builder_; }
} // namespace pcc
