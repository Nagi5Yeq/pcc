#include "Traveler.hh"
#include "BaseNode.hh"

namespace pcc {
Traveler::Traveler(std::ostream& out)
    : out_(out) {}

void Traveler::Travel(std::shared_ptr<BaseNode> root) {
    PreTravel();
    root->Travel(*this);
    PostTravel();
}

void Traveler::PreTravel() {}
void Traveler::PostTravel() {}
} // namespace pcc
