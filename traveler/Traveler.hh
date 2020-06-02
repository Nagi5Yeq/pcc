#ifndef PCC_TRAVELER_HH
#define PCC_TRAVELER_HH

#include <ostream>

#include "Type.hh"
#include "TypeIdentifier.hh"

namespace pcc {
enum class TravelPart {
    PREFIX,
    NAME_BEGIN,
    NAME_END,
    DESCRPTION_BEGIN,
    DESCRPTION_END,
    CHILD_BEGIN,
    CHILD_END,
    LAST_CHILD_BEGIN,
    LAST_CHILD_END,
    END
};
class Traveler {
  public:
    Traveler(std::ostream& out);
    virtual Traveler& operator<<(TravelPart part) = 0;
    Traveler& operator<<(std::shared_ptr<Type> type);
    Traveler& operator<<(std::shared_ptr<TypeIdentifier> identifier);

    template <typename T>
    Traveler& operator<<(T value) {
        out_ << value;
        return *this;
    }

  protected:
    std::ostream& out_;
};
} // namespace pcc

#endif // !PCC_TRAVELER_HH
