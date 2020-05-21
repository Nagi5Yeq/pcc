#include "TypeManager.hh"
#include "Context.hh"
#include "Log.hh"

namespace pcc {
TypeManager::TypeManager()
    : builtins_{std::make_shared<BooleanType>(), std::make_shared<CharType>(),
                std::make_shared<IntegerType>(), std::make_shared<RealType>()} {
    PointerDifferenceType_ = GetBuiltinType(BuiltinType::INTEGER);
    PointerIndexType_ = GetBuiltinType(BuiltinType::INTEGER);
    builtins_.push_back(std::make_shared<PointerType>(
        GetBuiltinType(BuiltinType::CHAR), PointerDifferenceType_,
        PointerIndexType_, "string"));
}

std::shared_ptr<Type> TypeManager::GetBuiltinType(BuiltinType type) {
    return builtins_[ToUnderlying(type)];
}

std::pair<std::shared_ptr<Type>, Value>
    TypeManager::CreateOperation(BinaryOperator op,
                                 std::shared_ptr<Type> LeftType,
                                 std::shared_ptr<Type> RightType, Value left,
                                 Value right, Context* context) {
    llvm::IRBuilder<>* builder = context->GetBuilder();
    std::shared_ptr<IntegerBaseType> LeftIntegerBaseType =
        std::dynamic_pointer_cast<IntegerBaseType>(LeftType);
    std::shared_ptr<IntegerBaseType> RightIntegerBaseType =
        std::dynamic_pointer_cast<IntegerBaseType>(RightType);
    std::shared_ptr<PointerType> LeftPointerType =
        std::dynamic_pointer_cast<PointerType>(LeftType);
    std::shared_ptr<ArrayType> LeftArrayType =
        std::dynamic_pointer_cast<ArrayType>(LeftType);

    // array or pointer access
    if (op == BinaryOperator::ARRAY_ACCESS) {
        if (LeftArrayType != nullptr) {
            if (RightIntegerBaseType == nullptr) {
                Log(LogLevel::PCC_ERROR,
                    "cannot access the array %s with a non-integer type %s",
                    LeftType->GetCommonName(), RightType->GetCommonName());
                return {nullptr, nullptr};
            }
            std::shared_ptr<Type> LeftIndexType = LeftArrayType->GetIndexType();
            Value CastedInedx =
                builder->CreateSExtOrTrunc(right, LeftIndexType->GetLLVMType());
            return {LeftArrayType->GetElementType(),
                    LeftType->CreateBinaryOperation(op, left, CastedInedx,
                                                    context)};
        }
        if (LeftPointerType != nullptr) {
            if (RightIntegerBaseType == nullptr) {
                Log(LogLevel::PCC_ERROR,
                    "cannot access the pointer %s with a non-integer type %s",
                    LeftType->GetCommonName(), RightType->GetCommonName());
                return {nullptr, nullptr};
            }
            Value CastedInedx = builder->CreateSExtOrTrunc(
                right, PointerIndexType_->GetLLVMType());
            return {LeftPointerType->GetElementType(),
                    LeftType->CreateBinaryOperation(op, left, CastedInedx,
                                                    context)};
        }
        Log(LogLevel::PCC_ERROR,
            "the left operand type %s is not array or pointer",
            LeftType->GetCommonName());
        return {nullptr, nullptr};
    }

    // add/sub a pointer with a integer
    if (op == BinaryOperator::ADD || op == BinaryOperator::SUB) {
        if (LeftPointerType != nullptr) {
            if (RightIntegerBaseType != nullptr) {
                Log(LogLevel::PCC_ERROR,
                    "can not add pointer with non-integer type %s");
                return {nullptr, nullptr};
            }
            Value CastedInedx = builder->CreateSExtOrTrunc(
                right, PointerIndexType_->GetLLVMType());
            return {LeftType, builder->CreateInBoundsGEP(
                                  LeftType->GetLLVMType(), left, CastedInedx)};
        }
    }

    // other operations will require the types of left and right are the same.
    std::shared_ptr<RealType> LeftRealType =
        std::dynamic_pointer_cast<RealType>(LeftType);
    std::shared_ptr<RealType> RightRealType =
        std::dynamic_pointer_cast<RealType>(RightType);
    Value ConvertedLeft, ConvertedRight;
    std::shared_ptr<Type> CommonType;
    if (LeftType != RightType) {
        // we can cast the boolean, char, integer and real.
        if (LeftRealType != nullptr && RightIntegerBaseType != nullptr) {
            ConvertedLeft = left;
            ConvertedRight =
                builder->CreateSIToFP(right, LeftType->GetLLVMType());
            CommonType = LeftType;
        } else if (RightRealType != nullptr && LeftIntegerBaseType != nullptr) {
            ConvertedLeft =
                builder->CreateSIToFP(left, RightType->GetLLVMType());
            ConvertedRight = right;
            CommonType = RightType;
        } else if (LeftIntegerBaseType != nullptr &&
                   RightIntegerBaseType != nullptr) {
            CommonType = (LeftIntegerBaseType->GetWidth() >
                                  RightIntegerBaseType->GetWidth()
                              ? LeftType
                              : RightType);
            ConvertedLeft =
                builder->CreateSExt(left, CommonType->GetLLVMType());
            ConvertedRight =
                builder->CreateSExt(right, CommonType->GetLLVMType());
        } else {
            Log(LogLevel::PCC_ERROR,
                "can not apply binary operator on two different type %s and %s",
                LeftType->GetCommonName(), RightType->GetCommonName());
            return {nullptr, nullptr};
        }
    } else {
        ConvertedLeft = left;
        ConvertedRight = right;
        CommonType = LeftType;
    }
    Value result = CommonType->CreateBinaryOperation(op, ConvertedLeft,
                                                     ConvertedRight, context);
    if (result == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "binary operator %s is not supported on type %s",
            GetOperatorName(op), CommonType->GetCommonName());
    }
    // logical operators return boolean type
    if (ToUnderlying(op) >= ToUnderlying(BinaryOperator::LT) &&
        ToUnderlying(op) <= ToUnderlying(BinaryOperator::NE)) {
        CommonType = builtins_[ToUnderlying(BuiltinType::BOOLEAN)];
    }
    return {CommonType, result};
}

Value TypeManager::CreateOperation(UnaryOperator op, std::shared_ptr<Type> type,
                                   Value v, Context* context) {
    Value result = type->CreateUnaryOperation(op, v, context);
    if (result == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "unary operator %s is not supported on type %s",
            GetOperatorName(op), type->GetCommonName());
    }
    return result;
}
} // namespace pcc
