%{
#include <map>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "Log.hh"
#include "Driver.hh"
#include "Location.hh"

using pcc::Parser;
using pcc::Location;

namespace pcc {
    class Lexer;
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

char            [0-9a-zA-Z_{}[\]#()<>%:;.?*+\-/^&|~!=,\\" \t\n\v\f]
escaped_char    \\[\'?\\abfnrtv]
hex_char        \\x[0-9a-fA-F]{1,2}
oct_char        \\[0-7]{1,3}

string          '({char}|{escaped_char}|{hex_char}|{oct_char}|\\')*'

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
"**"        {return Parser::make_EXP(loc);}

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
"true"      {return Parser::make_TRUE(loc);}
"false"     {return Parser::make_FALSE(loc);}

{integer} {
    int number = std::strtol(yytext, NULL, 0);
    return pcc::Parser::make_INTEGER_LITERAL(number, loc);
}

{real} {
    float number = std::strtof(yytext, NULL);
    return pcc::Parser::make_REAL_LITERAL(number, loc);
}

{string} {
    static std::map<char,char> escape_map={
        {'\'','\''},
        {'?','\?'},
        {'\\','\\'},
        {'a','\a'},
        {'b','\b'},
        {'f','\f'},
        {'n','\n'},
        {'r','\r'},
        {'t','\t'},
        {'v','\v'}
    };
    char *str = new char[yyleng];
    char *p = str;
    int i = 1;
    while(i<yyleng-1){
        if(yytext[i]!='\\'){
            *p++=yytext[i];
            i++;
        }
        else if(yytext[i+1]=='x'){
            char c[3], *cend;
            c[0]=yytext[i+2];
            if(i+3<yyleng-1){
                c[1]=yytext[i+3];
                c[2]='\0';
            } else {
                c[1]='\0';
            }
            *p++=(char)std::strtol(c,&cend,16);
            i+=(int)(cend-c)+2;
        }
        else if(yytext[i+1]>='0' && yytext[i+1]<='7'){
            char c[4], *cend;
            c[0]=yytext[i+1];
            if(i+3<yyleng-1){
                c[1]=yytext[i+2];
                c[2]=yytext[i+3];
                c[3]='\0';
            }
            else if(i+3<yyleng){
                c[1]=yytext[i+2];
                c[2]='\0';
            }
            else {
                c[1]='\0';
            }
            *p++=(char)std::strtol(c,&cend,8);
            i+=(int)(cend-c)+1;
        }
        else {
            *p++=escape_map[yytext[i+1]];
            i+=2;
        }
    }
    *p++='\0';
    return Parser::make_STRING_LITERAL(str, loc);
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
    int Driver::StartLexer(std::string &filename)
    {
        yyin=std::fopen(filename.c_str(),"r");
        if(yyin==NULL){
            Log(PCC_ERROR, "Failed to open file %s", filename.c_str());
            return -1;
        }
        return 0;
    }

    void Driver::StopLexer(){
        std::fclose(yyin);
    }
}
