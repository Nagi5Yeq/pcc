#ifndef PCC_STATEMENTNODE_HH
#define PCC_STATEMENTNODE_HH

#include <memory>

#include "BaseNode.hh"
#include "ExprNode.hh"

namespace pcc {
class StatementListNode : public BaseNode {
  public:
    StatementListNode(Context* context);
    void Append(std::shared_ptr<BaseNode> child);
    virtual Value CodeGen();

  private:
    std::list<std::shared_ptr<BaseNode>> childs_;
};

class EmptyStatementNode : public BaseNode {
  public:
    EmptyStatementNode(Context* context);
    virtual Value CodeGen() override;
};

class AssignStatementNode : public BaseNode {
  public:
    AssignStatementNode(Context* context, std::shared_ptr<ExprNode> lhs,
                        std::shared_ptr<ExprNode> rhs);
    virtual Value CodeGen() override;

  protected:
    std::shared_ptr<ExprNode> lhs_;
    std::shared_ptr<ExprNode> rhs_;
};

class IfStatementNode : public BaseNode {
  public:
    IfStatementNode(Context* context, std::shared_ptr<ExprNode> expr,
                    std::shared_ptr<BaseNode> ThenPart,
                    std::shared_ptr<BaseNode> ElsePart);
    virtual Value CodeGen() override;

  protected:
    std::shared_ptr<ExprNode> expr_;
    std::shared_ptr<BaseNode> ThenPart_;
    std::shared_ptr<BaseNode> ElsePart_;
};

class WhileStatementNode : public BaseNode {
  public:
    WhileStatementNode(Context* context, std::shared_ptr<ExprNode> expr,
                       std::shared_ptr<BaseNode> WhileBody);
    virtual Value CodeGen() override;

  protected:
    std::shared_ptr<ExprNode> expr_;
    std::shared_ptr<BaseNode> WhileBody_;
};

class RepeatStatementNode : public BaseNode {
  public:
    RepeatStatementNode(Context* context, std::shared_ptr<ExprNode> expr,
                        std::shared_ptr<BaseNode> RepeatBody);
    virtual Value CodeGen() override;

  protected:
    std::shared_ptr<ExprNode> expr_;
    std::shared_ptr<BaseNode> RepeatBody_;
};
} // namespace pcc

#endif
