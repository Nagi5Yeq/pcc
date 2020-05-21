#include <llvm/IR/LLVMContext.h>

#include "Context.hh"

namespace pcc {
// llvm needs a LLVMContext for every thread, but we only have one thread, so we
// just use this global context.
llvm::LLVMContext GlobalLLVMContext;

Context::Context(const char* name)
    : builder_(GlobalLLVMContext)
    , module_(name, GlobalLLVMContext) {}

TypeManager* Context::GetTypeManager() { return &TypeManager_; }

llvm::Module* Context::GetModule() { return &module_; }

llvm::IRBuilder<>* Context::GetBuilder() { return &builder_; }
} // namespace pcc
