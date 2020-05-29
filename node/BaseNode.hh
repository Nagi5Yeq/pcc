#ifndef PCC_BASENODE_H
#define PCC_BASENODE_H

#include <list>

#include "Context.hh"

namespace pcc {
class BaseNode {
  public:
    BaseNode(Context* context);
    virtual Value CodeGen() = 0;

  protected:
    Context* context_;
};

class FunctionNode;
class DeclNode;

class ProgramNode : public BaseNode {
  public:
    ProgramNode(Context* context, std::string&& name,
                std::list<std::shared_ptr<DeclNode>>&& GlobalDeclarations,
                std::list<std::shared_ptr<FunctionNode>>&& functions);
    virtual Value CodeGen() override;

  protected:
    std::string name_;
    std::list<std::shared_ptr<DeclNode>> GlobalDeclarations_;
    std::list<std::shared_ptr<FunctionNode>> functions_;
};
} // namespace pcc

#endif
