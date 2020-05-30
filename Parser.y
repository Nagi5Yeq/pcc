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
#include <sstream>

#include "Driver.hh"
#include "Log.hh"

#define YY_DECL                                                                \
    pcc::Parser::symbol_type yylex(pcc::Driver& driver, pcc::Context* ctx)

YY_DECL;
}

%define api.namespace {pcc}
%define api.parser.class {Parser}
%define api.location.type {pcc::Location}
%define api.value.type variant
%define api.token.constructor
%define parse.trace
%define parse.error verbose
%define parse.assert

%token FILE_END 0;
%token COLON SEMICOLON COMMA DOTDOT DOT ASSIGN LBRACKET RBRACKET LPARENTHESIS RPARENTHESIS
%token PROGRAM IDENTIFIER CONST VAR BEGINS ENDS FUNCTION
%token IF THEN ELSE WHILE DO REPEAT UNTIL BREAK CONTINUE
%token VOID BOOLEAN CHAR INTEGER REAL STRING ARRAY OF
%token ADD SUB MUL REAL_DIV DIV MOD LT LE GT GE EQ NE CARET AT
%token AND NOT OR XOR SHL SHR
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL BOOLEAN_LITERAL CHAR_LITERAL

%type <std::string> IDENTIFIER program_header
%type <std::shared_ptr<pcc::Type>> type array_type
%type <std::list<Declaration>> var_decls
%type <std::pair<std::list<std::string>, std::shared_ptr<pcc::Type>>> var_decl
%type <std::list<std::string>> vars
%type <std::list<pcc::Constant>> const_decls
%type <pcc::Constant> const_decl

%type <bool> BOOLEAN_LITERAL
%type <char> CHAR_LITERAL
%type <int> INTEGER_LITERAL
%type <float> REAL_LITERAL
%type <std::vector<char>> STRING_LITERAL
%type <std::shared_ptr<pcc::ExprNode>> literal lvalue
%type <std::list<std::shared_ptr<pcc::ExprNode>>> arguments
%type <std::shared_ptr<pcc::FunctionCallNode>> function_call
%type <std::shared_ptr<pcc::ExprNode>> expression l1_expression l2_expression l3_expression standalone_expression
%type <pcc::BinaryOperator> l1_operator l2_operator l3_operator
%type <pcc::UnaryOperator> l4_operator

%type <std::list<std::shared_ptr<pcc::FunctionNode>>> functions
%type <std::shared_ptr<pcc::FunctionNode>> function
%type <std::list<std::shared_ptr<pcc::DeclNode>>> decls
%type <std::shared_ptr<pcc::DeclNode>> decl
%type <std::shared_ptr<pcc::VarDeclNode>> var_decl_statement
%type <std::shared_ptr<pcc::ConstDeclNode>> const_decl_statement

%type <std::shared_ptr<pcc::StatementListNode>> statements statement_block
%type <std::shared_ptr<pcc::BaseNode>> statement open_statement closed_statement normal_statement
%type <std::shared_ptr<pcc::AssignStatementNode>> assign_statement
%type <std::shared_ptr<pcc::RepeatStatementNode>> repeat_statement

%%

/* ==================[global part]================== */

program
    : program_header decls functions FILE_END {driver.SetRoot(std::make_shared<pcc::ProgramNode>(ctx, std::move($1), std::move($2), std::move($3)));}
    ;

program_header
    : PROGRAM IDENTIFIER SEMICOLON  {$$=std::move($2);}
    ;

decls
    : decls decl    {$$=std::move($1); $$.push_back($2);}
    |               {$$=std::list<std::shared_ptr<pcc::DeclNode>>();}
    ;

decl
    : var_decl_statement    {$$=$1;}
    | const_decl_statement  {$$=$1;}
    ;

functions
    : functions function SEMICOLON  {$$=std::move($1); $$.push_back($2);}
    |                               {$$=std::list<std::shared_ptr<pcc::FunctionNode>>();}
    ;

/* ==================[var part]================== */

var_decl_statement
    : VAR var_decls SEMICOLON       {$$=std::make_shared<pcc::VarDeclNode>(ctx, nullptr, std::move($2));}
    ;

var_decls
    : var_decls SEMICOLON var_decl  {$$=std::move($1); auto NewChilds=std::get<0>($3); for(auto& name: NewChilds){$$.push_back({name, std::get<1>($3)});}}
    | var_decl                      {auto NewChilds=std::get<0>($1); for(auto& name: NewChilds){$$.push_back({name, std::get<1>($1)});}}
    ;

var_decl
    : vars COLON type               {$$={$1, $3};}
    ;

vars
    : vars COMMA IDENTIFIER         {$$=std::move($1); $$.push_back($3);}
    | IDENTIFIER                    {$$.push_back($1);}
    ;

const_decl_statement
    :   CONST const_decls SEMICOLON     {$$=std::make_shared<pcc::ConstDeclNode>(ctx, nullptr, std::move($2));}
    ;

const_decls
    : const_decls SEMICOLON const_decl  {$$=std::move($1); $$.push_back($3);}
    | const_decl                        {$$.push_back($1);}
    ;

const_decl
    : IDENTIFIER EQ literal             {$$={$1, $3};}
    ;

type
    : VOID          {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::VOID);}
    | BOOLEAN       {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::BOOLEAN);}
    | CHAR          {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::CHAR);}
    | INTEGER       {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::INTEGER);}
    | REAL          {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::REAL);}
    | STRING        {$$=ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::STRING);}
    | CARET type    {$$=ctx->GetTypeManager()->GetPointerType($2);}
    | array_type    {$$=$1;}
    ;

