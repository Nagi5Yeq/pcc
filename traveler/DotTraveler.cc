#include "DotTraveler.hh"

namespace pcc {
DotTraveler::DotTraveler(std::ostream& out)
    : Traveler(out)
    , CurrentIndex_(0) {}

void DotTraveler::PreTravel() {
    out_ << "digraph \"AST dump\"\n"
            "{\n"
            "  node [fontname = courier, shape = box, colorscheme = paired6]\n"
            "  edge [fontname = courier]\n\n";
}

void DotTraveler::PostTravel() { out_ << "}\n"; }

Traveler& DotTraveler::operator<<(TravelPart part) {
    switch (part) {
    case TravelPart::PREFIX:
        out_ << "  " << CurrentIndex_ << " [label=\"";
        NodeIndex_.push(CurrentIndex_);
        ++CurrentIndex_;
        break;
    case TravelPart::NAME_BEGIN:
    case TravelPart::DESCRPTION_BEGIN:
        break;
    case TravelPart::NAME_END:
        out_ << "\\n\\n";
        break;
    case TravelPart::DESCRPTION_END:
        out_ << "\"]\n";
        break;
    case TravelPart::CHILD_BEGIN:
    case TravelPart::LAST_CHILD_BEGIN:
        out_ << "  " << NodeIndex_.top() << " -> " << CurrentIndex_
             << " [style=solid]\n";
        break;
    case TravelPart::CHILD_END:
    case TravelPart::LAST_CHILD_END:
        break;
    case TravelPart::END:
        NodeIndex_.pop();
        break;
    default:
        break;
    }
    return *this;
}
} // namespace pcc
