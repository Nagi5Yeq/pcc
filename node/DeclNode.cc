#include <llvm/IR/IRBuilder.h>

#include "DeclNode.hh"
#include "Log.hh"
#include "TypeIdentifier.hh"

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
    TypeManager* manager = context_->GetTypeManager();
    if (scope_ != nullptr) {
        for (auto&& decl : decls_) {
            std::shared_ptr<Type> type = std::get<1>(decl)->GetType();
            Value value = builder->CreateAlloca(type->GetLLVMType());
            scope_->AddVariable(std::get<0>(decl),
                                {manager->GetPointerType(type), value});
        }
        return nullptr;
    }
    // global variable case
    VariableList* globals = context_->GetGlobals();
    for (auto&& decl : decls_) {
        std::shared_ptr<Type> type = std::get<1>(decl)->GetType();
        llvm::GlobalValue* value = new llvm::GlobalVariable(
            *context_->GetModule(), type->GetLLVMType(), false,
            llvm::GlobalVariable::PrivateLinkage, type->GetZeroInitializer());
        value->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        globals->AddVariable(std::get<0>(decl),
                             {manager->GetPointerType(type), value});
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
    TypeManager* manager = context_->GetTypeManager();
    scope_ = (scope_ == nullptr ? context_->GetGlobals() : scope_);
    for (auto&& decl : decls_) {
        std::shared_ptr<ExprNode> ValueNode = std::get<1>(decl);
        Value value = ValueNode->CodeGen();
        std::shared_ptr<Type> type = ValueNode->GetType();
        llvm::GlobalValue* constant = new llvm::GlobalVariable(
            *context_->GetModule(), type->GetLLVMType(), true,
            llvm::GlobalValue::PrivateLinkage,
            llvm::cast<llvm::Constant>(value));
        constant->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
        scope_->AddVariable(std::get<0>(decl),
                            {manager->GetPointerType(type), constant});
    }
    return nullptr;
}

TypeDeclNode::TypeDeclNode(Context* context, VariableList* scope,
                           std::list<Declaration>&& decls)
    : DeclNode(context, scope)
    , decls_(std::move(decls)) {}

Value TypeDeclNode::CodeGen() {
    scope_ = (scope_ == nullptr ? context_->GetGlobals() : scope_);
    for (auto&& decl : decls_) {
        scope_->AddTypeAlias(std::get<0>(decl), std::get<1>(decl)->GetType());
    }
    return nullptr;
}
} // namespace pcc
