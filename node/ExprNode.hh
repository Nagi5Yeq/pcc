#ifndef PCC_EXPRNODE_H
#define PCC_EXPRNODE_H

#include "BaseNode.hh"
#include "Type.hh"

namespace pcc {
class ExprNode : public BaseNode {
  public:
    ExprNode(Context* context);
    virtual Value CodeGen() = 0;
    std::shared_ptr<Type> GetType();

  protected:
    std::shared_ptr<Type> type_;
};

class BinaryExprNode : public ExprNode {
  public:
    BinaryExprNode(Context* context, BinaryOperator op,
                   std::shared_ptr<ExprNode> left,
                   std::shared_ptr<ExprNode> right);
    virtual Value CodeGen();

  protected:
    BinaryOperator op_;
    std::shared_ptr<ExprNode> left_, right_;
};

class UnaryExprNode : public ExprNode {
  public:
    UnaryExprNode(Context* context, UnaryOperator op,
                  std::shared_ptr<ExprNode> value);
    virtual Value CodeGen();

  protected:
    UnaryOperator op_;
    std::shared_ptr<ExprNode> value_;
};

} // namespace pcc

#endif // !PCC_EXPRNODE_H
