#ifndef PCC_DOTTRAVELER_HH
#define PCC_DOTTRAVELER_HH

#include <stack>

#include "Traveler.hh"

namespace pcc {
class DotTraveler : public Traveler {
  public:
    DotTraveler(std::ostream& out);
    virtual Traveler& operator<<(TravelPart part) override;

  protected:
    virtual void PreTravel() override;
    virtual void PostTravel() override;
    std::stack<int> NodeIndex_;
    int CurrentIndex_;
};
} // namespace pcc

#endif // !PCC_DOTTRAVELER_HH
