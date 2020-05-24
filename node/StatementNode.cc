#include "StatementNode.hh"

namespace pcc {
template <> Value StatementListNode::CodeGen() {
    Value result = nullptr;
    for (auto& child : childs_) {
        result = child->CodeGen();
    }
    return result;
}
} // namespace pcc