array_type
    :   ARRAY LBRACKET literal DOTDOT literal RBRACKET OF type  {$$=ctx->GetTypeManager()->CreateArrayType($8, $3, $5);}
    ;

/* ==================[function part]================== */

function
    : FUNCTION IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON type SEMICOLON decls statement_block    {$$=std::make_shared<pcc::FunctionNode>(ctx, std::move($2), std::move($4), std::move($9), $7, $10);}
    | FUNCTION IDENTIFIER LPARENTHESIS RPARENTHESIS COLON type SEMICOLON decls statement_block              {$$=std::make_shared<pcc::FunctionNode>(ctx, std::move($2), std::list<Declaration>(), std::move($8), $6, $9);}
    ;

/* ==================[statements part]================== */

statements
    : statements SEMICOLON statement    {$$=$1; $$->Append($3);}
    | statement                         {$$=std::make_shared<pcc::StatementListNode>(ctx); $$->Append($1);}
    ;

statement
    : open_statement                    {$$=$1;}
    | closed_statement                  {$$=$1;}
    ;

open_statement
    : IF expression THEN statement                              {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, nullptr);}
    | IF expression THEN closed_statement ELSE open_statement   {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, $6);}
    | WHILE expression DO open_statement                        {$$=std::make_shared<pcc::WhileStatementNode>(ctx, $2, $4);}
    ;

closed_statement
    : normal_statement                                          {$$=$1;}
    | IF expression THEN closed_statement ELSE closed_statement {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, $6);}
    | WHILE expression DO closed_statement                      {$$=std::make_shared<pcc::WhileStatementNode>(ctx, $2, $4);}
    ;

normal_statement
    : statement_block   {$$=$1;}
    | assign_statement  {$$=$1;}
    | repeat_statement  {$$=$1;}
    | expression        {$$=$1;}
    | BREAK             {$$=std::make_shared<pcc::BreakStatementNode>(ctx);}
    | CONTINUE          {$$=std::make_shared<pcc::ContinueStatementNode>(ctx);}
    |                   {$$=std::make_shared<pcc::EmptyStatementNode>(ctx);}
    ;

statement_block
    : BEGINS statements ENDS    {$$=$2;}
    ;

assign_statement
    : lvalue ASSIGN expression  {$$=std::make_shared<pcc::AssignStatementNode>(ctx, $1, $3);}
    ;

repeat_statement
    : REPEAT statement UNTIL expression {$$=std::make_shared<pcc::RepeatStatementNode>(ctx, $4, $2);}
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
    : l4_operator l3_expression             {$$=std::make_shared<pcc::UnaryExprNode>(ctx,$1,$2);}
    | AT lvalue                             {$$=$2;}
    | standalone_expression                 {$$=$1;}
    ;

standalone_expression
    : lvalue                                {$$=std::make_shared<pcc::L2RCastingNode>(ctx, $1);}
    | function_call                         {$$=$1;}
    | literal                               {$$=$1;}
    | LPARENTHESIS expression RPARENTHESIS  {$$=$2;}
    ;

lvalue
    : IDENTIFIER                                            {$$=std::make_shared<pcc::IdentifierNode>(ctx, $1);}
    | lvalue LBRACKET expression RBRACKET                   {$$=std::make_shared<pcc::ArrayAccessNode>(ctx, $1, $3);}
    | standalone_expression DOT IDENTIFIER
    ;

function_call
    : IDENTIFIER LPARENTHESIS arguments RPARENTHESIS    {$$=std::make_shared<pcc::FunctionCallNode>(ctx, $1, std::move($3));}
    | IDENTIFIER LPARENTHESIS RPARENTHESIS              {$$=std::make_shared<pcc::FunctionCallNode>(ctx, $1, std::list<std::shared_ptr<pcc::ExprNode>>());}
    ;

arguments
    : arguments COMMA expression    {$$=std::move($1); $$.push_back($3);}
    | expression                    {$$.push_back($1);}
    ;

literal
    : BOOLEAN_LITERAL   {$$=std::make_shared<pcc::BooleanLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::BOOLEAN), $1);}
    | CHAR_LITERAL      {$$=std::make_shared<pcc::CharLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::CHAR), $1);}
    | INTEGER_LITERAL   {$$=std::make_shared<pcc::IntegerLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::INTEGER), $1);}
    | REAL_LITERAL      {$$=std::make_shared<pcc::RealLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::REAL), $1);}
    | STRING_LITERAL    {$$=std::make_shared<pcc::StringLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::STRING), $1);}
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
    ;

l3_operator
    : MUL       {$$=pcc::BinaryOperator::MUL;}
    | DIV       {$$=pcc::BinaryOperator::DIV;}
    | REAL_DIV  {$$=pcc::BinaryOperator::REAL_DIV;}
    | MOD       {$$=pcc::BinaryOperator::MOD;}
    | AND       {$$=pcc::BinaryOperator::AND;}
    | XOR       {$$=pcc::BinaryOperator::XOR;}
    | SHL       {$$=pcc::BinaryOperator::SHL;}
    | SHR       {$$=pcc::BinaryOperator::SHR;}
    ;

l4_operator
    : NOT   {$$=pcc::UnaryOperator::NOT;}
    | ADD   {$$=pcc::UnaryOperator::POS;}
    | SUB   {$$=pcc::UnaryOperator::NEG;}
    ;

%%

namespace pcc {

    void Parser::error (const location_type& l, const std::string& m)
    {
        std::stringstream loc;
        loc<<l;
        Log(LogLevel::PCC_ERROR, "error at %s: %s", loc.str().c_str(), m.c_str());
    }

}
