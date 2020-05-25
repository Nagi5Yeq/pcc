#ifndef PCC_DRIVER_H
#define PCC_DRIVER_H

#include <cstdio>

#include "BaseNode.hh"
#include "Context.hh"
#include "Location.hh"

namespace pcc {
class Driver {
  public:
    Driver(const char* name);
    ~Driver();
    int StartLexer(std::string& filename);
    void StopLexer();
    std::shared_ptr<ProgramNode> Parse(std::string& filename);
    void SetRoot(std::shared_ptr<ProgramNode> root);
    Context* GetContext();
    Location location_;

  private:
    Context context_;
    std::shared_ptr<ProgramNode> root_;
};
} // namespace pcc

#endif
