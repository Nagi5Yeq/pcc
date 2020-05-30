#include <llvm/IR/InstrTypes.h>

#include "Context.hh"
#include "Type.hh"

namespace pcc {
const char* GetOperatorName(BinaryOperator op) {
    static const char* names[ToUnderlying(BinaryOperator::BINARYOP_NUMBER)] = {
        "+",  "-", "*",  "/",   "%",  "<",   "<=",  ">",
        ">=", "=", "<>", "and", "or", "xor", "shl", "shr"};
    return names[ToUnderlying(op)];
}

const char* GetOperatorName(UnaryOperator op) {
    static const char* names[ToUnderlying(UnaryOperator::UNARYOP_NUMEBR)] = {
        "+", "-", "not"};
    return names[ToUnderlying(op)];
}

Type::Type(std::string&& name, llvm::Type* LLVMType)
    : name_(std::move(name))
    , LLVMType_(LLVMType) {}

const char* Type::GetCommonName() { return name_.c_str(); }

llvm::Type* Type::GetLLVMType() { return LLVMType_; }

llvm::Constant* Type::GetZeroInitializer() {
    return llvm::Constant::getNullValue(LLVMType_);
}

Value Type::NotAllowed(Value v, Context* context) { return nullptr; }
Value Type::NotAllowed(Value v0, Value v1, Context* context) { return nullptr; }

Value Type::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                  Context* context) {
    return NotAllowed(v0, v1, context);
}

Value Type::CreateUnaryOperation(UnaryOperator op, Value v, Context* context) {
    return NotAllowed(v, context);
}

Value Type::CreateAdd(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateAdd(v0, v1);
}

Value Type::CreateSub(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateSub(v0, v1);
}

Value Type::CreateMul(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateMul(v0, v1);
}

Value Type::CreateDiv(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateSDiv(v0, v1);
}

Value Type::CreateMod(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateSRem(v0, v1);
}

Value Type::CreateLt(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SLT, v0, v1);
}

Value Type::CreateLe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SLE, v0, v1);
}

Value Type::CreateGt(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SGT, v0, v1);
}

Value Type::CreateGe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SGE, v0, v1);
}

Value Type::CreateEq(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_EQ, v0, v1);
}

Value Type::CreateNe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_NE, v0, v1);
}

Value Type::CreateAnd(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateAnd(v0, v1);
}

Value Type::CreateOr(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateOr(v0, v1);
}

Value Type::CreateXor(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateXor(v0, v1);
}

Value Type::CreateShl(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateShl(v0, v1);
}

Value Type::CreateShr(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateAShr(v0, v1);
}

Value Type::CreatePos(Value v, Context* context) { return v; }

Value Type::CreateNeg(Value v, Context* context) {
    return context->GetBuilder()->CreateNeg(v);
}

Value Type::CreateNot(Value v, Context* context) {
    return context->GetBuilder()->CreateNot(v);
}

Value Type::CreateFAdd(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFAdd(v0, v1);
}

Value Type::CreateFSub(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFSub(v0, v1);
}

Value Type::CreateFMul(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFMul(v0, v1);
}

Value Type::CreateFDiv(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFDiv(v0, v1);
}

Value Type::CreateFMod(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFRem(v0, v1);
}

Value Type::CreateFLt(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OLT, v0, v1);
}

Value Type::CreateFLe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OLE, v0, v1);
}

Value Type::CreateFGt(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OGT, v0, v1);
}

Value Type::CreateFGe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OGE, v0, v1);
}

Value Type::CreateFEq(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OEQ, v0, v1);
}

Value Type::CreateFNe(Value v0, Value v1, Context* context) {
    return context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_ONE, v0, v1);
}

Value Type::CreateFPos(Value v, Context* context) { return v; }

Value Type::CreateFNeg(Value v, Context* context) {
    return context->GetBuilder()->CreateFNeg(v);
}

VoidType::VoidType()
    : Type("void", llvm::Type::getVoidTy(GlobalLLVMContext)) {}

IntegerBaseType::IntegerBaseType(std::string&& name, int width,
                                 llvm::Type* LLVMType)
    : Type(std::move(name), LLVMType)
    , width_(width) {}

