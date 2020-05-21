#include "ExprNode.hh"
#include "Log.hh"
#include "TypeManager.hh"

namespace pcc {
ExprNode::ExprNode(Context* context)
    : BaseNode(context) {}

std::shared_ptr<Type> ExprNode::GetType() { return type_; }

BinaryExprNode::BinaryExprNode(Context* context, BinaryOperator op,
                               std::shared_ptr<ExprNode> left,
                               std::shared_ptr<ExprNode> right)
    : ExprNode(context)
    , op_(op)
    , left_(left)
    , right_(right) {}

Value BinaryExprNode::CodeGen() {
    Value lhs = left_->CodeGen();
    Value rhs = right_->CodeGen();
    if (lhs == nullptr || rhs == nullptr) {
        return nullptr;
    }
    std::pair<std::shared_ptr<Type>, Value> result =
        context_->GetTypeManager()->CreateOperation(
            op_, left_->GetType(), right_->GetType(), lhs, rhs, context_);
    if (std::get<1>(result) == nullptr) {
        Log(LogLevel::PCC_ERROR, "failed to generate a binary expression");
        return nullptr;
    }
    type_ = std::get<0>(result);
    return std::get<1>(result);
}

UnaryExprNode::UnaryExprNode(Context* context, UnaryOperator op,
                             std::shared_ptr<ExprNode> value)
    : ExprNode(context)
    , op_(op)
    , value_(value) {}

Value UnaryExprNode::CodeGen() {
    Value v = value_->CodeGen();
    if (v == nullptr) {
        return nullptr;
    }
    Value result = context_->GetTypeManager()->CreateOperation(
        op_, value_->GetType(), v, context_);
    if (result == nullptr) {
        Log(LogLevel::PCC_ERROR, "failed to generate a unary operation");
    }
    return result;
}
} // namespace pcc
