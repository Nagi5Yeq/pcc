#include <vector>

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
    type_ = value_->GetType();
    return result;
}

L2RCastingNode::L2RCastingNode(Context* context,
                               std::shared_ptr<ExprNode> lvalue)
    : ExprNode(context)
    , lvalue_(lvalue) {}

Value L2RCastingNode::CodeGen() {
    Value left = lvalue_->CodeGen();
    type_ = lvalue_->GetType();
    return context_->GetBuilder()->CreateLoad(type_->GetLLVMType(), left);
}

GetAddressNode::GetAddressNode(Context* context,
                               std::shared_ptr<ExprNode> lvalue)
    : ExprNode(context)
    , lvalue_(lvalue) {}

Value GetAddressNode::CodeGen() {
    Value left = lvalue_->CodeGen();
    type_ = context_->GetTypeManager()->GetPointerType(lvalue_->GetType());
    return left;
}

IdentifierNode::IdentifierNode(Context* context, const std::string& name)
    : ExprNode(context)
    , name_(name) {}

Value IdentifierNode::CodeGen() {
    Variable var = context_->GetCurrentScope()->Find(name_);
    type_ = std::get<0>(var);
    return std::get<1>(var);
}

FunctionCallNode::FunctionCallNode(Context* context, const std::string& name,
                                   std::list<std::shared_ptr<ExprNode>>&& args)
    : ExprNode(context)
    , name_(name)
    , args_(std::move(args)) {}

Value FunctionCallNode::CodeGen() {
    TypeManager* manager = context_->GetTypeManager();
    Function function = context_->FindFunction(name_);
    std::shared_ptr<FunctionType> type = std::get<0>(function);
    if (type == nullptr) {
        Log(LogLevel::PCC_ERROR, "call to undefined function %s",
            name_.c_str());
        return nullptr;
    }
    const std::vector<std::shared_ptr<Type>>& ArgTypes = type->GetArgTypes();
    std::vector<Value> ArgValues(args_.size());
    auto ArgTypeIt = ArgTypes.begin();
    auto ArgValueIt = ArgValues.begin();
    for (auto&& arg : args_) {
        Value argv = arg->CodeGen();
        Value ConvertedArgv =
            manager->CreateCast(*ArgTypeIt, arg->GetType(), argv, context_);
        if (ConvertedArgv == nullptr) {
            Log(LogLevel ::PCC_ERROR,
                "argument type mismatch, the function type is %s, an argument "
                "of type %s needed",
                type->GetCommonName(), (*ArgTypeIt)->GetCommonName());
        }
        *ArgValueIt = ConvertedArgv;
        ++ArgTypeIt, ++ArgValueIt;
    }
    type_ = type->GetReturnType();
    return context_->GetBuilder()->CreateCall(std::get<1>(function), ArgValues);
}
} // namespace pcc
