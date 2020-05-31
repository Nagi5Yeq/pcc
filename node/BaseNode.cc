#include "Node.hh"

namespace pcc {
BaseNode::BaseNode(Context* context)
    : context_(context) {}

ProgramNode::ProgramNode(
    Context* context, std::string&& name,
    std::list<std::shared_ptr<DeclNode>>&& GlobalDeclarations,
    std::list<std::shared_ptr<FunctionNode>>&& functions)
    : BaseNode(context)
    , name_(std::move(name))
    , GlobalDeclarations_(std::move(GlobalDeclarations))
    , functions_(std::move(functions)) {}

Value ProgramNode::CodeGen() {
    context_->GetModule()->setModuleIdentifier(name_);
    for (auto&& decl : GlobalDeclarations_) {
        decl->CodeGen();
    }
    for (auto&& function : functions_) {
        function->CodeGen();
    }
    return nullptr;
}
} // namespace pcc
