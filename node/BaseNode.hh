#ifndef PCC_BASENODE_H
#define PCC_BASENODE_H

#include <llvm/IR/Value.h>

namespace pcc {
using Value = llvm::Value*;
class Driver;

class BaseNode {
  public:
    virtual Value CodeGen() = 0;
    BaseNode(Driver* driver);

  protected:
    Driver* driver_;
};
} // namespace pcc

#endif
