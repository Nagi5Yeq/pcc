#ifndef PCC_BASENODE_H
#define PCC_BASENODE_H

#include <llvm/IR/Value.h>

#include "Context.hh"

namespace pcc {
class BaseNode {
  public:
    virtual Value CodeGen() = 0;
    BaseNode(Context* context);

  protected:
    Context* context_;
};
} // namespace pcc

#endif
