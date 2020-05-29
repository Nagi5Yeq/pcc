#include <memory>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

#include "DeclNode.hh"
#include "FunctionNode.hh"
#include "Log.hh"

namespace pcc {
FunctionNode::FunctionNode(
    Context* context, std::string&& name, std::list<Declaration>&& arguments,
    std::list<std::shared_ptr<DeclNode>>&& LocalDeclarations,
    std::shared_ptr<Type> type, std::shared_ptr<StatementListNode> body)
    : BaseNode(context)
    , name_(std::move(name))
    , arguments_(std::move(arguments))
    , LocalDeclarations_(std::move(LocalDeclarations))
    , type_(type)
    , locals_(context->GetGlobals())
    , body_(body) {
    for (auto&& decl : LocalDeclarations_) {
        decl->SetScope(&locals_);
    }
}

Value FunctionNode::CodeGen() {
    llvm::Module* module = context_->GetModule();
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    std::vector<std::shared_ptr<Type>> params(arguments_.size());
    std::transform(arguments_.cbegin(), arguments_.cend(), params.begin(),
                   [](const Declaration& decl) { return std::get<1>(decl); });
    std::shared_ptr<FunctionType> type =
        manager->CreateFunctionType(type_, params);
    context_->AddFunctionType(name_, type);
    llvm::FunctionType* LLVMType =
        llvm::cast<llvm::FunctionType>(type->GetLLVMType());
    llvm::Function* function = llvm::Function::Create(
        LLVMType, llvm::GlobalValue::ExternalLinkage, name_, *module);
    function->setCallingConv(llvm::CallingConv::C);
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(GlobalLLVMContext, name_, function);
    Log(LogLevel::PCC_INFO, "generating function %s of type %s", name_.c_str(),
        type->GetCommonName());
    builder->SetInsertPoint(block);
    // alloc arguments
    llvm::Function::arg_iterator ArgIt = function->arg_begin();
    for (auto&& arg : arguments_) {
        Value ArgValue = builder->CreateAlloca(std::get<1>(arg)->GetLLVMType());
        builder->CreateStore(ArgIt++, ArgValue);
        locals_.Add(std::get<0>(arg), {std::get<1>(arg), ArgValue});
    }
    // alloc local variables
    for (auto&& decl : LocalDeclarations_) {
        decl->CodeGen();
    }
    Value rv = nullptr;
    bool IsVoid = type_ == manager->GetBuiltinType(BuiltinType::VOID);
    if (IsVoid == false) {
        // create a variable with the same name as the function for return value
        rv = builder->CreateAlloca(type_->GetLLVMType());
        locals_.Add(name_, {type_, rv});
    }
    // push the function scope variables
    context_->PushScope(&locals_);
    body_->CodeGen();
    if (IsVoid == false) {
        builder->CreateRet(builder->CreateLoad(type_->GetLLVMType(), rv));
    } else {
        builder->CreateRetVoid();
    }
    context_->PopScope();
    std::string msg;
    llvm::raw_string_ostream MsgStream(msg);
    llvm::verifyFunction(*function, &MsgStream);
    MsgStream.flush();
    if (!msg.empty()) {
        Log(LogLevel::PCC_INFO, "%s", msg.c_str());
    }
    return nullptr;
}

} // namespace pcc
