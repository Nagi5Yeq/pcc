#include "TypeManager.hh"
#include "Context.hh"
#include "ExprNode.hh"
#include "Log.hh"

namespace pcc {
TypeManager::TypeManager()
    : builtins_{std::make_shared<VoidType>(),  std::make_shared<BooleanType>(),
                std::make_shared<CharType>(),  std::make_shared<IntegerType>(),
                std::make_shared<Int64Type>(), std::make_shared<RealType>()} {
    PointerIndexType_ = GetBuiltinType(BuiltinType::INT64);
    PointerDifferenceType_ = GetBuiltinType(BuiltinType::INT64);
    // llvm seems doesn't support GEP with i64 index type on records
    RecordMemberIndexType_ = GetBuiltinType(BuiltinType::INTEGER);
    builtins_.push_back(
        GetPointerType(GetBuiltinType(BuiltinType::CHAR), "string"));
    // llvm doesn't have void* type, we use char* instead
    PointerTypes_.emplace(
        GetBuiltinType(BuiltinType::VOID),
        std::make_shared<PointerType>(GetBuiltinType(BuiltinType::CHAR),
                                      PointerDifferenceType_, PointerIndexType_,
                                      "^void"));
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
    std::shared_ptr<PointerType> RightPointerType =
        std::dynamic_pointer_cast<PointerType>(RightType);
    std::shared_ptr<ArrayType> LeftArrayType =
        std::dynamic_pointer_cast<ArrayType>(LeftType);

    // add/sub a pointer with a integer
    if (LeftPointerType != nullptr && RightPointerType == nullptr) {
        if (op != BinaryOperator::ADD && op != BinaryOperator::SUB) {
            Log(LogLevel::PCC_ERROR, "unsupported operation %s on %s and %s",
                GetOperatorName(op), LeftType->GetCommonName(),
                RightType->GetCommonName());
            return {nullptr, nullptr};
        }
        if (RightIntegerBaseType == nullptr) {
            Log(LogLevel::PCC_ERROR,
                "can not add pointer with non-integer type %s",
                RightType->GetCommonName());
            return {nullptr, nullptr};
        }
        Value CastedInedx =
            builder->CreateSExtOrTrunc(right, PointerIndexType_->GetLLVMType());
        return {LeftType, builder->CreateInBoundsGEP(left, CastedInedx)};
    }

    // pointer sub case
    if (LeftPointerType != nullptr && RightPointerType != nullptr &&
        op == BinaryOperator::SUB) {
        if (LeftType != RightType) {
            Log(LogLevel::PCC_ERROR,
                "cannot sub to pointers of different type");
            return {nullptr, nullptr};
        }
        Value result =
            LeftType->CreateBinaryOperation(op, left, right, context);
        return {PointerDifferenceType_, result};
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
        // for other types, REAL_DIV and DIV are unavailable.
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

    // array to pointer decay
    std::shared_ptr<PointerType> SrcPointerType =
        std::dynamic_pointer_cast<PointerType>(SrcType);
    std::shared_ptr<PointerType> DstPointerType =
        std::dynamic_pointer_cast<PointerType>(DstType);
    if (SrcPointerType != nullptr && DstPointerType != nullptr) {
        std::shared_ptr<ArrayType> SrcInnerType =
            std::dynamic_pointer_cast<ArrayType>(
                SrcPointerType->GetElementType());
        if (SrcInnerType->GetElementType() ==
            DstPointerType->GetElementType()) {
            Value zero =
                llvm::ConstantInt::get(PointerIndexType_->GetLLVMType(), 0);
            Value indices[2] = {zero, zero};
            return builder->CreateInBoundsGEP(v, indices);
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

std::shared_ptr<ArrayType>
    TypeManager::CreateArrayType(std::shared_ptr<Type> ElementType,
                                 std::shared_ptr<ExprNode> start,
                                 std::shared_ptr<ExprNode> end) {
    llvm::Constant* StartValue = llvm::cast<llvm::Constant>(start->CodeGen());
    llvm::Constant* EndValue = llvm::cast<llvm::Constant>(end->CodeGen());
    std::shared_ptr<Type> IndexType = start->GetType();
    if (IndexType != end->GetType()) {
        Log(LogLevel::PCC_ERROR,
            "the start and end indices of an array type must be same "
            "(start=%s, end=%s)",
            IndexType->GetCommonName(), end->GetType()->GetCommonName());
        return nullptr;
    }
    if (std::dynamic_pointer_cast<IntegerBaseType>(IndexType) == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "array index must have a integer-like type (i.e. integer, char)");
        return nullptr;
    }
    uint64_t StartIndex = StartValue->getUniqueInteger().getZExtValue();
    uint64_t ElementNumber =
        EndValue->getUniqueInteger().getZExtValue() - StartIndex + 1;
    std::shared_ptr<ArrayType> NewType = std::make_shared<ArrayType>(
        ElementType, ElementNumber, IndexType, StartValue, EndValue,
        std::string(ElementType->GetCommonName()) + "[" +
            std::to_string(ElementNumber) + "]",
        StartIndex == 0);
    ArrayTypes_.push_back(NewType);
    return NewType;
}

std::shared_ptr<RecordType>
    TypeManager::CreateRecordType(const std::vector<RecordMember>& members) {
    std::string name = "{";
    std::vector<RecordMember>::const_iterator MemberIt = members.cbegin();
    name.append(std::get<1>(*MemberIt)->GetCommonName());
    for (++MemberIt; MemberIt != members.cend(); ++MemberIt) {
        name.append(", ");
        name.append(std::get<1>(*MemberIt)->GetCommonName());
    }
    std::shared_ptr<RecordType> NewType =
        std::make_shared<RecordType>(members, name + "}");
    RecordTypes_.push_back(NewType);
    return NewType;
}

std::shared_ptr<FunctionType>
    TypeManager::CreateFunctionType(std::shared_ptr<Type> ReturnType,
                                    std::vector<std::shared_ptr<Type>> ArgTypes,
                                    bool IsVariadic) {
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
        ReturnType, ArgTypes, ReturnType->GetCommonName() + name + ")",
        IsVariadic);
    FunctionTypes_.push_back(NewType);
    return NewType;
}
std::shared_ptr<Type> TypeManager::GetPointerIndexType() {
    return PointerIndexType_;
}

std::shared_ptr<Type> TypeManager::GetPointerDifferenceType() {
    return PointerDifferenceType_;
}

std::shared_ptr<Type> TypeManager::GetRecordMemberIndexType() {
    return RecordMemberIndexType_;
}

} // namespace pcc
