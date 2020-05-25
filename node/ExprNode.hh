#ifndef PCC_EXPRNODE_H
#define PCC_EXPRNODE_H

#include <list>

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

// when accessing lvalues, we need an extra load instruction to actually load
// the value from the pointer, this is like clang's 'LValueToRValue' node below.
// c++: int x=7; int y=x;
//  `-CompoundStmt 0xd28720 <col:9, line:9:1>
//    |-DeclStmt 0xd28638 <line:7:2, col:9>
//    | `-VarDecl 0xd285b0 <col:2, col:8> col:6 used x 'int' cinit
//    |   `-IntegerLiteral 0xd28618 <col:8> 'int' 7
//    `-DeclStmt 0xd28708 <line:8:2, col:9>
//      `-VarDecl 0xd28668 <col:2, col:8> col:6 y 'int' cinit
//        `-ImplicitCastExpr 0xd286f0 <col:8> 'int' <LValueToRValue>
//          `-DeclRefExpr 0xd286d0 <col:8> 'int' lvalue Var 0xd285b0 'x' 'int'
class L2RCastingNode : public ExprNode {
  public:
    L2RCastingNode(Context* context, std::shared_ptr<ExprNode> lvalue);
    virtual Value CodeGen() override;

  protected:
    std::shared_ptr<ExprNode> lvalue_;
};

class IdentifierNode : public ExprNode {
  public:
    IdentifierNode(Context* context, const std::string& name);
    virtual Value CodeGen() override;

  protected:
    std::string name_;
};

class FunctionCallNode : public ExprNode {
  public:
    FunctionCallNode(Context* context, const std::string& name,
                     std::list<std::shared_ptr<ExprNode>> args);
    virtual Value CodeGen() override;

  protected:
    std::string name_;
    std::list<std::shared_ptr<ExprNode>> args_;
};

} // namespace pcc

#endif // !PCC_EXPRNODE_H
