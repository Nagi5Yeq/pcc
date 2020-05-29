#include <llvm/IR/LLVMContext.h>

#include "Context.hh"
#include "Log.hh"

namespace pcc {
// llvm needs a LLVMContext for every thread, but we only have one thread, so we
// just use this global context.
llvm::LLVMContext GlobalLLVMContext;

Context::Context(const char* name)
    : builder_(GlobalLLVMContext)
    , module_(name, GlobalLLVMContext)
    , JumpingFlag_(false) {
    scopes_.push(&globals_);
}

TypeManager* Context::GetTypeManager() { return &TypeManager_; }

VariableList* Context::GetGlobals() { return &globals_; }

llvm::Module* Context::GetModule() { return &module_; }

llvm::IRBuilder<>* Context::GetBuilder() { return &builder_; }

VariableList* Context::GetCurrentScope() { return scopes_.top(); }

void Context::PushScope(VariableList* scope) { scopes_.push(scope); }

void Context::PopScope() { scopes_.pop(); }

std::shared_ptr<FunctionType> Context::GetFunctionType(std::string name) {
    decltype(FuncTypes_)::iterator result = FuncTypes_.find(name);
    if (result == FuncTypes_.end()) {
        return nullptr;
    }
    return std::get<1>(*result);
}

void Context::AddFunctionType(std::string name,
                              std::shared_ptr<FunctionType> type) {
    FuncTypes_.insert({name, type});
}

void Context::PushBreakDestination(llvm::BasicBlock* dest) {
    BreakDestinations_.push(dest);
}

void Context::PopBreakDestination() { BreakDestinations_.pop(); }

llvm::BasicBlock* Context::GetBreakDestination() {
    return BreakDestinations_.empty() ? nullptr : BreakDestinations_.top();
}

void Context::PushContinueDestination(llvm::BasicBlock* dest) {
    ContinueDestinations_.push(dest);
}

void Context::PopContinueDestination() { ContinueDestinations_.pop(); }

llvm::BasicBlock* Context::GetContinueDestination() {
    return ContinueDestinations_.empty() ? nullptr
                                         : ContinueDestinations_.top();
}

void Context::SetJumpingFlag(bool flag) { JumpingFlag_ = flag; }

bool Context::GetJumpingFlag() { return JumpingFlag_; }
} // namespace pcc
