#include <llvm/IR/IRBuilder.h>

#include "DeclNode.hh"

namespace pcc {
DeclNode::DeclNode(Context* context, VariableList* scope)
    : BaseNode(context)
    , scope_(scope) {}

void DeclNode::SetScope(VariableList* scope) { scope_ = scope; }

VarDeclNode::VarDeclNode(Context* context, VariableList* scope,
                         std::list<Declaration>&& decls)
    : DeclNode(context, scope)
    , decls_(std::move(decls)) {}

// if the scope is nullptr, this vardecl is in global scope. global variables
// are static, while local variables are allocated on stack.
Value VarDeclNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    if (scope_ != nullptr) {
        for (auto&& decl : decls_) {
            Value value =
                builder->CreateAlloca(std::get<1>(decl)->GetLLVMType());
            scope_->Add(std::get<0>(decl), {std::get<1>(decl), value});
        }
        return nullptr;
    }
    // global variable case
    VariableList* globals = context_->GetGlobals();
    for (auto&& decl : decls_) {
        llvm::Constant* value = new llvm::GlobalVariable(
            *context_->GetModule(), std::get<1>(decl)->GetLLVMType(), false,
            llvm::GlobalVariable::PrivateLinkage, nullptr);
        globals->Add(std::get<0>(decl), {std::get<1>(decl), value});
    }
    return nullptr;
}

ConstDeclNode::ConstDeclNode(Context* context, VariableList* scope,
                             std::list<Constant>&& decls)
    : DeclNode(context, scope)
    , decls_(std::move(decls)) {}

// for constants, we don't need to allocate them on stack, both global and local
// constants are static.
Value ConstDeclNode::CodeGen() {
    scope_ = (scope_ == nullptr ? context_->GetGlobals() : scope_);
    for (auto&& decl : decls_) {
        std::shared_ptr<ExprNode> ValueNode = std::get<1>(decl);
        Value value = ValueNode->CodeGen();
        std::shared_ptr<Type> type = ValueNode->GetType();
        llvm::Constant* constant = new llvm::GlobalVariable(
            *context_->GetModule(), type->GetLLVMType(), true,
            llvm::GlobalValue::PrivateLinkage,
            llvm::cast<llvm::Constant>(value));
        scope_->Add(std::get<0>(decl), {type, constant});
    }
    return nullptr;
}

} // namespace pcc
