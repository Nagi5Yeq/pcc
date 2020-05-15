#ifndef PCC_TYPEMANAGER_H
#define PCC_TYPEMANAGER_H

#include "Type.hh"

namespace pcc {
class TypeManager {
  public:
    TypeManager();

  private:
    std::vector<std::shared_ptr<Type>> types_;
};
} // namespace pcc

#endif // !PCC_TYPEMANAGER_H
