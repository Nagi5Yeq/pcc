#ifndef PCC_ASCIITRAVELER_HH
#define PCC_ASCIITRAVELER_HH

#include <stack>
#include <string>

#include "Traveler.hh"

namespace pcc {
class AsciiTraveler : public Traveler {
  public:
    AsciiTraveler(std::ostream& out);
    virtual Traveler& operator<<(TravelPart part);

  protected:
    int level_;
    std::string prefix_;
    std::stack<bool> IsLastChild_;

    static const std::string MiddleLevelPrefix_;
    static const std::string LastLevelPrefix_;
    static const std::string MiddleChildPrefix_;
    static const std::string LastChildPrefix_;
    static const size_t LevelWidth_;
};
} // namespace pcc

#endif // !PCC_ASCIITRAVELER_HH
