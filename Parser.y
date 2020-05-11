%language "c++"
%require "3.2"

%code requires {
#include <string>

namespace pcc {
    class Driver;
}
}

%param {pcc::Driver& driver}

%code {
#include "Driver.hh"
}

%define api.namespace {pcc}
%define api.parser.class {Parser}
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose
%define parse.assert

%token COLON SEMICOLON COMMA DOT ASSIGN LBRACKET RBRACKET LPARENTHESIS RPARENTHESIS
%token PROGRAM IDENTIFIER CONST VAR BEGINS ENDS FUNCTION
%token IF THEN ELSE WHILE DO
%token INTEGER BOOLEAN CHAR REAL STRING
%token ADD SUB MUL REAL_DIV DIV MOD EXP LT LE GT GE EQ NE
%token AND NOT OR XOR TRUE FALSE
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL

%type <int> INTEGER_LITERAL
%type <float> REAL_LITERAL
%type <char*> STRING_LITERAL
%type <char*> IDENTIFIER

%%

/* ==================[global part]================== */

program
    : program_decl global_decls function_decls
    ;

program_decl
    : PROGRAM IDENTIFIER SEMICOLON
    ;

global_decls
    : var_block
    ;

function_decls
    : function_decls function_decl
    |
    ;

/* ==================[var part]================== */

var_block
    : VAR var_decls SEMICOLON
    ;

var_decls
    : var_decls SEMICOLON var_decl
    | var_decl
    ;

var_decl
    : vars COLON var_type
    ;

vars
    : vars COMMA IDENTIFIER
    | IDENTIFIER

var_type
    : INTEGER
    | REAL
    | BOOLEAN
    | CHAR
    | STRING
    ;

/* ==================[function part]================== */

function_decl
    : function_header function_body
    ;

function_header
    : FUNCTION IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON var_type SEMICOLON
    ;

function_body
    : var_block statement_block SEMICOLON
    | statement_block SEMICOLON
    ;

/* ==================[statements part]================== */

statements
    : statements statement SEMICOLON
    |
    ;

statement
    : closed_statement
    | open_statement
    ;

open_statement
    : IF expression THEN statement
    | IF expression THEN closed_statement ELSE open_statement
    ;

closed_statement
    : normal_statement
    | IF expression THEN closed_statement ELSE closed_statement
    ;

normal_statement
    : statement_block
    | empty_statement
    | assign_statement
    | while_statement
    | expression_statement
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

while_statement
    : WHILE expression DO statement
    ;

expression_statement
    : function_call
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

    void Parser::error (const std::string& m)
    {
        std::cerr << m << '\n';
    }

}
