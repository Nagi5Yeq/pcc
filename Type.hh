#ifndef PCC_TYPE_H
#define PCC_TYPE_H

#include <memory>
#include <string>
#include <vector>

#include "Context.hh"

namespace pcc {
enum class BinaryOperator {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    LT,
    LE,
    GT,
    GE,
    EQ,
    NE,
    AND,
    OR,
    XOR,
    ARRAY_ACCESS,
    OPERATOR_NUMBER
};

template <typename T>
constexpr std::underlying_type_t<T> ToUnderlying(T value) {
    return static_cast<std::underlying_type_t<T>>(value);
}

class Type {
  public:
    Type(const char* name, bool IsBasicType, int width);
    virtual const char* GetCommonName();

    // return -1 if the operation is not supported
    virtual int CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context);

  protected:
    int NotAllowed(Value v0, Value v1, Context* context);

    int CreateAdd(Value v0, Value v1, Context* context);
    int CreateSub(Value v0, Value v1, Context* context);
    int CreateMul(Value v0, Value v1, Context* context);
    int CreateDiv(Value v0, Value v1, Context* context);
    int CreateMod(Value v0, Value v1, Context* context);
    int CreateLt(Value v0, Value v1, Context* context);
    int CreateLe(Value v0, Value v1, Context* context);
    int CreateGt(Value v0, Value v1, Context* context);
    int CreateGe(Value v0, Value v1, Context* context);
    int CreateEq(Value v0, Value v1, Context* context);
    int CreateNe(Value v0, Value v1, Context* context);
    int CreateAnd(Value v0, Value v1, Context* context);
    int CreateOr(Value v0, Value v1, Context* context);
    int CreateXor(Value v0, Value v1, Context* context);
    int CreateFAdd(Value v0, Value v1, Context* context);
    int CreateFSub(Value v0, Value v1, Context* context);
    int CreateFMul(Value v0, Value v1, Context* context);
    int CreateFDiv(Value v0, Value v1, Context* context);
    int CreateFMod(Value v0, Value v1, Context* context);
    int CreateFLt(Value v0, Value v1, Context* context);
    int CreateFLe(Value v0, Value v1, Context* context);
    int CreateFGt(Value v0, Value v1, Context* context);
    int CreateFGe(Value v0, Value v1, Context* context);
    int CreateFEq(Value v0, Value v1, Context* context);
    int CreateFNe(Value v0, Value v1, Context* context);
    int CreateArrayAccess(Value v0, Value v1, Context* context);
    const char* name_;
    bool IsBasicType_;
    int width_;
};

class BooleanType : public Type {
  public:
    BooleanType();
    virtual int CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) override;
};

class CharType : public Type {
  public:
    CharType();
    virtual int CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) override;
};

class IntegerType : public Type {
  public:
    IntegerType();
    virtual int CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) override;
};

class RealType : public Type {
  public:
    RealType();
    virtual int CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                      Context* context) override;
};

} // namespace pcc

#endif // PCC_TYPE_H
