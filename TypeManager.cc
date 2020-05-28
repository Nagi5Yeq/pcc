#include "TypeManager.hh"
#include "Context.hh"
#include "Log.hh"

namespace pcc {
TypeManager::TypeManager()
    : builtins_{std::make_shared<BooleanType>(), std::make_shared<CharType>(),
                std::make_shared<IntegerType>(), std::make_shared<RealType>()} {
    PointerDifferenceType_ = GetBuiltinType(BuiltinType::INTEGER);
    PointerIndexType_ = GetBuiltinType(BuiltinType::INTEGER);
    builtins_.push_back(
        GetPointerType(GetBuiltinType(BuiltinType::CHAR), "string"));
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
            if (RightIntegerBaseType == nullptr) {
                Log(LogLevel::PCC_ERROR,
                    "can not add pointer with non-integer type %s");
                return {nullptr, nullptr};
            }
            Value CastedInedx = builder->CreateSExtOrTrunc(
                right, PointerIndexType_->GetLLVMType());
            return {LeftType, builder->CreateInBoundsGEP(left, CastedInedx)};
        }
    }

    // other operations will require the types of left and right are the same.
    std::shared_ptr<RealType> LeftRealType =
        std::dynamic_pointer_cast<RealType>(LeftType);
    std::shared_ptr<RealType> RightRealType =
        std::dynamic_pointer_cast<RealType>(RightType);
    Value ConvertedLeft, ConvertedRight;
    std::shared_ptr<Type> CommonType;
    if (op == BinaryOperator::REAL_DIV) {
        // we can only set the CommonType to real for REAL_DIV on builtin types,
        // for other types, REAL_DIV and DIV are unavaliable.
        CommonType = builtins_[ToUnderlying(BuiltinType::REAL)];
        op = BinaryOperator::DIV;
    } else if (LeftType != RightType) {
        // we can cast the boolean, char, integer and real.
        if (op == BinaryOperator::REAL_DIV) {
            CommonType = builtins_[ToUnderlying(BuiltinType::REAL)];
        } else if (LeftRealType != nullptr && RightIntegerBaseType != nullptr) {
            CommonType = LeftType;
        } else if (RightRealType != nullptr && LeftIntegerBaseType != nullptr) {
            CommonType = RightType;
        } else if (LeftIntegerBaseType != nullptr &&
                   RightIntegerBaseType != nullptr) {
            CommonType = (LeftIntegerBaseType->GetWidth() >
                                  RightIntegerBaseType->GetWidth()
                              ? LeftType
                              : RightType);
        } else {
            Log(LogLevel::PCC_ERROR,
                "can not apply binary operator on two different type %s and %s",
                LeftType->GetCommonName(), RightType->GetCommonName());
            return {nullptr, nullptr};
        }
    } else {
        CommonType = LeftType;
    }
    ConvertedLeft = CreateCast(CommonType, LeftType, left, context);
    ConvertedRight = CreateCast(CommonType, RightType, right, context);
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

Value TypeManager::CreateCast(std::shared_ptr<Type> DstType,
                              std::shared_ptr<Type> SrcType, Value v,
                              Context* context) {
    if (DstType == SrcType) {
        return v;
    }
    llvm::IRBuilder<>* builder = context->GetBuilder();
    // we can only cast between integer and real
    std::shared_ptr<IntegerBaseType> DstIntegerBaseType =
        std::dynamic_pointer_cast<IntegerBaseType>(DstType);
    std::shared_ptr<IntegerBaseType> SrcIntegerBaseType =
        std::dynamic_pointer_cast<IntegerBaseType>(SrcType);
    std::shared_ptr<RealType> DstRealType =
        std::dynamic_pointer_cast<RealType>(DstType);
    std::shared_ptr<RealType> SrcRealType =
        std::dynamic_pointer_cast<RealType>(SrcType);
    if (SrcRealType != nullptr && DstIntegerBaseType != nullptr) {
        return builder->CreateFPToSI(v, DstType->GetLLVMType());
    } else if (SrcIntegerBaseType != nullptr) {
        if (DstRealType != nullptr) {
            return builder->CreateSIToFP(v, DstType->GetLLVMType());
        } else if (DstIntegerBaseType != nullptr) {
            return builder->CreateSExtOrTrunc(v, DstType->GetLLVMType());
        }
    }
    Log(LogLevel::PCC_ERROR, "unsupported type casting from %s to %s",
        SrcType->GetCommonName(), DstType->GetCommonName());
    return nullptr;
}

std::shared_ptr<PointerType>
    TypeManager::GetPointerType(std::shared_ptr<Type> ElementType,
                                const char* CustomName) {
    decltype(PointerTypes_)::iterator result = PointerTypes_.find(ElementType);
    if (result != PointerTypes_.end()) {
        return std::get<1>(*result);
    }
    std::shared_ptr<PointerType> NewType = std::make_shared<PointerType>(
        ElementType, PointerDifferenceType_, PointerIndexType_,
        CustomName == nullptr ? "^" + std::string(ElementType->GetCommonName())
                              : CustomName);
    PointerTypes_.insert({ElementType, NewType});
    return NewType;
}

std::shared_ptr<FunctionType> TypeManager::CreateFunctionType(
    std::shared_ptr<Type> ReturnType,
    std::vector<std::shared_ptr<Type>> ArgTypes) {
    decltype(ArgTypes)::const_iterator ArgIt = ArgTypes.cbegin();
    std::string name = " (*)(";
    if (!ArgTypes.empty()) {
        name.append((*ArgIt)->GetCommonName());
        for (++ArgIt; ArgIt != ArgTypes.cend(); ++ArgIt) {
            name.append(", ");
            name.append((*ArgIt)->GetCommonName());
        }
    }
    std::shared_ptr<FunctionType> NewType = std::make_shared<FunctionType>(
        ReturnType, ArgTypes, ReturnType->GetCommonName() + name + ")");
    FunctionTypes_.push_back(NewType);
    return NewType;
}

} // namespace pcc
