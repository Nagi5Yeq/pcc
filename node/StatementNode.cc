#include "StatementNode.hh"

namespace pcc {
template <> Value StatementListNode::CodeGen() {
    Value result = nullptr;
    for (auto& child : childs_) {
        result = child->CodeGen();
    }
    return result;
}

EmptyStatementNode::EmptyStatementNode(Context* context)
    : BaseNode(context) {}

Value EmptyStatementNode::CodeGen() { return nullptr; }

AssignStatementNode::AssignStatementNode(Context* context,
                                         std::shared_ptr<ExprNode> lhs,
                                         std::shared_ptr<ExprNode> rhs)
    : BaseNode(context)
    , lhs_(lhs)
    , rhs_(rhs) {}

Value AssignStatementNode::CodeGen() {
    Value LeftValue = lhs_->CodeGen();
    Value RightValue = rhs_->CodeGen();
    RightValue = context_->GetTypeManager()->CreateCast(
        lhs_->GetType(), rhs_->GetType(), RightValue, context_);
    context_->GetBuilder()->CreateStore(RightValue, LeftValue);
    return nullptr;
}
} // namespace pcc
