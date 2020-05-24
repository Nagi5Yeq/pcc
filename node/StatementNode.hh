#ifndef PCC_STATEMENTNODE_HH
#define PCC_STATEMENTNODE_HH

#include <memory>

#include "BaseNode.hh"

namespace pcc {
using StatementListNode = ListNode<std::shared_ptr<BaseNode>>;
} // namespace pcc

#endif
