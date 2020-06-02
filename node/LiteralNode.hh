#ifndef PCC_LITERALNODE_H
#define PCC_LITERALNODE_H

#include <string>

#include "BaseNode.hh"
#include "ExprNode.hh"

namespace pcc {
template <typename T>
class LiteralNode : public ExprNode {
  public:
    LiteralNode(Context* context, std::shared_ptr<Type> type, T value);
    virtual Value CodeGen() override;
    virtual int Travel(Traveler& traveler) override;

  private:
    T value_;
};

// the implementation of member functions of template classes must be put in
// header, but we put type-specific CodeGen() in the .cc file.
template <typename T>
LiteralNode<T>::LiteralNode(Context* context, std::shared_ptr<Type> type,
                            T value)
    : ExprNode(context)
    , value_(value) {
    type_ = type;
}

using BooleanLiteralNode = LiteralNode<bool>;
using CharLiteralNode = LiteralNode<char>;
using IntegerLiteralNode = LiteralNode<long long>;
using RealLiteralNode = LiteralNode<float>;

class StringLiteralNode : public ExprNode {
  public:
    StringLiteralNode(Context* context, std::shared_ptr<Type> type,
                      std::vector<char> value);
    virtual Value CodeGen() override;
    virtual int Travel(Traveler& traveler) override;

  private:
    std::vector<char> value_;
};
} // namespace pcc

#endif
