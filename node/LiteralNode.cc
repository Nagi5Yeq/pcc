#include "LiteralNode.hh"

namespace pcc {
StringLiteralNode::StringLiteralNode(Driver* driver, char* value)
    : BaseNode(driver)
    , value_(value) {}

Value StringLiteralNode::CodeGen() {
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
