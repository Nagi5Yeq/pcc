#include <cctype>
#include <type_traits>

#include "Node.hh"
#include "Type.hh"
#include "TypeIdentifier.hh"

namespace pcc {
Traveler& Traveler::operator<<(std::shared_ptr<Type> type) {
    out_ << type->GetCommonName();
    return *this;
}

Traveler& Traveler::operator<<(std::shared_ptr<TypeIdentifier> identifier) {
    return *this << identifier->GetType();
}

template <typename Lambda, typename Container>
void NonterminalContainerTravel(Traveler& traveler, Lambda&& lambda,
                                Container&& childs) {
    for (auto&& child : childs) {
        traveler << TravelPart::CHILD_BEGIN;
        std::forward<Lambda>(lambda)(traveler, child);
        traveler << TravelPart::CHILD_END;
    }
}

template <typename Lambda, typename Container>
void TerminalContainerTravel(Traveler& traveler, Lambda&& lambda,
                             Container&& childs) {
    if (childs.empty()) {
        return;
    }
    auto LastOne = childs.end();
    --LastOne;
    for (auto child = childs.begin(); child != LastOne; ++child) {
        traveler << TravelPart::CHILD_BEGIN;
        std::forward<Lambda>(lambda)(traveler, *child);
        traveler << TravelPart::CHILD_END;
    }
    traveler << TravelPart::LAST_CHILD_BEGIN;
    std::forward<Lambda>(lambda)(traveler, *LastOne);
    traveler << TravelPart::LAST_CHILD_END;
}

template <typename L0, typename C0, typename L1, typename C1>
void MultipleContainerTravel(Traveler& traveler, L0&& ThisLambda, C0&& ThisOne,
                             L1&& LastLambda, C1&& LastOne) {
    if (LastOne.empty()) {
        TerminalContainerTravel(traveler, std::forward<L0>(ThisLambda),
                                std::forward<C0>(ThisOne));
        return;
    }
    NonterminalContainerTravel(traveler, std::forward<L0>(ThisLambda),
                               std::forward<C0>(ThisOne));
    TerminalContainerTravel(traveler, std::forward<L1>(LastLambda),
                            std::forward<C1>(LastOne));
}

int ProgramNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ProgramNode" << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "program " + name_
             << TravelPart::DESCRPTION_END;
    MultipleContainerTravel(
        traveler,
        [](Traveler& traveler, auto&& decl) { decl->Travel(traveler); },
        GlobalDeclarations_,
        [](Traveler& traveler, auto&& decl) { decl->Travel(traveler); },
        functions_);
    traveler << TravelPart::END;
    return 0;
}

int VarDeclNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "VarDeclNode" << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN
             << (scope_ == nullptr ? "global size=" : "local size=")
             << decls_.size() << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler,
        [](Traveler& traveler, Declaration& decl) {
            traveler << TravelPart::PREFIX;
            traveler << TravelPart::NAME_BEGIN << "Variable"
                     << TravelPart::NAME_END;
            traveler << TravelPart::DESCRPTION_BEGIN
                     << "name=" << std::get<0>(decl)
                     << " type=" << (std::get<1>(decl)->GetType())
                     << TravelPart::DESCRPTION_END;
            traveler << TravelPart::END;
        },
        decls_);
    traveler << TravelPart::END;
    return 0;
}

int ConstDeclNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ConstDeclNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "const size=" << decls_.size()
             << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler,
        [](Traveler& traveler, Constant& decl) {
            traveler << TravelPart::PREFIX;
            traveler << TravelPart::NAME_BEGIN << "Constant"
                     << TravelPart::NAME_END;
            traveler << TravelPart::DESCRPTION_BEGIN
                     << "name=" << std::get<0>(decl)
                     << " type=" << (std::get<1>(decl)->GetType())
                     << TravelPart::DESCRPTION_END;
            traveler << TravelPart::END;
        },
        decls_);
    traveler << TravelPart::END;
    return 0;
}

int TypeDeclNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "TypeDeclNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN
             << (scope_ == nullptr ? "global size=" : "local size=")
             << decls_.size() << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler,
        [](Traveler& traveler, Declaration& decl) {
            traveler << TravelPart::PREFIX;
            traveler << TravelPart::NAME_BEGIN << "TypeAlias"
                     << TravelPart::NAME_END;
            traveler << TravelPart::DESCRPTION_BEGIN
                     << "name=" << std::get<0>(decl)
                     << " type=" << (std::get<1>(decl)->GetType())
                     << TravelPart::DESCRPTION_END;
            traveler << TravelPart::END;
        },
        decls_);
    traveler << TravelPart::END;
    return 0;
}

int BinaryExprNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "BinaryExprNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "op='" << GetOperatorName(op_)
             << "' ResultType=" << type_ << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    left_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    right_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int UnaryExprNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "UnaryExprNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "op='" << GetOperatorName(op_)
             << " ResultType='" << type_ << TravelPart::DESCRPTION_END;
    traveler << TravelPart::LAST_CHILD_BEGIN;
    value_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int L2RCastingNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "L2RCastingNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "ResultType=" << type_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::LAST_CHILD_BEGIN;
    lvalue_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int PointerAccessNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "PointerAccessNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "ResultType=" << type_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    lhs_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    rhs_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    traveler << TravelPart::END;
    return 0;
}

int ArrayAccessNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ArrayAccessNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "ResultType=" << type_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    lhs_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    rhs_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    traveler << TravelPart::END;
    return 0;
}

int DereferenceNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "DereferenceNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "ResultType=" << type_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::LAST_CHILD_BEGIN;
    lhs_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int MemberAccessNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "MemberAccessNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "MemberName=" << rhs_
             << " ResultType=" << type_ << TravelPart::DESCRPTION_END;
    traveler << TravelPart::LAST_CHILD_BEGIN;
    lhs_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int IdentifierNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "IdentifierNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "lvalue name=" << name_
             << " type=" << type_ << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

int FunctionCallNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "FunctionCallNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "FunctionName=" << name_
             << " ResultType=" << type_ << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler, [](Traveler& traveler, auto&& arg) { arg->Travel(traveler); },
        args_);
    traveler << TravelPart::END;
    return 0;
}

int FunctionNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "FunctionNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "FunctionName=" << name_
             << " ReturnType=" << ReturnTypeIdentifier_
             << TravelPart::DESCRPTION_END;
    NonterminalContainerTravel(
        traveler,
        [](Traveler& traveler, auto& arg) {
            traveler << TravelPart::PREFIX;
            traveler << TravelPart::NAME_BEGIN << "Argument"
                     << TravelPart::NAME_END;
            traveler << TravelPart::DESCRPTION_BEGIN
                     << "name=" << std::get<0>(arg)
                     << " type=" << std::get<1>(arg)
                     << TravelPart::DESCRPTION_END;
            traveler << TravelPart::END;
        },
        arguments_);
    NonterminalContainerTravel(
        traveler,
        [](Traveler& traveler, auto&& decl) { decl->Travel(traveler); },
        LocalDeclarations_);
    traveler << TravelPart::LAST_CHILD_BEGIN;
    body_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END;
    traveler << TravelPart::END;
    return 0;
}

int ExternNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ExternNode" << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "FunctionName=" << name_
             << " ReturnType=" << ReturnTypeIdentifier_
             << (IsVariadic_ ? "Variadic" : "") << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler,
        [](Traveler& traveler, auto& arg) {
            traveler << TravelPart::PREFIX;
            traveler << TravelPart::NAME_BEGIN << "Argument"
                     << TravelPart::NAME_END;
            traveler << TravelPart::DESCRPTION_BEGIN
                     << "name=" << std::get<0>(arg)
                     << " type=" << std::get<1>(arg)
                     << TravelPart::DESCRPTION_END;
            traveler << TravelPart::END;
        },
        arguments_);
    traveler << TravelPart::END;
    return 0;
}

