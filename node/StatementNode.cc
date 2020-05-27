#include "StatementNode.hh"

namespace pcc {
template <> Value StatementListNode::CodeGen() {
    Value result = nullptr;
    for (auto& child : childs_) {
        result = child->CodeGen();
    }
    return result;
}

EmptyStatementNode::EmptyStatementNode(Context* context)
    : BaseNode(context) {}

Value EmptyStatementNode::CodeGen() { return nullptr; }

AssignStatementNode::AssignStatementNode(Context* context,
                                         std::shared_ptr<ExprNode> lhs,
                                         std::shared_ptr<ExprNode> rhs)
    : BaseNode(context)
    , lhs_(lhs)
    , rhs_(rhs) {}

Value AssignStatementNode::CodeGen() {
    Value LeftValue = lhs_->CodeGen();
    Value RightValue = rhs_->CodeGen();
    RightValue = context_->GetTypeManager()->CreateCast(
        lhs_->GetType(), rhs_->GetType(), RightValue, context_);
    context_->GetBuilder()->CreateStore(RightValue, LeftValue);
    return nullptr;
}

IfStatementNode::IfStatementNode(Context* context,
                                 std::shared_ptr<ExprNode> expr,
                                 std::shared_ptr<BaseNode> ThenPart,
                                 std::shared_ptr<BaseNode> ElsePart)
    : BaseNode(context)
    , expr_(expr)
    , ThenPart_(ThenPart)
    , ElsePart_(ElsePart) {}

Value IfStatementNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    std::shared_ptr<Type> boolean =
        manager->GetBuiltinType(BuiltinType::BOOLEAN);
    llvm::Function* body = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* ThenBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "then", body);
    llvm::BasicBlock* EndBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "endif");
    Value ExprValue = manager->CreateCast(boolean, expr_->GetType(),
                                          expr_->CodeGen(), context_);
    if (ElsePart_ != nullptr) {
        llvm::BasicBlock* ElseBlock =
            llvm::BasicBlock::Create(GlobalLLVMContext, "else");
        builder->CreateCondBr(ExprValue, ThenBlock, ElseBlock);
        builder->SetInsertPoint(ThenBlock);
        ThenPart_->CodeGen();
        builder->CreateBr(EndBlock);
        body->getBasicBlockList().push_back(ElseBlock);
        builder->SetInsertPoint(ElseBlock);
        ElsePart_->CodeGen();
        builder->CreateBr(EndBlock);
        body->getBasicBlockList().push_back(EndBlock);
        builder->SetInsertPoint(EndBlock);
        return nullptr;
    }
    builder->CreateCondBr(ExprValue, ThenBlock, EndBlock);
    builder->SetInsertPoint(ThenBlock);
    ThenPart_->CodeGen();
    builder->CreateBr(EndBlock);
    body->getBasicBlockList().push_back(EndBlock);
    builder->SetInsertPoint(EndBlock);
    return nullptr;
}

WhileStatementNode::WhileStatementNode(Context* context,
                                       std::shared_ptr<ExprNode> expr,
                                       std::shared_ptr<BaseNode> WhileBody)
    : BaseNode(context)
    , expr_(expr)
    , WhileBody_(WhileBody) {}

Value WhileStatementNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    std::shared_ptr<Type> boolean =
        manager->GetBuiltinType(BuiltinType::BOOLEAN);
    llvm::Function* body = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* StartBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "start", body);
    llvm::BasicBlock* LoopBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "loop");
    llvm::BasicBlock* EndBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "done");
    builder->SetInsertPoint(StartBlock);
    Value ExprValue = manager->CreateCast(boolean, expr_->GetType(),
                                          expr_->CodeGen(), context_);
    builder->CreateCondBr(ExprValue, LoopBlock, EndBlock);
    body->getBasicBlockList().push_back(LoopBlock);
    builder->SetInsertPoint(LoopBlock);
    WhileBody_->CodeGen();
    builder->CreateBr(StartBlock);
    body->getBasicBlockList().push_back(EndBlock);
    builder->SetInsertPoint(EndBlock);
    return nullptr;
}

RepeatStatementNode::RepeatStatementNode(Context* context,
                                         std::shared_ptr<ExprNode> expr,
                                         std::shared_ptr<BaseNode> RepeatBody)
    : BaseNode(context)
    , expr_(expr)
    , RepeatBody_(RepeatBody) {}

Value RepeatStatementNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    TypeManager* manager = context_->GetTypeManager();
    std::shared_ptr<Type> boolean =
        manager->GetBuiltinType(BuiltinType::BOOLEAN);
    llvm::Function* body = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* LoopBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "loop", body);
    llvm::BasicBlock* EndBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "done");
    builder->SetInsertPoint(LoopBlock);
    RepeatBody_->CodeGen();
    Value ExprValue = manager->CreateCast(boolean, expr_->GetType(),
                                          expr_->CodeGen(), context_);
    builder->CreateCondBr(ExprValue, EndBlock, LoopBlock);
    body->getBasicBlockList().push_back(EndBlock);
    builder->SetInsertPoint(EndBlock);
    return nullptr;
}
} // namespace pcc
