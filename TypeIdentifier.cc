#include "TypeIdentifier.hh"
#include "TypeManager.hh"
#include "VariableList.hh"

namespace pcc {

TypeIdentifier::TypeIdentifier(Context* context, std::shared_ptr<Type> type)
    : type_(type)
    , context_(context) {}

TrivialTypeIdentifier::TrivialTypeIdentifier(Context* context,
                                             std::shared_ptr<Type> type)
    : TypeIdentifier(context, type) {}

std::shared_ptr<Type> TrivialTypeIdentifier::GetType() { return type_; }

AliasTypeIdentifier::AliasTypeIdentifier(Context* context, std::string&& alias)
    : TypeIdentifier(context)
    , alias_(alias) {}

std::shared_ptr<Type> AliasTypeIdentifier::GetType() {
    // cache the result, save some time in a "var a,b,c:somealias;" case
    if (type_ == nullptr) {
        type_ = context_->GetCurrentScope()->FindTypeAlias(alias_);
    }
    return type_;
}

PointerTypeIdentifier::PointerTypeIdentifier(
    Context* context, std::shared_ptr<TypeIdentifier> ElementType)
    : TypeIdentifier(context)
    , ElementType_(ElementType) {}

std::shared_ptr<Type> PointerTypeIdentifier::GetType() {
    if (type_ == nullptr) {
        type_ =
            context_->GetTypeManager()->GetPointerType(ElementType_->GetType());
    }
    return type_;
}

ArrayTypeIdentifier::ArrayTypeIdentifier(
    Context* context, std::shared_ptr<TypeIdentifier> ElementType,
    std::shared_ptr<ExprNode> start, std::shared_ptr<ExprNode> end)
    : TypeIdentifier(context)
    , ElementType_(ElementType)
    , start_(start)
    , end_(end) {}

std::shared_ptr<Type> ArrayTypeIdentifier::GetType() {
    if (type_ == nullptr) {
        type_ = context_->GetTypeManager()->CreateArrayType(
            ElementType_->GetType(), start_, end_);
    }
    return type_;
}

RecordTypeIdentifier::RecordTypeIdentifier(Context* context,
                                   std::list<Declaration>&& members)
    : TypeIdentifier(context)
    , members_(std::move(members)) {}

std::shared_ptr<Type> RecordTypeIdentifier::GetType() {
    if (type_ == nullptr) {
        std::vector<RecordMember> MemberTypes(members_.size());
        std::transform(members_.cbegin(), members_.cend(), MemberTypes.begin(),
                       [](const Declaration& member) {
                           return std::make_pair(
                               std::get<0>(member),
                               std::get<1>(member)->GetType());
                       });
        type_ = context_->GetTypeManager()->CreateRecordType(MemberTypes);
    }
    return type_;
}
} // namespace pcc
