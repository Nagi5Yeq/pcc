#ifndef PCC_VARIABLELIST_HH
#define PCC_VARIABLELIST_HH

#include <memory>
#include <string>
#include <unordered_map>

#include "Type.hh"

namespace pcc {
// type and value
using Variable = std::pair<std::shared_ptr<Type>, Value>;
// name and type
using Declaration = std::pair<std::string, std::shared_ptr<Type>>;

class VariableList {
  public:
    VariableList(VariableList* parent = nullptr);
    Variable Find(const std::string& name);
    void Add(std::string name, Variable variable);

  protected:
    std::unordered_map<std::string, Variable> variables_;
    VariableList* parent_;
};
} // namespace pcc

#endif // !PCC_VARIABLELIST_HH
