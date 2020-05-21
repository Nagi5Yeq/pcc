#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>

#include "Driver.hh"
#include "LiteralNode.hh"

namespace pcc {
template <> Value BooleanLiteralNode::CodeGen() {
    if (value_ == true) {
        return llvm::ConstantInt::getTrue(GlobalLLVMContext);
    } else {
        return llvm::ConstantInt::getFalse(GlobalLLVMContext);
    }
}

template <> Value CharLiteralNode::CodeGen() {
    return llvm::ConstantInt::get(type_->GetLLVMType(), value_, true);
}

template <> Value IntegerLiteralNode::CodeGen() {
    return llvm::ConstantInt::get(type_->GetLLVMType(), value_, true);
}

template <> Value RealLiteralNode::CodeGen() {
    llvm::APFloat value(value_);
    return llvm::ConstantFP::get(GlobalLLVMContext, value);
}

StringLiteralNode::StringLiteralNode(Context* context,
                                     std::shared_ptr<Type> type, char* begin,
                                     char* end)
    : ExprNode(context)
    , value_(begin, end) {
    type_ = type;
}

Value StringLiteralNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    std::shared_ptr<PointerType> pointer =
        std::static_pointer_cast<PointerType>(type_);
    llvm::Constant* ConstantString =
        llvm::ConstantDataArray::get(GlobalLLVMContext, value_);
    llvm::Constant* GlobalString = new llvm::GlobalVariable(
        type_->GetLLVMType(), true, llvm::GlobalValue::PrivateLinkage,
        ConstantString);
    Value ZeroIndex =
        llvm::ConstantInt::get(pointer->GetIndexType()->GetLLVMType(), 0, true);
    return builder->CreateInBoundsGEP(GlobalString, {ZeroIndex, ZeroIndex});
}
} // namespace pcc
