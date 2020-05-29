#ifndef PCC_DECLNODE_HH
#define PCC_DECLNODE_HH

#include <list>

#include "BaseNode.hh"
#include "LiteralNode.hh"

namespace pcc {
class DeclNode : public BaseNode {
  public:
    DeclNode(Context* context, VariableList* scope);
    // the scope of variable is an inherited attribute, but we don't have a good
    // way to set inherited attribute during the construction of AST, so we call
    // this in parent's constructor.
    void SetScope(VariableList* scope);
    virtual Value CodeGen() = 0;

  protected:
    VariableList* scope_;
};

class VarDeclNode : public DeclNode {
  public:
    VarDeclNode(Context* context, VariableList* scope,
                std::list<Declaration>&& decls);
    virtual Value CodeGen() override;

  protected:
    std::list<Declaration> decls_;
};

// name and type
using Constant = std::pair<std::string, std::shared_ptr<ExprNode>>;

class ConstDeclNode : public DeclNode {
  public:
    ConstDeclNode(Context* context, VariableList* scope,
                  std::list<Constant>&& decls);
    virtual Value CodeGen() override;

  protected:
    std::list<Constant> decls_;
};
} // namespace pcc

#endif // !PCC_DECLNODE_HH
