%language "c++"
%require "3.2"

%code requires {
#include <string>
#include <memory>

#include "Location.hh"

namespace pcc {
    class Driver;
    class StringLiteralNode;
}
}

%locations

%param {pcc::Driver& driver}

%code {
#include "Driver.hh"
#include "Node.hh"

#define YY_DECL pcc::Parser::symbol_type yylex(pcc::Driver& driver)
YY_DECL;
}

%define api.namespace {pcc}
%define api.parser.class {Parser}
%define api.location.type {pcc::Location}
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose
%define parse.assert

%token COLON SEMICOLON COMMA DOT ASSIGN LBRACKET RBRACKET LPARENTHESIS RPARENTHESIS
%token PROGRAM IDENTIFIER CONST VAR BEGINS ENDS FUNCTION FILE_END
%token IF THEN ELSE WHILE DO
%token INTEGER BOOLEAN CHAR REAL STRING
%token ADD SUB MUL REAL_DIV DIV MOD EXP LT LE GT GE EQ NE
%token AND NOT OR XOR TRUE FALSE
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL

%type <int> INTEGER_LITERAL
%type <float> REAL_LITERAL
%type <char*> IDENTIFIER

%type <std::shared_ptr<pcc::StringLiteralNode>> STRING_LITERAL

%%

/* ==================[global part]================== */

program
    : program_header decls functions
    ;

program_header
    : PROGRAM IDENTIFIER SEMICOLON
    ;

decls 
    : decls decl SEMICOLON
    |
    ;

decl
    : VAR var_decls SEMICOLON
    ;

functions
    : functions function SEMICOLON
    |
    ;

/* ==================[var part]================== */

var_decls
    : var_decls SEMICOLON var_decl
    | var_decl
    ;

var_decl
    : vars COLON type
    ;

vars
    : vars COMMA IDENTIFIER
    | IDENTIFIER

type
    : INTEGER
    | REAL
    | BOOLEAN
    | CHAR
    | STRING
    ;

/* ==================[function part]================== */

function
    : function_header decls statement_block
    ;

function_header
    : FUNCTION IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON type SEMICOLON
    ;

/* ==================[statements part]================== */

statements
    : statements SEMICOLON statement
    | statement
    ;

statement
    : open_statement
    | closed_statement
    ;

open_statement
    : IF expression THEN statement
    | IF expression THEN closed_statement ELSE open_statement
    | WHILE expression DO open_statement
    ;

closed_statement
    : normal_statement
    | IF expression THEN closed_statement ELSE closed_statement
    | WHILE expression DO closed_statement
    ;

normal_statement
    : statement_block
    | empty_statement
    | assign_statement
    | expression
    ;

statement_block
    : BEGINS statements ENDS
    ;

empty_statement
    :
    ;

assign_statement
    : lvalue ASSIGN expression
    ;

/* ==================[expression part]================== */

expression
    : l1_expression l1_operator expression
    | l1_expression
    ;

l1_expression
    : l1_expression l2_operator l2_expression
    | l2_expression
    ;

l2_expression
    : l2_expression l3_operator l3_expression
    | l3_expression
    ;

l3_expression
    : l3_expression l4_operator l4_expression
    | l4_expression

l4_expression
    : lvalue
    | function_call
    | literal
    | LPARENTHESIS expression RPARENTHESIS
    | l5_operator l4_expression
    ;

l1_operator
    : LT
    | LE
    | GT
    | GE
    | EQ
    | NE
    ;

l2_operator
    : ADD
    | SUB
    | OR
    | XOR
    ;

l3_operator
    : MUL
    | REAL_DIV
    | DIV
    | MOD
    | AND
    ;

l4_operator
    : EXP
    ;

l5_operator
    : NOT
    | ADD
    | SUB
    ;

lvalue
    : IDENTIFIER
    | IDENTIFIER LBRACKET expression RBRACKET
    | IDENTIFIER DOT IDENTIFIER
    ;

function_call
    : IDENTIFIER LPARENTHESIS arguments RPARENTHESIS
    | IDENTIFIER LPARENTHESIS RPARENTHESIS
    ;

arguments
    : arguments COMMA expression
    | expression
    ;

literal
    : TRUE
    | FALSE
    | INTEGER_LITERAL
    | REAL_LITERAL
    | STRING_LITERAL
    ;

%%

namespace pcc {

    void Parser::error (const location_type& l, const std::string& m)
    {
        std::cerr << l << ":" << m << '\n';
    }

}
