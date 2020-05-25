#include "Node.hh"

namespace pcc {
BaseNode::BaseNode(Context* context)
    : context_(context) {}

ProgramNode::ProgramNode(
    Context* context, const std::string& name,
    std::list<std::shared_ptr<BaseNode>> GlobalDeclarations,
    std::list<std::shared_ptr<FunctionNode>> functions)
    : BaseNode(context)
    , name_(name)
    , GlobalDeclarations_(GlobalDeclarations)
    , functions_(functions) {}

Value ProgramNode::CodeGen() {
    for (auto decl : GlobalDeclarations_) {
        decl->CodeGen();
    }
    for (auto function : functions_) {
        function->CodeGen();
    }
    return nullptr;
}
} // namespace pcc
