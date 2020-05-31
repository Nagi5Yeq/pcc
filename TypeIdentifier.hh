#ifndef PCC_TYPEIDENTIFIER_HH
#define PCC_TYPEIDENTIFIER_HH

#include <list>

#include "Context.hh"
#include "Type.hh"
#include "VariableList.hh"

namespace pcc {

// since TypeAlias returns std::shared_ptr<Type> instead of llvm::Value*, we
// won't use CodeGen() override.
class TypeIdentifier {
  public:
    TypeIdentifier(Context* context, std::shared_ptr<Type> type = nullptr);
    virtual std::shared_ptr<Type> GetType() = 0;

  protected:
    Context* context_;
    std::shared_ptr<Type> type_;
};

class TrivialTypeIdentifier : public TypeIdentifier {
  public:
    TrivialTypeIdentifier(Context* context, std::shared_ptr<Type> type);
    virtual std::shared_ptr<Type> GetType() override;
};

class AliasTypeIdentifier : public TypeIdentifier {
  public:
    AliasTypeIdentifier(Context* context, std::string&& alias);
    virtual std::shared_ptr<Type> GetType() override;

  protected:
    std::string alias_;
};

// for pointers and arrays, so that we can create pointers to other alias type
class PointerTypeIdentifier : public TypeIdentifier {
  public:
    PointerTypeIdentifier(Context* context,
                          std::shared_ptr<TypeIdentifier> ElementType);
    virtual std::shared_ptr<Type> GetType() override;

  protected:
    std::shared_ptr<TypeIdentifier> ElementType_;
};

class ArrayTypeIdentifier : public TypeIdentifier {
  public:
    ArrayTypeIdentifier(Context* context,
                        std::shared_ptr<TypeIdentifier> ElementType,
                        std::shared_ptr<ExprNode> start,
                        std::shared_ptr<ExprNode> end);
    virtual std::shared_ptr<Type> GetType() override;

  protected:
    std::shared_ptr<TypeIdentifier> ElementType_;
    std::shared_ptr<ExprNode> start_;
    std::shared_ptr<ExprNode> end_;
};

class RecordTypeIdentifier : public TypeIdentifier {
  public:
    RecordTypeIdentifier(Context* context, std::list<Declaration>&& members);
    virtual std::shared_ptr<Type> GetType() override;

  protected:
    std::list<Declaration> members_;
};
} // namespace pcc

#endif // !PCC_TYPEIDENTIFIER_HH
