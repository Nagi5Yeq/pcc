#include "VariableList.hh"
#include "Log.hh"

namespace pcc {
VariableList::VariableList(VariableList* parent)
    : parent_(parent) {}

Variable VariableList::Find(const std::string& name) {
    bool found = false;
    VariableList* level = this;
    decltype(variables_)::iterator result;
    while (level != nullptr) {
        result = level->variables_.find(name);
        if (result != level->variables_.end()) {
            found = true;
            break;
        } else {
            level = level->parent_;
        }
    }
    if (found == false) {
        Log(LogLevel::PCC_ERROR, "unknown identifier %s", name.c_str());
        return {nullptr, nullptr};
    }
    return std::get<1>(*result);
}

void VariableList::Add(std::string name, Variable variable) {
    variables_.insert({name, variable});
}
} // namespace pcc
