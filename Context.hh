#ifndef PCC_CONTEXT_H
#define PCC_CONTEXT_H

#include <stack>
#include <unordered_map>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "TypeManager.hh"
#include "VariableList.hh"

namespace pcc {
class Context {
  public:
    Context(const char* name);
    TypeManager* GetTypeManager();
    VariableList* GetGlobals();
    llvm::Module* GetModule();
    llvm::IRBuilder<>* GetBuilder();
    VariableList* GetCurrentScope();
    void PushScope(VariableList* scope);
    void PopScope();
    std::shared_ptr<FunctionType> GetFunctionType(std::string name);
    void AddFunctionType(std::string name, std::shared_ptr<FunctionType> type);

    // every time we enter a while, repeat or for statement, we push the
    // destination of break and continue.
    void PushBreakDestination(llvm::BasicBlock* dest);
    void PopBreakDestination();
    llvm::BasicBlock* GetBreakDestination();
    void PushContinueDestination(llvm::BasicBlock* dest);
    void PopContinueDestination();
    llvm::BasicBlock* GetContinueDestination();
    void SetJumpingFlag(bool flag);
    bool GetJumpingFlag();

  private:
    llvm::IRBuilder<> builder_;
    llvm::Module module_;
    TypeManager TypeManager_;
    VariableList globals_;
    std::stack<VariableList*> scopes_;
    std::unordered_map<std::string, std::shared_ptr<FunctionType>> FuncTypes_;
    std::stack<llvm::BasicBlock*> BreakDestinations_;
    std::stack<llvm::BasicBlock*> ContinueDestinations_;
    // if statments always link then and else block with the endif block, it
    // assumes that no instruction will jump out from the body. break and
    // continue break this assumption, which makes IfStatementNode creates extra
    // br instructions, and LLVM will complain "No predecessors!". we use this
    // flag to tell IfStatementNode if there is a break or continue inside it.
    bool JumpingFlag_;
};

extern llvm::LLVMContext GlobalLLVMContext;
} // namespace pcc

#endif
