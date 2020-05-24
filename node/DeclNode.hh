#ifndef PCC_DECLNODE_HH
#define PCC_DECLNODE_HH

#include <list>

#include "BaseNode.hh"

namespace pcc {
class VarDeclNode : public BaseNode {
  public:
    VarDeclNode(Context* context, VariableList* scope,
                std::list<Declaration> decls);

    // the scope of variable is an inherited attribute, but we don't have a good
    // way to set inherited attribute during the construction of AST, so we call
    // this in parent's constructor
    void SetScope(VariableList* scope);
    virtual Value CodeGen() override;

  protected:
    std::list<Declaration> decls_;
    VariableList* scope_;
};
} // namespace pcc

#endif // !PCC_DECLNODE_HH
