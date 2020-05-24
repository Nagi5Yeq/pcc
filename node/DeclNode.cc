#include "DeclNode.hh"

#include <llvm/IR/IRBuilder.h>

namespace pcc {
VarDeclNode::VarDeclNode(Context* context, VariableList* scope,
                         std::list<Declaration> decls)
    : BaseNode(context)
    , scope_(scope)
    , decls_(decls) {}

void VarDeclNode::SetScope(VariableList* scope) { scope_ = scope; }

Value VarDeclNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    for (auto& decl : decls_) {
        Value value = builder->CreateAlloca(std::get<1>(decl)->GetLLVMType());
        scope_->Add(std::get<0>(decl), {std::get<1>(decl), value});
    }
    return nullptr;
}

} // namespace pcc
