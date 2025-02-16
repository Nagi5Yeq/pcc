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
    std::shared_ptr<PointerType> type =
        std::dynamic_pointer_cast<PointerType>(lvalue_->GetType());
    if (type == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "type %s is not a valid pointer type, this lvalue can't be loaded",
            lvalue_->GetType()->GetCommonName());
    }
    type_ = type->GetElementType();
    return context_->GetBuilder()->CreateLoad(type_->GetLLVMType(), left);
}

PointerAccessNode::PointerAccessNode(Context* context,
                                     std::shared_ptr<ExprNode> lhs,
                                     std::shared_ptr<ExprNode> rhs)
    : ExprNode(context)
    , lhs_(lhs)
    , rhs_(rhs) {}

Value PointerAccessNode::CodeGen() {
    Value left = lhs_->CodeGen();
    Value right = rhs_->CodeGen();
    std::shared_ptr<PointerType> pointer =
        std::dynamic_pointer_cast<PointerType>(lhs_->GetType());
    if (pointer == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "type %s is not a valid type for pointer access operator[]",
            lhs_->GetType()->GetCommonName());
        return nullptr;
    }
    Value CastedRight = context_->GetTypeManager()->CreateCast(
        pointer->GetIndexType(), rhs_->GetType(), right, context_);
    type_ = pointer;
    return context_->GetBuilder()->CreateInBoundsGEP(left, CastedRight);
}

ArrayAccessNode::ArrayAccessNode(Context* context,
                                 std::shared_ptr<ExprNode> lhs,
                                 std::shared_ptr<ExprNode> rhs)
    : ExprNode(context)
    , lhs_(lhs)
    , rhs_(rhs) {}

Value ArrayAccessNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    Value left = lhs_->CodeGen();
    Value right = rhs_->CodeGen();
    std::shared_ptr<PointerType> LeftType =
        std::dynamic_pointer_cast<PointerType>(lhs_->GetType());
    std::shared_ptr<Type> RightType = rhs_->GetType();
    if (LeftType == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "type %s is not a valid type for array access operator[]",
            lhs_->GetType()->GetCommonName());
        return nullptr;
    }
    std::shared_ptr<Type> type = LeftType->GetElementType();
    std::shared_ptr<ArrayType> array =
        std::dynamic_pointer_cast<ArrayType>(type);
    if (array != nullptr) {
        std::shared_ptr<Type> IndexType = array->GetIndexType();
        Value indices[2] = {llvm::ConstantInt::get(
            manager->GetPointerIndexType()->GetLLVMType(), 0)};
        if (array->GetIsZeroStarted() == false) {
            Value CastedRight =
                manager->CreateCast(IndexType, RightType, right, context_);
            auto AdjustedRight = IndexType->CreateBinaryOperation(
                BinaryOperator::SUB, CastedRight, array->GetIndexStart(),
                context_);
            indices[1] = AdjustedRight;
        } else {
            indices[1] = right;
        }
        type_ = manager->GetPointerType(array->GetElementType());
        return builder->CreateInBoundsGEP(left, indices);
    }
    std::shared_ptr<PointerType> pointer =
        std::dynamic_pointer_cast<PointerType>(type);
    if (pointer != nullptr) {
        Value PointerValue = builder->CreateLoad(pointer->GetLLVMType(), left);
        Value CastedRight = manager->CreateCast(
            pointer->GetIndexType(), rhs_->GetType(), right, context_);
        type_ = pointer;
        return builder->CreateInBoundsGEP(PointerValue, CastedRight);
    }
    Log(LogLevel::PCC_ERROR,
        "type %s is not a valid lvalue type for array access operator[]",
        lhs_->GetType()->GetCommonName());
    return nullptr;
}

DereferenceNode::DereferenceNode(Context* context,
                                 std::shared_ptr<ExprNode> lhs)
    : ExprNode(context)
    , lhs_(lhs) {}

Value DereferenceNode::CodeGen() {
    Value value = lhs_->CodeGen();
    std::shared_ptr<PointerType> pointer =
        std::dynamic_pointer_cast<PointerType>(lhs_->GetType());
    if (pointer == nullptr) {
        Log(LogLevel::PCC_ERROR, "type %s is not a valid type for dereference",
            lhs_->GetType()->GetCommonName());
        return nullptr;
    }
    Value zero = llvm::ConstantInt::get(
        context_->GetTypeManager()->GetPointerIndexType()->GetLLVMType(), 0);
    type_ = pointer;
    return context_->GetBuilder()->CreateInBoundsGEP(value, zero);
}

MemberAccessNode::MemberAccessNode(Context* context,
                                   std::shared_ptr<ExprNode> lhs,
                                   std::string&& rhs)
    : ExprNode(context)
    , lhs_(lhs)
    , rhs_(std::move(rhs)) {}

Value MemberAccessNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    Value left = lhs_->CodeGen();
    std::shared_ptr<PointerType> LeftType =
        std::dynamic_pointer_cast<PointerType>(lhs_->GetType());
    if (LeftType == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "type %s is not a valid type for member access",
            lhs_->GetType()->GetCommonName());
        return nullptr;
    }
    std::shared_ptr<RecordType> LeftInner =
        std::dynamic_pointer_cast<RecordType>(LeftType->GetElementType());
    if (LeftInner == nullptr) {
        Log(LogLevel::PCC_ERROR, "type %s is not a record type",
            LeftType->GetElementType()->GetCommonName());
        return nullptr;
    }
    int MemberIndex;
    std::shared_ptr<Type> MemberType;
    std::tie(MemberIndex, MemberType) = LeftInner->GetMember(rhs_);
    llvm::Type* IndexLLLVMType =
        manager->GetRecordMemberIndexType()->GetLLVMType();
    Value indices[2] = {llvm::ConstantInt::get(IndexLLLVMType, 0),
                        llvm::ConstantInt::get(IndexLLLVMType, MemberIndex)};
    type_ = manager->GetPointerType(MemberType);
    return builder->CreateInBoundsGEP(left, indices);
}

IdentifierNode::IdentifierNode(Context* context, const std::string& name)
    : ExprNode(context)
    , name_(name) {}

Value IdentifierNode::CodeGen() {
    Variable var = context_->GetCurrentScope()->FindVariable(name_);
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
    if (type->IsVariadic() == false && ArgTypes.size() != args_.size()) {
        Log(LogLevel ::PCC_ERROR,
            "argument number mismatch, %d wanted, %d given", ArgTypes.size(),
            args_.size());
        return nullptr;
    }
    std::vector<Value> ArgValues;
    ArgValues.reserve(args_.size());
    auto ArgIt = args_.begin();
    for (auto&& ArgType : ArgTypes) {
        Value argv = (*ArgIt)->CodeGen();
        Value ConvertedArgv =
            manager->CreateCast(ArgType, (*ArgIt)->GetType(), argv, context_);
        if (ConvertedArgv == nullptr) {
            Log(LogLevel ::PCC_ERROR,
                "argument type mismatch, the function type is %s, an argument "
                "of type %s needed",
                type->GetCommonName(), ArgType->GetCommonName());
        }
        ArgValues.push_back(ConvertedArgv);
        ++ArgIt;
    }
    while (ArgIt != args_.end()) {
        ArgValues.push_back((*ArgIt)->CodeGen());
        ++ArgIt;
    }
    type_ = type->GetReturnType();
    return context_->GetBuilder()->CreateCall(std::get<1>(function), ArgValues);
}
} // namespace pcc