int IntegerBaseType::GetWidth() { return width_; }

BooleanType::BooleanType()
    : IntegerBaseType("boolean", 1, llvm::Type::getInt1Ty(GlobalLLVMContext)) {}

Value BooleanType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                         Context* context) {
    static Value (pcc::BooleanType::*operations[ToUnderlying(
        BinaryOperator::BINARYOP_NUMBER)])(Value, Value, Context*) = {
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::CreateEq,
        &BooleanType::CreateNe,   &BooleanType::CreateAnd,
        &BooleanType::CreateOr,   &BooleanType::CreateXor,
        &BooleanType::CreateShl,  &BooleanType::CreateShr};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

Value BooleanType::CreateUnaryOperation(UnaryOperator op, Value v,
                                        Context* context) {
    if (op == UnaryOperator::NOT) {
        return CreateNot(v, context);
    }
    return NotAllowed(v, context);
}

CharType::CharType()
    : IntegerBaseType("char", 8, llvm::Type::getInt8Ty(GlobalLLVMContext)) {}

Value CharType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) {
    static Value (pcc::CharType::*operations[ToUnderlying(
        BinaryOperator::BINARYOP_NUMBER)])(Value, Value, Context*) = {
        &CharType::CreateAdd, &CharType::CreateSub, &CharType::CreateMul,
        &CharType::CreateDiv, &CharType::CreateMod, &CharType::CreateLt,
        &CharType::CreateLe,  &CharType::CreateGt,  &CharType::CreateGe,
        &CharType::CreateEq,  &CharType::CreateNe,  &CharType::CreateAnd,
        &CharType::CreateOr,  &CharType::CreateXor, &CharType::CreateShl,
        &CharType::CreateShr};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

Value CharType::CreateUnaryOperation(UnaryOperator op, Value v,
                                     Context* context) {
    static Value (pcc::CharType::*operations[ToUnderlying(
        UnaryOperator::UNARYOP_NUMEBR)])(Value, Context*) = {
        &CharType::CreatePos, &CharType::CreateNeg, &CharType::CreateNot};
    return (this->*operations[ToUnderlying(op)])(v, context);
}

IntegerType::IntegerType()
    : IntegerBaseType("integer", 32,
                      llvm::Type::getInt32Ty(GlobalLLVMContext)) {}

Value IntegerType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                         Context* context) {
    static Value (pcc::IntegerType::*operations[ToUnderlying(
        BinaryOperator::BINARYOP_NUMBER)])(Value, Value, Context*) = {
        &IntegerType::CreateAdd, &IntegerType::CreateSub,
        &IntegerType::CreateMul, &IntegerType::CreateDiv,
        &IntegerType::CreateMod, &IntegerType::CreateLt,
        &IntegerType::CreateLe,  &IntegerType::CreateGt,
        &IntegerType::CreateGe,  &IntegerType::CreateEq,
        &IntegerType::CreateNe,  &IntegerType::CreateAnd,
        &IntegerType::CreateOr,  &IntegerType::CreateXor,
        &IntegerType::CreateShl, &IntegerType::CreateShr};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

Value IntegerType::CreateUnaryOperation(UnaryOperator op, Value v,
                                        Context* context) {
    static Value (pcc::IntegerType::*
                      operations[ToUnderlying(UnaryOperator::UNARYOP_NUMEBR)])(
        Value, Context*) = {&IntegerType::CreatePos, &IntegerType::CreateNeg,
                            &IntegerType::CreateNot};
    return (this->*operations[ToUnderlying(op)])(v, context);
}

RealType::RealType()
    : Type("real", llvm::Type::getFloatTy(GlobalLLVMContext)) {}

Value RealType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) {
    static Value (pcc::RealType::*operations[ToUnderlying(
        BinaryOperator::BINARYOP_NUMBER)])(Value, Value, Context*) = {
        &RealType::CreateFAdd, &RealType::CreateFSub, &RealType::CreateFMul,
        &RealType::CreateFDiv, &RealType::CreateFMod, &RealType::CreateFLt,
        &RealType::CreateFLe,  &RealType::CreateFGt,  &RealType::CreateFGe,
        &RealType::CreateFEq,  &RealType::CreateFNe,  &RealType::NotAllowed,
        &RealType::NotAllowed, &RealType::NotAllowed, &RealType::NotAllowed,
        &RealType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

Value RealType::CreateUnaryOperation(UnaryOperator op, Value v,
                                     Context* context) {
    static Value (pcc::RealType::*operations[ToUnderlying(
        UnaryOperator::UNARYOP_NUMEBR)])(Value, Context*) = {
        &RealType::CreateFPos, &RealType::CreateFNeg, &RealType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v, context);
}

ArrayType::ArrayType(std::shared_ptr<Type> ElementType, uint64_t ElementNumber,
                     std::shared_ptr<Type> IndexType, Value start, Value end,
                     std::string&& name, bool IsZeroStarted)
    : Type(std::move(name),
           llvm::ArrayType::get(ElementType->GetLLVMType(), ElementNumber))
    , ElementType_(ElementType)
    , IndexType_(IndexType)
    , start_(start)
    , end_(end)
    , IsZeroStarted_(IsZeroStarted) {}

std::shared_ptr<Type> ArrayType::GetElementType() { return ElementType_; }
std::shared_ptr<Type> ArrayType::GetIndexType() { return IndexType_; }
bool ArrayType::GetIsZeroStarted() { return IsZeroStarted_; }
Value ArrayType::GetIndexStart() { return start_; }

PointerType::PointerType(std::shared_ptr<Type> ElementType,
                         std::shared_ptr<Type> DifferenceType,
                         std::shared_ptr<Type> IndexType, std::string&& name)
    : Type(std::move(name),
           llvm::PointerType::get(ElementType->GetLLVMType(), 0))
    , ElementType_(ElementType)
    , DifferenceType_(DifferenceType)
    , IndexType_(IndexType) {}

std::shared_ptr<Type> PointerType::GetElementType() { return ElementType_; }
std::shared_ptr<Type> PointerType::GetIndexType() { return IndexType_; }
std::shared_ptr<Type> PointerType::GeDiifferenceType() {
    return DifferenceType_;
}

Value PointerType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                         Context* context) {
    static Value (pcc::PointerType::*operations[ToUnderlying(
        BinaryOperator::BINARYOP_NUMBER)])(Value, Value, Context*) = {
        &PointerType::NotAllowed, &PointerType::CreatePointerSub,
        &PointerType::NotAllowed, &PointerType::NotAllowed,
        &PointerType::NotAllowed, &PointerType::CreateLt,
        &PointerType::CreateLe,   &PointerType::CreateGt,
        &PointerType::CreateGe,   &PointerType::CreateEq,
        &PointerType::CreateNe,   &PointerType::NotAllowed,
        &PointerType::NotAllowed, &PointerType::NotAllowed,
        &PointerType::NotAllowed, &PointerType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

Value PointerType::CreatePointerSub(Value v0, Value v1, Context* context) {
    llvm::IRBuilder<>* builder = context->GetBuilder();
    llvm::Type* ResultLLVMType = DifferenceType_->GetLLVMType();
    Value ConvertedV0 = builder->CreatePtrToInt(v0, ResultLLVMType);
    Value ConvertedV1 = builder->CreatePtrToInt(v1, ResultLLVMType);
    return builder->CreateSub(ConvertedV0, ConvertedV1);
}

FunctionType::FunctionType(std::shared_ptr<Type> ReturnType,
                           std::vector<std::shared_ptr<Type>> ArgTypes,
                           std::string&& name)
    : Type(std::move(name), nullptr)
    , ReturnType_(ReturnType)
    , ArgTypes_(ArgTypes) {
    std::vector<llvm::Type*> params(ArgTypes_.size());
    std::transform(
        ArgTypes_.cbegin(), ArgTypes_.cend(), params.begin(),
        [](std::shared_ptr<Type> decl) { return decl->GetLLVMType(); });
    LLVMType_ =
        llvm::FunctionType::get(ReturnType_->GetLLVMType(), params, false);
}

std::shared_ptr<Type> FunctionType::GetReturnType() { return ReturnType_; }

std::vector<std::shared_ptr<Type>> FunctionType::GetArgTypes() {
    return ArgTypes_;
}
} // namespace pcc
