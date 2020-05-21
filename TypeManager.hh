#ifndef PCC_TYPEMANAGER_H
#define PCC_TYPEMANAGER_H

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "Type.hh"

namespace pcc {
enum class BuiltinType { BOOLEAN, CHAR, INTEGER, REAL, STRING, BUILTIN_NUMBER };
class TypeManager {

  public:
    TypeManager();
    std::shared_ptr<Type> GetBuiltinType(BuiltinType type);

    // binary operators may change type
    std::pair<std::shared_ptr<Type>, Value>
        CreateOperation(BinaryOperator op, std::shared_ptr<Type> LeftType,
                        std::shared_ptr<Type> RightType, Value left,
                        Value right, Context* context);

    // unary operators never change type
    Value CreateOperation(UnaryOperator op, std::shared_ptr<Type> type, Value v,
                          Context* context);

  private:
    std::vector<std::shared_ptr<Type>> builtins_;
    std::vector<std::shared_ptr<Type>> types_;

    // if we are porting to some 64-bit machines, we can change these types to
    // 64-bit type, in order to make pointers behave correctly
    std::shared_ptr<Type> PointerDifferenceType_;
    std::shared_ptr<Type> PointerIndexType_;
};
} // namespace pcc

#endif // !PCC_TYPEMANAGER_H
