#ifndef PCC_TYPEMANAGER_H
#define PCC_TYPEMANAGER_H

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "Type.hh"

namespace pcc {
class ExprNode;

enum class BuiltinType {
    VOID,
    BOOLEAN,
    CHAR,
    INTEGER,
    REAL,
    STRING,
    BUILTIN_NUMBER
};
class TypeManager {

  public:
    TypeManager();
    std::shared_ptr<Type> GetBuiltinType(BuiltinType type);

    // if call this function twice with the same ElementType, it will return the
    // same shared_ptr to the Type.
    std::shared_ptr<PointerType>
        GetPointerType(std::shared_ptr<Type> ElementType,
                       const char* CustomName = nullptr);

    // array types and function types are considered unique, unless use type
    // alias, you can't get the same type as previous call to this function.
    std::shared_ptr<ArrayType>
        CreateArrayType(std::shared_ptr<Type> ElementType,
                        std::shared_ptr<ExprNode> start,
                        std::shared_ptr<ExprNode> end);

    std::shared_ptr<FunctionType>
        CreateFunctionType(std::shared_ptr<Type> ReturnType,
                           std::vector<std::shared_ptr<Type>> ArgTypes);

    // binary operators may change type
    std::pair<std::shared_ptr<Type>, Value>
        CreateOperation(BinaryOperator op, std::shared_ptr<Type> LeftType,
                        std::shared_ptr<Type> RightType, Value left,
                        Value right, Context* context);

    // unary operators never change type
    Value CreateOperation(UnaryOperator op, std::shared_ptr<Type> type, Value v,
                          Context* context);

    Value CreateCast(std::shared_ptr<Type> DstType,
                     std::shared_ptr<Type> SrcType, Value v, Context* context);

  private:
    std::vector<std::shared_ptr<Type>> builtins_;
    std::map<std::shared_ptr<Type>, std::shared_ptr<PointerType>> PointerTypes_;
    std::vector<std::shared_ptr<FunctionType>> FunctionTypes_;
    std::vector<std::shared_ptr<ArrayType>> ArrayTypes_;

    // if we are porting to some 64-bit machines, we can change these types to
    // 64-bit type, in order to make pointers behave correctly
    std::shared_ptr<Type> PointerDifferenceType_;
    std::shared_ptr<Type> PointerIndexType_;
};
} // namespace pcc

#endif // !PCC_TYPEMANAGER_H
