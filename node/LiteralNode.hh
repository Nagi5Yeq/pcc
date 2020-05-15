#ifndef PCC_LITERALNODE_H
#define PCC_LITERALNODE_H

#include <string>

#include "BaseNode.hh"

namespace pcc {
template <typename Type, typename Construct = Type>
class LiteralNode : public BaseNode {
  public:
    LiteralNode(Context* context, Construct value);
    Value CodeGen() override; // type-specific CodeGen() is in the .cc file.

  private:
    Type value_;
};

template <typename Type, typename Construct>
LiteralNode<Type, Construct>::LiteralNode(Context* context, Construct value)
    : BaseNode(context)
    , value_(value) {}

using BooleanLiteralNode = LiteralNode<bool>;
using IntegerLiteralNode = LiteralNode<int>;
using RealLiteralNode = LiteralNode<float>;
using StringLiteralNode = LiteralNode<std::string, char*>;
} // namespace pcc

#endif
