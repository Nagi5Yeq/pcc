#include <memory>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

#include "DeclNode.hh"
#include "FunctionNode.hh"
#include "Log.hh"

namespace pcc {
FunctionNode::FunctionNode(
    Context* context, std::string name, std::list<Declaration> arguments,
    std::list<std::shared_ptr<BaseNode>> LocalDeclarations,
    std::shared_ptr<Type> type, std::shared_ptr<StatementListNode> body)
    : BaseNode(context)
    , name_(name)
    , arguments_(arguments)
    , LocalDeclarations_(LocalDeclarations)
    , type_(type)
    , locals_(context->GetGlobals())
    , body_(body) {
    for (auto& decl : LocalDeclarations_) {
        std::shared_ptr<VarDeclNode> VarDecl =
            std::dynamic_pointer_cast<VarDeclNode>(decl);
        if (VarDecl != nullptr) {
            // set the inherited attribute
            VarDecl->SetScope(&locals_);
        }
    }
}

Value FunctionNode::CodeGen() {
    llvm::Module* module = context_->GetModule();
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    std::vector<std::shared_ptr<Type>> params(arguments_.size());
    std::transform(arguments_.cbegin(), arguments_.cend(), params.begin(),
                   [](const Declaration& decl) { return std::get<1>(decl); });
    std::shared_ptr<FunctionType> type =
        context_->GetTypeManager()->CreateFunctionType(type_, params);
    context_->AddFunctionType(name_, type);
    llvm::FunctionType* LLVMType =
        llvm::cast<llvm::FunctionType>(type->GetLLVMType());
    llvm::Function* function = llvm::Function::Create(
        LLVMType, llvm::GlobalValue::ExternalLinkage, name_, *module);
    function->setCallingConv(llvm::CallingConv::C);
    llvm::BasicBlock* block =
        llvm::BasicBlock::Create(GlobalLLVMContext, name_, function);
    builder->SetInsertPoint(block);
    // alloc arguments
    llvm::Function::arg_iterator ArgIt = function->arg_begin();
    for (auto& arg : arguments_) {
        Value ArgValue = builder->CreateAlloca(std::get<1>(arg)->GetLLVMType());
        builder->CreateStore(ArgIt++, ArgValue);
        locals_.Add(std::get<0>(arg), {std::get<1>(arg), ArgValue});
    }
    // alloc local variables
    for (auto& decl : LocalDeclarations_) {
        decl->CodeGen();
    }
    // create a variable with the same name as the function for return value
    Value rv = builder->CreateAlloca(type_->GetLLVMType());
    locals_.Add(name_, {type_, rv});
    // push the function scope variables
    context_->PushScope(&locals_);
    body_->CodeGen();
    builder->CreateRet(builder->CreateLoad(type_->GetLLVMType(), rv));
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
