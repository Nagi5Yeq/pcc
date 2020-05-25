#ifndef PCC_STATEMENTNODE_HH
#define PCC_STATEMENTNODE_HH

#include <memory>

#include "BaseNode.hh"
#include "ExprNode.hh"

namespace pcc {
using StatementListNode = ListNode<std::shared_ptr<BaseNode>>;

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
} // namespace pcc

#endif