template <>
int BooleanLiteralNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "BooleanLiteralNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN
             << "value=" << (value_ == true ? "true" : "false")
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

template <>
int CharLiteralNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "CharLiteralNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "value=0x" << std::hex
             << (int) value_ << std::dec << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

template <>
int IntegerLiteralNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "IntegerLiteralNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "value=" << value_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

template <>
int RealLiteralNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "RealLiteralNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "value=" << value_
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

int StringLiteralNode::Travel(Traveler& traveler) {
    const char HexChars[] = "0123456789abcdef";
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "StringLiteralNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "value=\'";
    std::string escaped;
    escaped.reserve(value_.size() * 2);
    for (char c : value_) {
        if (std::isprint(c)) {
            escaped.push_back(c);
        } else {
            auto hex = {'\\', 'x', HexChars[c & 0xf], HexChars[(c >> 4) & 0xf]};
            escaped.append(hex);
        }
    }
    traveler << escaped << '\'' << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

int StatementListNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "StatementListNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "size=" << childs_.size()
             << TravelPart::DESCRPTION_END;
    TerminalContainerTravel(
        traveler,
        [](Traveler& traveler, auto&& child) { child->Travel(traveler); },
        childs_);
    traveler << TravelPart::END;
    return 0;
}

int EmptyStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "EmptyStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "empty"
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

int AssignStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "AssignStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "ValueType=" << rhs_->GetType()
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    lhs_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    rhs_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int IfStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "IfStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN
             << "HasElse=" << (ElsePart_ == nullptr ? "false" : "true")
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    expr_->Travel(traveler);
    traveler << TravelPart::CHILD_END;
    if (ElsePart_ == nullptr) {
        traveler << TravelPart::LAST_CHILD_BEGIN;
        ThenPart_->Travel(traveler);
        traveler << TravelPart::LAST_CHILD_END;
    } else {
        traveler << TravelPart::CHILD_BEGIN;
        ThenPart_->Travel(traveler);
        traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
        ElsePart_->Travel(traveler);
        traveler << TravelPart::LAST_CHILD_END;
    }
    traveler << TravelPart::END;
    return 0;
}

int WhileStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "WhileStatementNode"
             << TravelPart::NAME_END << TravelPart::DESCRPTION_BEGIN
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    expr_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    WhileBody_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int RepeatStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "RepeatStatementNode"
             << TravelPart::NAME_END << TravelPart::DESCRPTION_BEGIN
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::CHILD_BEGIN;
    expr_->Travel(traveler);
    traveler << TravelPart::CHILD_END << TravelPart::LAST_CHILD_BEGIN;
    RepeatBody_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int ForStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ForStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN
             << "direction=" << (IsUpward_ ? "up" : "down")
             << TravelPart::DESCRPTION_END;
    auto childs = {variable_, start_, end_};
    NonterminalContainerTravel(
        traveler,
        [](Traveler& traveler, auto&& child) { child->Travel(traveler); },
        childs);
    traveler << TravelPart::LAST_CHILD_BEGIN;
    ForBody_->Travel(traveler);
    traveler << TravelPart::LAST_CHILD_END << TravelPart::END;
    return 0;
}

int BreakStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "BreakStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "break"
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}

int ContinueStatementNode::Travel(Traveler& traveler) {
    traveler << TravelPart::PREFIX;
    traveler << TravelPart::NAME_BEGIN << "ContinueStatementNode"
             << TravelPart::NAME_END;
    traveler << TravelPart::DESCRPTION_BEGIN << "continue"
             << TravelPart::DESCRPTION_END;
    traveler << TravelPart::END;
    return 0;
}
} // namespace pcc
