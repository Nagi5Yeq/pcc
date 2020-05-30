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
                                     std::shared_ptr<Type> type,
                                     std::vector<char> value)
    : ExprNode(context)
    , value_(value) {
    type_ = type;
}

Value StringLiteralNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    std::shared_ptr<PointerType> pointer =
        std::static_pointer_cast<PointerType>(type_);
    llvm::Constant* ConstantString =
        llvm::ConstantDataArray::get(GlobalLLVMContext, value_);
    llvm::GlobalValue* GlobalString = new llvm::GlobalVariable(
        *context_->GetModule(), ConstantString->getType(), true,
        llvm::GlobalValue::PrivateLinkage, ConstantString);
    GlobalString->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
    Value ZeroIndex =
        llvm::ConstantInt::get(pointer->GetIndexType()->GetLLVMType(), 0, true);
    Value indices[2] = {ZeroIndex, ZeroIndex};
    return builder->CreateInBoundsGEP(GlobalString, indices);
}
} // namespace pcc
