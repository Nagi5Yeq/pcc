#ifndef PCC_FUNCTIONNODE_H
#define PCC_FUNCTIONNODE_H

#include <list>

#include "BaseNode.hh"
#include "StatementNode.hh"
#include "VariableList.hh"

namespace pcc {
class FunctionNode : public BaseNode {
  public:
    FunctionNode(Context* context, std::string&& name,
                 std::list<Declaration>&& arguments,
                 std::list<std::shared_ptr<DeclNode>>&& LocalDeclarations,
                 std::shared_ptr<Type> ReturnType,
                 std::shared_ptr<StatementListNode> body);
    virtual Value CodeGen() override;

  protected:
    std::string name_;
    std::list<Declaration> arguments_;
    std::list<std::shared_ptr<DeclNode>> LocalDeclarations_;
    std::shared_ptr<Type> ReturnType_;
    VariableList locals_;
    std::shared_ptr<StatementListNode> body_;
};
} // namespace pcc

#endif
