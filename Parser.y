%language "c++"
%require "3.2"

%code requires {
#include <string>
#include <memory>

#include "Location.hh"
#include "Node.hh"
#include "Context.hh"

namespace pcc{
    class Driver;
}
}

%locations

%param {pcc::Driver& driver}
%param {pcc::Context* ctx}

%code{
#define YY_DECL                                                                \
    pcc::Parser::symbol_type yylex(pcc::Driver& driver, pcc::Context* ctx)

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
%token BOOLEAN CHAR INTEGER REAL STRING
%token ADD SUB MUL REAL_DIV DIV MOD LT LE GT GE EQ NE
%token AND NOT OR XOR
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL BOOLEAN_LITERAL CHAR_LITERAL

%type <std::string> IDENTIFIER
%type <std::shared_ptr<pcc::Type>> type
%type <std::list<Declaration>> var_decls
%type <std::pair<std::list<std::string>, std::shared_ptr<pcc::Type>>> var_decl
%type <std::list<std::string>> vars

%type <std::shared_ptr<pcc::BooleanLiteralNode>> BOOLEAN_LITERAL
%type <std::shared_ptr<pcc::CharLiteralNode>> CHAR_LITERAL
%type <std::shared_ptr<pcc::IntegerLiteralNode>> INTEGER_LITERAL
%type <std::shared_ptr<pcc::RealLiteralNode>> REAL_LITERAL
%type <std::shared_ptr<pcc::StringLiteralNode>> STRING_LITERAL
%type <std::shared_ptr<pcc::ExprNode>> literal 

%type <std::shared_ptr<pcc::ExprNode>> expression l1_expression l2_expression l3_expression
%type <pcc::BinaryOperator> l1_operator l2_operator l3_operator
%type <pcc::UnaryOperator> l4_operator

%type <std::shared_ptr<pcc::StatementListNode>> statements statement_block

%type <std::shared_ptr<pcc::FunctionNode>> function
%type <std::list<std::shared_ptr<pcc::BaseNode>>> local_decls
%type <std::shared_ptr<pcc::BaseNode>> local_decl

%%

/* ==================[global part]================== */

program
    : program_header global_decls functions
    ;

program_header
    : PROGRAM IDENTIFIER SEMICOLON
    ;

global_decls
    : global_decls global_decl SEMICOLON
    |
    ;

global_decl
    : VAR var_decls
    ;

functions
    : functions function SEMICOLON
    |
    ;

/* ==================[var part]================== */

var_decls
    : var_decls SEMICOLON var_decl  {$$=$1; auto NewChilds=std::get<0>($3); for(auto& name: NewChilds){$$.push_back({name, std::get<1>($3)});}}
    | var_decl                      {auto NewChilds=std::get<0>($1); for(auto& name: NewChilds){$$.push_back({name, std::get<1>($1)});}}
    ;

var_decl
    : vars COLON type       {$$={$1,$3};}
    ;

vars
    : vars COMMA IDENTIFIER {$$=$1; $$.push_back($3);}
    | IDENTIFIER            {$$.push_back($1);}
    ;

type
    : BOOLEAN   {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::BOOLEAN);}
    | CHAR      {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::CHAR);}
    | INTEGER   {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::INTEGER);}
    | REAL      {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::REAL);}
    | STRING    {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::STRING);}
    ;

/* ==================[function part]================== */

function
    : FUNCTION IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON type SEMICOLON
      local_decls statement_block
    ;

local_decls 
    : local_decls local_decl    {$$=$1; $$.push_back($2);}
    |
    ;

local_decl
    : VAR var_decls {$$=std::make_shared<pcc::VarDeclNode>(ctx, nullptr, $2);}
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
    : l1_expression l1_operator expression      {$$=std::make_shared<pcc::BinaryExprNode>(ctx,$2,$1,$3);}
    | l1_expression                             {$$=$1;}
    ;

l1_expression
    : l1_expression l2_operator l2_expression   {$$=std::make_shared<pcc::BinaryExprNode>(ctx,$2,$1,$3);}
    | l2_expression                             {$$=$1;}
    ;

l2_expression
    : l2_expression l3_operator l3_expression   {$$=std::make_shared<pcc::BinaryExprNode>(ctx,$2,$1,$3);}
    | l3_expression                             {$$=$1;}
    ;

l3_expression
    : lvalue
    | function_call
    | literal                               {$$=$1;}
    | LPARENTHESIS expression RPARENTHESIS  {$$=$2;}
    | l4_operator l3_expression             {$$=std::make_shared<pcc::UnaryExprNode>(ctx,$1,$2);}
    ;

l1_operator
    : LT        {$$=pcc::BinaryOperator::LT;}
    | LE        {$$=pcc::BinaryOperator::LE;}
    | GT        {$$=pcc::BinaryOperator::GT;}
    | GE        {$$=pcc::BinaryOperator::GE;}
    | EQ        {$$=pcc::BinaryOperator::EQ;}
    | NE        {$$=pcc::BinaryOperator::NE;}
    ;

l2_operator
    : ADD       {$$=pcc::BinaryOperator::ADD;}
    | SUB       {$$=pcc::BinaryOperator::SUB;}
    | OR        {$$=pcc::BinaryOperator::OR;}
    | XOR       {$$=pcc::BinaryOperator::XOR;}
    ;

l3_operator
    : MUL       {$$=pcc::BinaryOperator::MUL;}
    | DIV       {$$=pcc::BinaryOperator::DIV;}
    | MOD       {$$=pcc::BinaryOperator::MOD;}
    | AND       {$$=pcc::BinaryOperator::AND;}
    ;

l4_operator
    : NOT   {$$=pcc::UnaryOperator::NOT;}
    | ADD   {$$=pcc::UnaryOperator::POS;}
    | SUB   {$$=pcc::UnaryOperator::NEG;}
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
    : BOOLEAN_LITERAL   {$$=$1;}
    | INTEGER_LITERAL   {$$=$1;}
    | REAL_LITERAL      {$$=$1;}
    | STRING_LITERAL    {$$=$1;}
    ;

%%

namespace pcc {

    void Parser::error (const location_type& l, const std::string& m)
    {
        std::cerr << l << ":" << m << '\n';
    }

}
