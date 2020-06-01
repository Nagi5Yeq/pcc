#include "VariableList.hh"
#include "Log.hh"

namespace pcc {
VariableList::VariableList(VariableList* parent)
    : parent_(parent) {}

Variable VariableList::FindVariable(const std::string& name) {
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
        Log(LogLevel::PCC_ERROR, "unknown identifier {0}", name);
        return {nullptr, nullptr};
    }
    return std::get<1>(*result);
}

void VariableList::AddVariable(const std::string& name, Variable variable) {
    variables_.insert({name, variable});
}

std::shared_ptr<Type> VariableList::FindTypeAlias(const std::string& name) {
    bool found = false;
    VariableList* level = this;
    decltype(TypeAlias_)::iterator result;
    while (level != nullptr) {
        result = level->TypeAlias_.find(name);
        if (result != level->TypeAlias_.end()) {
            found = true;
            break;
        } else {
            level = level->parent_;
        }
    }
    if (found == false) {
        Log(LogLevel::PCC_ERROR, "unknown type alias {0}", name);
        return nullptr;
    }
    return std::get<1>(*result);
}

void VariableList::AddTypeAlias(const std::string& name,
                                std::shared_ptr<Type> type) {
    TypeAlias_.insert({name, type});
}

} // namespace pcc
