#ifndef PCC_LEXER_H
#define PCC_LEXER_H

#include <map>

#include "Node.hh"
#include "Parser.hh"

#define YY_DECL                                                                \
    pcc::Parser::symbol_type yylex(pcc::Driver& driver, pcc::Context* context)

YY_DECL;

namespace pcc {
namespace Lexer {
char* GetUnescapedChar(char* s, char* end, char* out) {
    static std::map<char, char> escape_map = {
        {'\'', '\''}, {'?', '\?'}, {'\\', '\\'}, {'a', '\a'}, {'b', '\b'},
        {'f', '\f'},  {'n', '\n'}, {'r', '\r'},  {'t', '\t'}, {'v', '\v'}};
    if (s[0] != '\\') {
        *out = s[0];
        return s + 1;
    } else if (s[1] == 'x') {
        char c[3], *cend;
        c[0] = s[2];
        if (end - s > 3) {
            c[1] = s[3];
            c[2] = '\0';
        } else {
            c[1] = '\0';
        }
        *out = (char) std::strtol(c, &cend, 16);
        return s + 2 + (cend - c);
    } else if (s[1] >= '0' && s[1] <= '7') {
        char c[4], *cend;
        c[0] = s[1];
        if (end - s > 4) {
            c[1] = s[2];
            c[2] = s[3];
            c[3] = '\0';
        } else if (end - s > 3) {
            c[1] = s[2];
            c[2] = '\0';
        } else {
            c[1] = '\0';
        }
        *out = (char) std::strtol(c, &cend, 8);
        return s + 1 + (cend - c);
    } else {
        *out = escape_map[s[1]];
        return s + 2;
    }
}
} // namespace Lexer
} // namespace pcc

#endif // !PCC_LEXER_H
