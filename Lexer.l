%{
#include <map>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "Log.hh"
#include "Driver.hh"
#include "Lexer.hh"

using pcc::Parser;
using pcc::Location;

namespace pcc {
    char ToLower(char c){
        return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
}

%}

%option nodefault
%option never-interactive
%option nounistd
%option case-insensitive
%option noyywrap nounput noinput batch debug

decimal         ([1-9][0-9]*)|0
octal           0[0-7]*
hexadecimal     0[xX][0-9a-fA-F]

integer         [+-]?({decimal}|{octal}|{hexadecimal})

real_exp        [0-9]*(\.[0-9]*)?[eE]{decimal}
real_noexp      [0-9]*\.[0-9]*

real            [+-]?({real_exp}|{real_noexp})

trival_char     [0-9a-zA-Z_{}[\]#()<>%:;.?*+\-/^&|~!=,\\" \t\n\v\f]
escaped_char    \\[\'?\\abfnrtv]
hex_char        \\x[0-9a-fA-F]{1,2}
oct_char        \\[0-7]{1,3}
char_value      {trival_char}|{escaped_char}|{hex_char}|{oct_char}|\\'

char            '{char_value}'
string          '{char_value}+'

identifier      [a-z][0-9a-z]*

white           [\t\f\r ]

%{
#define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
  Location& loc = driver.location_;
  loc.step();
%}

":="        {return Parser::make_ASSIGN(loc);}
":"         {return Parser::make_COLON(loc);}
";"         {return Parser::make_SEMICOLON(loc);}
"."         {return Parser::make_DOT(loc);}
"["         {return Parser::make_LBRACKET(loc);}
"]"         {return Parser::make_RBRACKET(loc);}
"("         {return Parser::make_LPARENTHESIS(loc);}
")"         {return Parser::make_RPARENTHESIS(loc);}

"+"         {return Parser::make_ADD(loc);}
"-"         {return Parser::make_SUB(loc);}
"*"         {return Parser::make_MUL(loc);}
"/"         {return Parser::make_REAL_DIV(loc);}
"div"       {return Parser::make_DIV(loc);}
"mod"       {return Parser::make_MOD(loc);}

"and"       {return Parser::make_AND(loc);}
"not"       {return Parser::make_NOT(loc);}
"or"        {return Parser::make_OR(loc);}
"xor"       {return Parser::make_XOR(loc);}

"<"         {return Parser::make_LT(loc);}
"<="        {return Parser::make_LE(loc);}
">"         {return Parser::make_GT(loc);}
">="        {return Parser::make_GE(loc);}
"="         {return Parser::make_EQ(loc);}
"<>"        {return Parser::make_NE(loc);}

"begin"     {return Parser::make_BEGINS(loc);}
"end"       {return Parser::make_ENDS(loc);}
"program"   {return Parser::make_PROGRAM(loc);}
"function"  {return Parser::make_FUNCTION(loc);}
"const"     {return Parser::make_CONST(loc);}
"var"       {return Parser::make_VAR(loc);}
"if"        {return Parser::make_IF(loc);}
"then"      {return Parser::make_THEN(loc);}
"else"      {return Parser::make_ELSE(loc);}
"while"     {return Parser::make_WHILE(loc);}
"do"        {return Parser::make_DO(loc);}
"integer"   {return Parser::make_INTEGER(loc);}
"boolean"   {return Parser::make_BOOLEAN(loc);}
"char"      {return Parser::make_CHAR(loc);}
"real"      {return Parser::make_REAL(loc);}
"string"    {return Parser::make_STRING(loc);}

"true"      {return pcc::Lexer::make_BOOLEAN_LITERAL(true, context, loc);}
"false"     {return pcc::Lexer::make_BOOLEAN_LITERAL(false, context, loc);}

{integer} {
    int number = std::strtol(yytext, NULL, 0);
    return pcc::Lexer::make_INTEGER_LITERAL(number, context, loc);
}

{real} {
    float number = std::strtof(yytext, NULL);
    return pcc::Lexer::make_REAL_LITERAL(number, context, loc);
}

{char} {
    char *s, *end, c;
    s = yytext + 1;
    end = s + yyleng - 2;
    pcc::Lexer::GetUnescapedChar(s, end, &c);
    return pcc::Lexer::make_CHAR_LITERAL(c, context, loc);
}

{string} {
    char *s, *end, *p;
    s = p = yytext + 1;
    end = s + yyleng - 2;
    while (s < end) {
        s = pcc::Lexer::GetUnescapedChar(s, end, p);
        p++;
    }
    *p++ = '\0';
    return pcc::Lexer::make_STRING_LITERAL(yytext + 1, p, context, loc);
}

{identifier} {
    char *name = new char[yyleng];
    std::transform(yytext, yytext+yyleng, name, pcc::ToLower);
    return Parser::make_IDENTIFIER(yytext, loc);
}

{white}+ {loc.step();}
\n+      {loc.lines(yyleng); loc.step();}

. {
    std::cerr<<"ERROR";
}

<<EOF>>  {return Parser::make_FILE_END(loc);}

%%

namespace pcc {
int Driver::StartLexer(std::string& filename) {
    yyin = std::fopen(filename.c_str(), "r");
    if (yyin == NULL) {
        Log(PCC_ERROR, "Failed to open file %s", filename.c_str());
        return -1;
    }
    return 0;
}

void Driver::StopLexer() { std::fclose(yyin); }
} // namespace pcc
