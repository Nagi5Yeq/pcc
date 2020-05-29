#include "StatementNode.hh"
#include "Log.hh"

namespace pcc {
StatementListNode::StatementListNode(Context* context)
    : BaseNode(context) {}

void StatementListNode::Append(std::shared_ptr<BaseNode> child) {
    childs_.push_back(child);
}

Value StatementListNode::CodeGen() {
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
    Value cond = expr_->CodeGen();
    cond = manager->CreateCast(boolean, expr_->GetType(), cond, context_);
    if (ElsePart_ != nullptr) {
        llvm::BasicBlock* ElseBlock =
            llvm::BasicBlock::Create(GlobalLLVMContext, "else");
        builder->CreateCondBr(cond, ThenBlock, ElseBlock);
        builder->SetInsertPoint(ThenBlock);
        // see Context.hh for detail
        context_->SetJumpingFlag(false);
        ThenPart_->CodeGen();
        if (context_->GetJumpingFlag() == false) {
            builder->CreateBr(EndBlock);
        }
        context_->SetJumpingFlag(false);
        body->getBasicBlockList().push_back(ElseBlock);
        builder->SetInsertPoint(ElseBlock);
        context_->SetJumpingFlag(false);
        ElsePart_->CodeGen();
        if (context_->GetJumpingFlag() == false) {
            builder->CreateBr(EndBlock);
        }
        context_->SetJumpingFlag(false);
        body->getBasicBlockList().push_back(EndBlock);
        builder->SetInsertPoint(EndBlock);
        return nullptr;
    }
    builder->CreateCondBr(cond, ThenBlock, EndBlock);
    builder->SetInsertPoint(ThenBlock);
    context_->SetJumpingFlag(false);
    ThenPart_->CodeGen();
    if (context_->GetJumpingFlag() == false) {
        builder->CreateBr(EndBlock);
    }
    context_->SetJumpingFlag(false);
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
    builder->CreateBr(StartBlock);
    builder->SetInsertPoint(StartBlock);
    Value cond = expr_->CodeGen();
    cond = manager->CreateCast(boolean, expr_->GetType(), cond, context_);
    builder->CreateCondBr(cond, LoopBlock, EndBlock);
    body->getBasicBlockList().push_back(LoopBlock);
    builder->SetInsertPoint(LoopBlock);
    context_->PushBreakDestination(EndBlock);
    context_->PushContinueDestination(StartBlock);
    WhileBody_->CodeGen();
    context_->PopBreakDestination();
    context_->PopContinueDestination();
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
    llvm::BasicBlock* CheckBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "check");
    llvm::BasicBlock* EndBlock =
        llvm::BasicBlock::Create(GlobalLLVMContext, "done");
    builder->CreateBr(LoopBlock);
    builder->SetInsertPoint(LoopBlock);
    context_->PushBreakDestination(EndBlock);
    context_->PushContinueDestination(CheckBlock);
    RepeatBody_->CodeGen();
    context_->PopBreakDestination();
    context_->PopContinueDestination();
    builder->CreateBr(CheckBlock);
    body->getBasicBlockList().push_back(CheckBlock);
    builder->SetInsertPoint(CheckBlock);
    Value cond = expr_->CodeGen();
    cond = manager->CreateCast(boolean, expr_->GetType(), cond, context_);
    builder->CreateCondBr(cond, EndBlock, LoopBlock);
    body->getBasicBlockList().push_back(EndBlock);
    builder->SetInsertPoint(EndBlock);
    return nullptr;
}

BreakStatementNode::BreakStatementNode(Context* context)
    : BaseNode(context) {}

Value BreakStatementNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    llvm::BasicBlock* dest = context_->GetBreakDestination();
    if (dest == nullptr) {
        Log(LogLevel::PCC_ERROR,
            "break statement not in loop or switch statement");
        return nullptr;
    }
    builder->CreateBr(dest);
    context_->SetJumpingFlag(true);
    return nullptr;
}

ContinueStatementNode::ContinueStatementNode(Context* context)
    : BaseNode(context) {}

Value ContinueStatementNode::CodeGen() {
    llvm::IRBuilder<>* builder = context_->GetBuilder();
    llvm::BasicBlock* dest = context_->GetContinueDestination();
    if (dest == nullptr) {
        Log(LogLevel::PCC_ERROR, "continue statement not in loop statement ");
        return nullptr;
    }
    builder->CreateBr(dest);
    context_->SetJumpingFlag(true);
    return nullptr;
}
} // namespace pcc
