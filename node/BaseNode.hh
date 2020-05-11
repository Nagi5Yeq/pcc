#ifndef PCC_BASE_H
#define PCC_BASE_H

#include <llvm/IR/Value.h>

#include "Driver.hh"

namespace pcc {
using Value = llvm::Value*;

class BaseNode {
  public:
    virtual Value CodeGen() = 0;
    BaseNode(Driver* driver);

  protected:
    Driver* driver_;
};
} // namespace pcc

#endif
