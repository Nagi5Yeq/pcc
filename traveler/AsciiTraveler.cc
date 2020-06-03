#include "AsciiTraveler.hh"
#include "Color.hh"

namespace pcc {
const std::string AsciiTraveler::MiddleLevelPrefix_ = "|-";
const std::string AsciiTraveler::LastLevelPrefix_ = "`-";
const std::string AsciiTraveler::MiddleChildPrefix_ = "| ";
const std::string AsciiTraveler::LastChildPrefix_ = "  ";
const size_t AsciiTraveler::LevelWidth_ =
    AsciiTraveler::MiddleLevelPrefix_.size();

AsciiTraveler::AsciiTraveler(std::ostream& out)
    : Traveler(out)
    , prefix_("")
    , random_(13)
    , dist_(0, 1) {
    IsLastChild_.push(true);
}

void AsciiTraveler::PostTravel() { out_ << Color::DEFAULT; }

Traveler& AsciiTraveler::operator<<(TravelPart part) {
    switch (part) {
    case TravelPart::PREFIX:
        out_ << Color::BLUE << prefix_
             << (IsLastChild_.top() ? LastLevelPrefix_ : MiddleLevelPrefix_);
        break;
    case TravelPart::NAME_BEGIN:
        out_ << (dist_(random_) == 1 ? Color::MAGENTA : Color::GREEN);
        break;
    case TravelPart::NAME_END:
        break;
    case TravelPart::DESCRPTION_BEGIN:
        out_ << (dist_(random_) == 1 ? Color::CYAN : Color::YELLOW) << " ";
        break;
    case TravelPart::DESCRPTION_END:
        out_ << "\n";
        break;
    case TravelPart::CHILD_BEGIN:
        prefix_.append(IsLastChild_.top() ? LastChildPrefix_
                                          : MiddleChildPrefix_);
        IsLastChild_.push(false);
        break;
    case TravelPart::LAST_CHILD_BEGIN:
        prefix_.append(IsLastChild_.top() ? LastChildPrefix_
                                          : MiddleChildPrefix_);
        IsLastChild_.push(true);
        break;
    case TravelPart::CHILD_END:
    case TravelPart::LAST_CHILD_END:
        prefix_.erase(prefix_.size() - LevelWidth_);
        IsLastChild_.pop();
        break;
    case TravelPart::END:
        break;
    default:
        break;
    }
    return *this;
}
} // namespace pcc
