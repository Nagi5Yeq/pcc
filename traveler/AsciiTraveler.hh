#ifndef PCC_ASCIITRAVELER_HH
#define PCC_ASCIITRAVELER_HH

#include <random>
#include <stack>
#include <string>

#include "Traveler.hh"

namespace pcc {
class AsciiTraveler : public Traveler {
  public:
    AsciiTraveler(std::ostream& out);
    virtual Traveler& operator<<(TravelPart part) override;

  protected:
    void PostTravel() override;
    std::string prefix_;
    std::stack<bool> IsLastChild_;
    std::mt19937 random_;
    std::uniform_int_distribution<int> dist_;

    static const std::string MiddleLevelPrefix_;
    static const std::string LastLevelPrefix_;
    static const std::string MiddleChildPrefix_;
    static const std::string LastChildPrefix_;
    static const size_t LevelWidth_;
};
} // namespace pcc

#endif // !PCC_ASCIITRAVELER_HH
