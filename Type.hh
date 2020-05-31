#ifndef PCC_TYPE_H
#define PCC_TYPE_H

#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

namespace pcc {
using Value = llvm::Value*;
class Context;

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
    SHL,
    SHR,
    BINARYOP_NUMBER,
    REAL_DIV // the difference between REAL_DIV and DIV is handled in
             // TypeManager, thus we don't need to create instruction for this.
};

enum class UnaryOperator { POS, NEG, NOT, UNARYOP_NUMEBR };

const char* GetOperatorName(BinaryOperator op);
const char* GetOperatorName(UnaryOperator op);

template <typename T>
constexpr std::underlying_type_t<T> ToUnderlying(T value) {
    return static_cast<std::underlying_type_t<T>>(value);
}

class Type {
  public:
    Type(std::string&& name, llvm::Type* LLVMType);
    virtual const char* GetCommonName();
    llvm::Type* GetLLVMType();

    // return nullptr if the operation is not supported
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context);
    virtual Value CreateUnaryOperation(UnaryOperator op, Value v,
                                       Context* context);
    virtual llvm::Constant* GetZeroInitializer();

  protected:
    Value NotAllowed(Value v0, Value v1, Context* context);
    Value NotAllowed(Value v, Context* context);

    Value CreateAdd(Value v0, Value v1, Context* context);
    Value CreateSub(Value v0, Value v1, Context* context);
    Value CreateMul(Value v0, Value v1, Context* context);
    Value CreateDiv(Value v0, Value v1, Context* context);
    Value CreateMod(Value v0, Value v1, Context* context);
    Value CreateLt(Value v0, Value v1, Context* context);
    Value CreateLe(Value v0, Value v1, Context* context);
    Value CreateGt(Value v0, Value v1, Context* context);
    Value CreateGe(Value v0, Value v1, Context* context);
    Value CreateEq(Value v0, Value v1, Context* context);
    Value CreateNe(Value v0, Value v1, Context* context);
    Value CreateAnd(Value v0, Value v1, Context* context);
    Value CreateOr(Value v0, Value v1, Context* context);
    Value CreateXor(Value v0, Value v1, Context* context);
    Value CreateShl(Value v0, Value v1, Context* context);
    Value CreateShr(Value v0, Value v1, Context* context);
    Value CreatePos(Value v, Context* context);
    Value CreateNeg(Value v, Context* context);
    Value CreateNot(Value v, Context* context);

    Value CreateFAdd(Value v0, Value v1, Context* context);
    Value CreateFSub(Value v0, Value v1, Context* context);
    Value CreateFMul(Value v0, Value v1, Context* context);
    Value CreateFDiv(Value v0, Value v1, Context* context);
    Value CreateFMod(Value v0, Value v1, Context* context);
    Value CreateFLt(Value v0, Value v1, Context* context);
    Value CreateFLe(Value v0, Value v1, Context* context);
    Value CreateFGt(Value v0, Value v1, Context* context);
    Value CreateFGe(Value v0, Value v1, Context* context);
    Value CreateFEq(Value v0, Value v1, Context* context);
    Value CreateFNe(Value v0, Value v1, Context* context);
    Value CreateFPos(Value v, Context* context);
    Value CreateFNeg(Value v, Context* context);

    std::string name_;
    llvm::Type* LLVMType_;
};

class VoidType : public Type {
  public:
    VoidType();
};

class IntegerBaseType : public Type {
  public:
    IntegerBaseType(std::string&& name, int width, llvm::Type* LLVMType);
    int GetWidth();

  protected:
    int width_;
};

class BooleanType : public IntegerBaseType {
  public:
    BooleanType();
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context) override;
    virtual Value CreateUnaryOperation(UnaryOperator op, Value v,
                                       Context* context) override;
};

class CharType : public IntegerBaseType {
  public:
    CharType();
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context) override;
    virtual Value CreateUnaryOperation(UnaryOperator op, Value v,
                                       Context* context) override;
};

class IntegerType : public IntegerBaseType {
  public:
    IntegerType();
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context) override;
    virtual Value CreateUnaryOperation(UnaryOperator op, Value v,
                                       Context* context) override;
};

class RealType : public Type {
  public:
    RealType();
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context) override;
    virtual Value CreateUnaryOperation(UnaryOperator op, Value v,
                                       Context* context) override;
};

class ArrayType : public Type {
  public:
    ArrayType(std::shared_ptr<Type> ElementType, uint64_t ElementNumber,
              std::shared_ptr<Type> IndexType, Value start, Value end,
              std::string&& name, bool IsZeroStarted = false);
    std::shared_ptr<Type> GetElementType();
    std::shared_ptr<Type> GetIndexType();
    bool GetIsZeroStarted();
    Value GetIndexStart();

  protected:
    std::shared_ptr<Type> ElementType_;
    std::shared_ptr<Type> IndexType_;
    bool IsZeroStarted_;
    Value start_, end_;
};

class PointerType : public Type {
  public:
    PointerType(std::shared_ptr<Type> ElementType,
                std::shared_ptr<Type> DifferenceType,
                std::shared_ptr<Type> IndexType, std::string&& name);
    std::shared_ptr<Type> GetElementType();
    std::shared_ptr<Type> GetIndexType();
    std::shared_ptr<Type> GeDiifferenceType();
    virtual Value CreateBinaryOperation(BinaryOperator op, Value v0, Value v1,
                                        Context* context) override;

  protected:
    Value CreatePointerSub(Value v0, Value v1, Context* context);
    std::shared_ptr<Type> ElementType_;
    std::shared_ptr<Type> DifferenceType_;
    std::shared_ptr<Type> IndexType_;
};

class FunctionType : public Type {
  public:
    FunctionType(std::shared_ptr<Type> ReturnType,
                 std::vector<std::shared_ptr<Type>> ArgTypes,
                 std::string&& name);
    std::shared_ptr<Type> GetReturnType();
    std::vector<std::shared_ptr<Type>> GetArgTypes();

  protected:
    std::shared_ptr<Type> ReturnType_;
    std::vector<std::shared_ptr<Type>> ArgTypes_;
};

using RecordMember = std::pair<std::string, std::shared_ptr<Type>>;

class RecordType : public Type {
  public:
    RecordType(const std::vector<RecordMember>& members, std::string&& name);
    std::pair<int, std::shared_ptr<Type>> GetMember(const std::string& name);

  protected:
    std::map<std::string, std::pair<int, std::shared_ptr<Type>>> members_;
};
} // namespace pcc

#endif // PCC_TYPE_H
