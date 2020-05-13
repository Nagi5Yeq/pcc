#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APInt.h>

#include "Driver.hh"
#include "Node.hh"

namespace pcc {
template <> Value BooleanLiteralNode::CodeGen() {
    llvm::LLVMContext& context = driver_->GetModule()->getContext();
    if (value_ == true) {
        return llvm::ConstantInt::getTrue(context);
    } else {
        return llvm::ConstantInt::getFalse(context);
    }
}

template <> Value IntegerLiteralNode::CodeGen() {
    llvm::LLVMContext& context = driver_->GetModule()->getContext();
    llvm::APInt value(32, (uint64_t) value_, true);
    return llvm::ConstantInt::get(context, value);
}

template <> Value RealLiteralNode::CodeGen() {
    llvm::LLVMContext& context = driver_->GetModule()->getContext();
    llvm::APFloat value(value_);
    return llvm::ConstantFP::get(context, value);
}

template <> Value StringLiteralNode::CodeGen() {
    llvm::Module* module = driver_->GetModule();
    llvm::LLVMContext& context = module->getContext();
    llvm::Constant* str = llvm::ConstantDataArray::getString(context, value_);
    llvm::Type* type = str->getType();
    llvm::GlobalVariable* var = new llvm::GlobalVariable(
        *module, type, true, llvm::GlobalVariable::InternalLinkage, str);
    llvm::Constant* zero =
        llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(context));
    llvm::Constant* indices[] = {zero, zero};
    llvm::Constant* ptr =
        llvm::ConstantExpr::getGetElementPtr(type, var, indices, true);
    return ptr;
}
} // namespace pcc
