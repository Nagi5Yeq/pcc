#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include "Node.hh"
#include "Parser.hh"

#define YY_DECL pcc::Parser::symbol_type yylex(pcc::Driver& driver)

YY_DECL;

namespace pcc {
namespace Lexer {
Parser::symbol_type make_STRING_LITERAL(char* s, pcc::Driver& driver,
                                        const Parser::location_type& loc) {
    return Parser::make_STRING_LITERAL(
        std::make_shared<StringLiteralNode>(&driver, s), loc);
}
} // namespace Lexer
} // namespace pcc

#endif // !PCC_LEXER_H
