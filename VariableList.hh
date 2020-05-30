#ifndef PCC_VARIABLELIST_HH
#define PCC_VARIABLELIST_HH

#include <memory>
#include <string>
#include <unordered_map>

#include "Type.hh"

namespace pcc {
class TypeIdentifier;

// type and value
using Variable = std::pair<std::shared_ptr<Type>, Value>;
// name and type
using Declaration = std::pair<std::string, std::shared_ptr<TypeIdentifier>>;

class VariableList {
  public:
    VariableList(VariableList* parent = nullptr);
    Variable FindVariable(const std::string& name);
    void AddVariable(const std::string& name, Variable variable);

    // stores type alias, just because we wan't create another TypeAliasList
    std::shared_ptr<Type> FindTypeAlias(const std::string& name);
    void AddTypeAlias(const std::string& name, std::shared_ptr<Type> type);

  protected:
    std::unordered_map<std::string, Variable> variables_;
    std::unordered_map<std::string, std::shared_ptr<Type>> TypeAlias_;
    VariableList* parent_;
};
} // namespace pcc

#endif // !PCC_VARIABLELIST_HH
