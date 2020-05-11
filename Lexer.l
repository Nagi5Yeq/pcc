%{
#include <map>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <algorithm>

#include "Log.hh"
#include "Driver.hh"

using token=pcc::Parser::token;

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
%option noyywrap nounput noinput

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

white           [\t\f\n\r ]

%%

":="        {return token::ASSIGN;}
":"         {return token::COLON;}
";"         {return token::SEMICOLON;}
"."         {return token::DOT;}
"["         {return token::LBRACKET;}
"]"         {return token::RBRACKET;}
"("         {return token::LPARENTHESIS;}
")"         {return token::RPARENTHESIS;}

"+"         {return token::ADD;}
"-"         {return token::SUB;}
"*"         {return token::MUL;}
"/"         {return token::REAL_DIV;}
"div"       {return token::DIV;}
"mod"       {return token::MOD;}
"**"        {return token::EXP;}

"and"       {return token::AND;}
"not"       {return token::NOT;}
"or"        {return token::OR;}
"xor"       {return token::XOR;}

"<"         {return token::LT;}
"<="        {return token::LE;}
">"         {return token::GT;}
">="        {return token::GE;}
"="         {return token::EQ;}
"<>"        {return token::NE;}

"begin"     {return token::BEGINS;}
"end"       {return token::ENDS;}
"program"   {return token::PROGRAM;}
"function"  {return token::FUNCTION;}
"const"     {return token::CONST;}
"var"       {return token::VAR;}
"if"        {return token::IF;}
"then"      {return token::THEN;}
"else"      {return token::ELSE;}
"while"     {return token::WHILE;}
"do"        {return token::DO;}
"integer"   {return token::INTEGER;}
"boolean"   {return token::BOOLEAN;}
"char"      {return token::CHAR;}
"real"      {return token::REAL;}
"string"    {return token::STRING;}
"true"      {return token::TRUE;}
"false"     {return token::FALSE;}

{integer} {
    int number = std::strtol(yytext, NULL, 0);
    return pcc::Parser::make_INTEGER_LITERAL(number);
}

{real} {
    float number = std::strtof(yytext, NULL);
    return pcc::Parser::make_REAL_LITERAL(number);
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
    return pcc::Parser::make_STRING_LITERAL(str);
}

{identifier} {
    char *name = new char[yyleng];
    std::transform(yytext, yytext+yyleng, name, pcc::ToLower);
    return pcc::Parser::make_IDENTIFIER(yytext);
}

{white} {}

. {
    std::cerr<<"ERROR";
}

%%

namespace pcc {
    int Driver::StartLexer(const std::string &filename)
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
