#ifndef PCC_BASENODE_H
#define PCC_BASENODE_H

#include "Context.hh"

namespace pcc {
class BaseNode {
  public:
    BaseNode(Context* context);
    virtual Value CodeGen() = 0;

  protected:
    Context* context_;
};
} // namespace pcc

#endif
