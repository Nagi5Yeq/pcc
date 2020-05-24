#include <llvm/IR/LLVMContext.h>

#include "Context.hh"

namespace pcc {
// llvm needs a LLVMContext for every thread, but we only have one thread, so we
// just use this global context.
llvm::LLVMContext GlobalLLVMContext;

Context::Context(const char* name)
    : builder_(GlobalLLVMContext)
    , module_(name, GlobalLLVMContext) {
    scopes_.push(&globals_);
}

TypeManager* Context::GetTypeManager() { return &TypeManager_; }

VariableList Context::GetGlobals() { return &globals_; }

llvm::Module* Context::GetModule() { return &module_; }

llvm::IRBuilder<>* Context::GetBuilder() { return &builder_; }

VariableList* Context::GetCurrentScope() { return scopes_.top(); }

void Context::PushScope(VariableList* scope) { scopes_.push(scope); }

void Context::PopScope() { scopes_.pop(); }
} // namespace pcc
