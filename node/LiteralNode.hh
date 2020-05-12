#ifndef PCC_LITERALNODE_H
#define PCC_LITERALNODE_H

#include <string>

#include "BaseNode.hh"

namespace pcc {
class StringLiteralNode : public BaseNode {
  public:
    StringLiteralNode(Driver* driver, char* value);
    Value CodeGen() override;

  private:
    std::string value_;
};
} // namespace pcc

#endif
