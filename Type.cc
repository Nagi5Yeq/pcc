#include <llvm/IR/InstrTypes.h>

#include "Type.hh"

namespace pcc {
Type::Type(const char* name, bool IsBasicType, int width)
    : name_(name)
    , IsBasicType_(IsBasicType)
    , width_(width) {}

const char* Type::GetCommonName() { return name_; }

int Type::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                Context* context) {
    return NotAllowed(v0, v1, context);
}

int Type::NotAllowed(Value v0, Value v1, Context* context) { return -1; }

int Type::CreateAdd(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::Add, v0, v1);
    return 0;
}

int Type::CreateSub(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::Sub, v0, v1);
    return 0;
}

int Type::CreateMul(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::Mul, v0, v1);
    return 0;
}

int Type::CreateDiv(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::SDiv, v0, v1);
    return 0;
}

int Type::CreateMod(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::SRem, v0, v1);
    return 0;
}

int Type::CreateLt(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SLT, v0, v1);
    return 0;
}

int Type::CreateLe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SLE, v0, v1);
    return 0;
}

int Type::CreateGt(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SGT, v0, v1);
    return 0;
}

int Type::CreateGe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_SGE, v0, v1);
    return 0;
}

int Type::CreateEq(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_EQ, v0, v1);
    return 0;
}

int Type::CreateNe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateICmp(llvm::CmpInst::ICMP_NE, v0, v1);
    return 0;
}

int Type::CreateAnd(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::And, v0, v1);
    return 0;
}

int Type::CreateOr(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::Or, v0, v1);
    return 0;
}

int Type::CreateXor(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::Xor, v0, v1);
    return 0;
}

int Type::CreateFAdd(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::FAdd, v0, v1);
    return 0;
}

int Type::CreateFSub(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::FSub, v0, v1);
    return 0;
}

int Type::CreateFMul(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::FMul, v0, v1);
    return 0;
}

int Type::CreateFDiv(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::FDiv, v0, v1);
    return 0;
}

int Type::CreateFMod(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateBinOp(llvm::Instruction::FRem, v0, v1);
    return 0;
}

int Type::CreateFLt(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OLT, v0, v1);
    return 0;
}

int Type::CreateFLe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OLE, v0, v1);
    return 0;
}

int Type::CreateFGt(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OGT, v0, v1);
    return 0;
}

int Type::CreateFGe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OGE, v0, v1);
    return 0;
}

int Type::CreateFEq(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_OEQ, v0, v1);
    return 0;
}

int Type::CreateFNe(Value v0, Value v1, Context* context) {
    context->GetBuilder()->CreateFCmp(llvm::CmpInst::FCMP_ONE, v0, v1);
    return 0;
}

BooleanType::BooleanType()
    : Type("boolean", true, 1) {}

int BooleanType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                       Context* context) {
    static int (pcc::BooleanType::*operations[ToUnderlying(
        BinaryOperator::OPERATOR_NUMBER)])(Value, Value, Context*) = {
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::NotAllowed,
        &BooleanType::NotAllowed, &BooleanType::CreateEq,
        &BooleanType::CreateNe,   &BooleanType::CreateAnd,
        &BooleanType::CreateOr,   &BooleanType::CreateXor,
        &BooleanType::NotAllowed,
    };
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

CharType::CharType()
    : Type("char", true, 8) {}

int CharType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                    Context* context) {
    static int (pcc::CharType::*operations[ToUnderlying(
        BinaryOperator::OPERATOR_NUMBER)])(Value, Value, Context*) = {
        &CharType::CreateAdd, &CharType::CreateSub, &CharType::CreateMul,
        &CharType::CreateDiv, &CharType::CreateMod, &CharType::CreateLt,
        &CharType::CreateLe,  &CharType::CreateGt,  &CharType::CreateGe,
        &CharType::CreateEq,  &CharType::CreateNe,  &CharType::CreateAnd,
        &CharType::CreateOr,  &CharType::CreateXor, &CharType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

IntegerType::IntegerType()
    : Type("integer", true, 32) {}

int IntegerType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                       Context* context) {
    static int (pcc::IntegerType::*operations[ToUnderlying(
        BinaryOperator::OPERATOR_NUMBER)])(Value, Value, Context*) = {
        &IntegerType::CreateAdd, &IntegerType::CreateSub,
        &IntegerType::CreateMul, &IntegerType::CreateDiv,
        &IntegerType::CreateMod, &IntegerType::CreateLt,
        &IntegerType::CreateLe,  &IntegerType::CreateGt,
        &IntegerType::CreateGe,  &IntegerType::CreateEq,
        &IntegerType::CreateNe,  &IntegerType::CreateAnd,
        &IntegerType::CreateOr,  &IntegerType::CreateXor,
        &IntegerType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

RealType::RealType()
    : Type("real", true, 32) {}

int RealType::CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                    Context* context) {
    static int (pcc::RealType::*operations[ToUnderlying(
        BinaryOperator::OPERATOR_NUMBER)])(Value, Value, Context*) = {
        &RealType::CreateFAdd, &RealType::CreateFSub, &RealType::CreateFMul,
        &RealType::CreateFDiv, &RealType::CreateFMod, &RealType::CreateFLt,
        &RealType::CreateFLe,  &RealType::CreateFGt,  &RealType::CreateFGe,
        &RealType::CreateFEq,  &RealType::CreateFNe,  &RealType::NotAllowed,
        &RealType::NotAllowed, &RealType::NotAllowed, &RealType::NotAllowed};
    return (this->*operations[ToUnderlying(op)])(v0, v1, context);
}

} // namespace pcc