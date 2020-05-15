#ifndef PCC_DRIVER_H
#define PCC_DRIVER_H

#include <cstdio>

#include "Context.hh"
#include "Location.hh"

namespace pcc {
class Driver {
  public:
    Driver(const char* name);
    ~Driver();
    int StartLexer(std::string& filename);
    void StopLexer();
    void Parse(std::string& filename);
    Location location_;

  private:
    Context context_;
};
} // namespace pcc

#endif
