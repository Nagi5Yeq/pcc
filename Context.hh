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
    VariableList GetGlobals();
    llvm::Module* GetModule();
    llvm::IRBuilder<>* GetBuilder();
    VariableList* GetCurrentScope();
    void PushScope(VariableList* scope);
    void PopScope();
    std::shared_ptr<FunctionType> GetFunctionType(std::string name);
    void AddFunctionType(std::string name, std::shared_ptr<FunctionType> type);

  private:
    TypeManager TypeManager_;
    VariableList globals_;
    std::stack<VariableList*> scopes_;
    llvm::IRBuilder<> builder_;
    llvm::Module module_;
    std::unordered_map<std::string, std::shared_ptr<FunctionType>> FuncTypes_;
};

extern llvm::LLVMContext GlobalLLVMContext;
} // namespace pcc

#endif
