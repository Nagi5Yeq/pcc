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

template <typename T, class Base = BaseNode> class ListNode : public Base {
  public:
    ListNode(Context* context)
        : Base(context) {}
    void Append(T child) { childs_.push_back(child); }
    virtual Value CodeGen();

  private:
    std::list<T> childs_;
};
} // namespace pcc

#endif
