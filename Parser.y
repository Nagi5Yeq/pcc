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
#include "TypeIdentifier.hh"
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
%token PROGRAM IDENTIFIER VAR CONST TYPE BEGINS ENDS FUNCTION EXTERN DOTDOTDOT
%token IF THEN ELSE WHILE DO REPEAT UNTIL BREAK CONTINUE FOR TO CASE
%token VOID BOOLEAN CHAR SHORT INTEGER INT64 REAL STRING ARRAY OF RECORD
%token ADD SUB MUL REAL_DIV DIV MOD LT LE GT GE EQ NE CARET AT
%token AND NOT OR XOR SHL SHR
%token INTEGER_LITERAL REAL_LITERAL STRING_LITERAL BOOLEAN_LITERAL CHAR_LITERAL

%type <std::string> IDENTIFIER program_header

%type <std::list<pcc::Declaration>> var_decls
%type <std::pair<std::list<std::string>, std::shared_ptr<pcc::TypeIdentifier>>> var_decl
%type <std::list<std::string>> vars
%type <std::list<pcc::Constant>> const_decls
%type <pcc::Constant> const_decl
%type <std::list<pcc::Declaration>> type_decls member_decls
%type <pcc::Declaration> type_decl member_decl
%type <std::shared_ptr<TypeIdentifier>> type array_type record_type

%type <bool> BOOLEAN_LITERAL TO
%type <char> CHAR_LITERAL
%type <long long> INTEGER_LITERAL
%type <float> REAL_LITERAL
%type <std::vector<char>> STRING_LITERAL
%type <std::shared_ptr<pcc::ExprNode>> literal lvalue rvalue standalone
%type <std::list<std::shared_ptr<pcc::ExprNode>>> arguments
%type <std::shared_ptr<pcc::FunctionCallNode>> function_call
%type <std::shared_ptr<pcc::ExprNode>> expression l1_expression l2_expression l3_expression
%type <pcc::BinaryOperator> l1_operator l2_operator l3_operator
%type <pcc::UnaryOperator> l4_operator

%type <std::list<std::shared_ptr<pcc::FunctionNode>>> functions
%type <std::shared_ptr<pcc::FunctionNode>> function
%type <std::list<std::shared_ptr<pcc::DeclNode>>> decls
%type <std::shared_ptr<pcc::DeclNode>> decl
%type <std::shared_ptr<pcc::VarDeclNode>> var_decl_statement
%type <std::shared_ptr<pcc::ConstDeclNode>> const_decl_statement
%type <std::shared_ptr<pcc::TypeDeclNode>> type_decl_statement

%type <std::shared_ptr<pcc::StatementListNode>> statements statement_block
%type <std::shared_ptr<pcc::BaseNode>> statement open_statement closed_statement normal_statement
%type <std::shared_ptr<pcc::AssignStatementNode>> assign_statement
%type <std::shared_ptr<pcc::RepeatStatementNode>> repeat_statement
%type <std::shared_ptr<pcc::SwitchStatementNode>> switch_statement
%type <std::list<pcc::CasePair>> case_statements
%type <std::list<std::shared_ptr<pcc::ExprNode>>> cases

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
    | type_decl_statement   {$$=$1;}
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
    : var_decls SEMICOLON var_decl  {$$=std::move($1); auto NewChilds=std::get<0>($3); for(auto&& name: NewChilds){$$.push_back({name, std::get<1>($3)});}}
    | var_decl                      {auto NewChilds=std::get<0>($1); for(auto&& name: NewChilds){$$.push_back({name, std::get<1>($1)});}}
    ;

var_decl
    : vars COLON type               {$$={$1, $3};}
    ;

vars
    : vars COMMA IDENTIFIER         {$$=std::move($1); $$.push_back($3);}
    | IDENTIFIER                    {$$.push_back($1);}
    ;

const_decl_statement
    : CONST const_decls SEMICOLON       {$$=std::make_shared<pcc::ConstDeclNode>(ctx, nullptr, std::move($2));}
    ;

const_decls
    : const_decls SEMICOLON const_decl  {$$=std::move($1); $$.push_back($3);}
    | const_decl                        {$$.push_back($1);}
    ;

const_decl
    : IDENTIFIER EQ literal             {$$={$1, $3};}
    ;

type_decl_statement
    : TYPE type_decls SEMICOLON         {$$=std::make_shared<pcc::TypeDeclNode>(ctx, nullptr, std::move($2));}
    ;

type_decls
    : type_decls SEMICOLON type_decl    {$$=std::move($1); $$.push_back($3);}
    | type_decl                         {$$.push_back($1);}
    ;

type_decl
    : IDENTIFIER EQ type                {$$={$1, $3};}
    ;

type
    : VOID          {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::VOID));}
    | BOOLEAN       {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::BOOLEAN));}
    | CHAR          {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::CHAR));}
    | SHORT         {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::SHORT));}
    | INTEGER       {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::INTEGER));}
    | INT64         {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::INT64));}
    | REAL          {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::REAL));}
    | STRING        {$$=std::make_shared<TrivialTypeIdentifier>(ctx, ctx->GetTypeManager()->GetBuiltinType(pcc::BuiltinType::STRING));}
    | CARET type    {$$=std::make_shared<PointerTypeIdentifier>(ctx, $2);}
    | array_type    {$$=$1;}
    | record_type   {$$=$1;}
    | IDENTIFIER    {$$=std::make_shared<AliasTypeIdentifier>(ctx, std::move($1));}
    ;

array_type
    : ARRAY LBRACKET literal DOTDOT literal RBRACKET OF type    {$$=std::make_shared<ArrayTypeIdentifier>(ctx, $8, $3, $5);}
    ;

record_type
    : RECORD member_decls ENDS              {$$=std::make_shared<pcc::RecordTypeIdentifier>(ctx, std::move($2));}
    ;

member_decls
    : member_decls SEMICOLON member_decl    {$$=std::move($1); $$.push_back($3);}
    | member_decl                           {$$.push_back($1);}
    ;

member_decl
    : IDENTIFIER COLON type                 {$$={$1, $3};}
    ;

/* ==================[function part]================== */

function
    : FUNCTION IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON type SEMICOLON decls statement_block    {$$=std::make_shared<pcc::FunctionNode>(ctx, std::move($2), std::move($4), std::move($9), $7, $10);}
    | FUNCTION IDENTIFIER LPARENTHESIS RPARENTHESIS COLON type SEMICOLON decls statement_block              {$$=std::make_shared<pcc::FunctionNode>(ctx, std::move($2), std::list<Declaration>(), std::move($8), $6, $9);}
    | EXTERN IDENTIFIER LPARENTHESIS var_decls RPARENTHESIS COLON type                                      {$$=std::make_shared<pcc::ExternNode>(ctx, std::move($2), std::move($4), $7, false);}
    | EXTERN IDENTIFIER LPARENTHESIS RPARENTHESIS COLON type                                                {$$=std::make_shared<pcc::ExternNode>(ctx, std::move($2), std::list<Declaration>(), $6, false);}
    | EXTERN IDENTIFIER LPARENTHESIS var_decls SEMICOLON DOTDOTDOT RPARENTHESIS COLON type                  {$$=std::make_shared<pcc::ExternNode>(ctx, std::move($2), std::move($4), $9, true);}
    ;

/* ==================[statements part]================== */

statements
    : statements SEMICOLON statement    {$$=$1; $$->Append($3);}
    | statement                         {$$=std::make_shared<pcc::StatementListNode>(ctx); $$->Append($1);}
    ;

statement
    : open_statement                    {$$=$1;}
    | closed_statement                  {$$=$1;}
    | error                             {$$=std::make_shared<pcc::EmptyStatementNode>(ctx);}
    ;

open_statement
    : IF expression THEN statement                                      {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, nullptr);}
    | IF expression THEN closed_statement ELSE open_statement           {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, $6);}
    | WHILE expression DO open_statement                                {$$=std::make_shared<pcc::WhileStatementNode>(ctx, $2, $4);}
    | FOR lvalue ASSIGN expression TO expression DO open_statement      {$$=std::make_shared<pcc::ForStatementNode>(ctx, $2, $4, $6, $5, $8);}
    ;

closed_statement
    : normal_statement                                                  {$$=$1;}
    | IF expression THEN closed_statement ELSE closed_statement         {$$=std::make_shared<pcc::IfStatementNode>(ctx, $2, $4, $6);}
    | WHILE expression DO closed_statement                              {$$=std::make_shared<pcc::WhileStatementNode>(ctx, $2, $4);}
    | FOR lvalue ASSIGN expression TO expression DO closed_statement    {$$=std::make_shared<pcc::ForStatementNode>(ctx, $2, $4, $6, $5, $8);}
    ;

normal_statement
    : statement_block   {$$=$1;}
    | assign_statement  {$$=$1;}
    | repeat_statement  {$$=$1;}
    | switch_statement  {$$=$1;}
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

switch_statement
    : CASE expression OF case_statements ENDS                           {$$=std::make_shared<SwitchStatementNode>(ctx, $2, std::move($4), nullptr);}
    | CASE expression OF case_statements ELSE statement SEMICOLON ENDS  {$$=std::make_shared<SwitchStatementNode>(ctx, $2, std::move($4), $6);}
    ;

case_statements
    : case_statements cases COLON statement SEMICOLON   {$$=std::move($1); for(auto&& child:$2){$$.push_back({child, $4});}}
    | cases COLON statement SEMICOLON                   {for(auto&& child:$1){$$.push_back({child, $3});}}
    ;

cases
    : cases COMMA literal   {$$=std::move($1); $$.push_back($3);}
    | literal               {$$.push_back($1);}
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
    | standalone                            {$$=$1;}
    ;

standalone
    : rvalue                                {$$=$1;}
    | lvalue                                {$$=std::make_shared<pcc::L2RCastingNode>(ctx, $1);}
    ;

rvalue
    : function_call                         {$$=$1;}
    | literal                               {$$=$1;}
    | LPARENTHESIS expression RPARENTHESIS  {$$=$2;}
    ;

lvalue
    : IDENTIFIER                            {$$=std::make_shared<pcc::IdentifierNode>(ctx, $1);}
    | lvalue LBRACKET expression RBRACKET   {$$=std::make_shared<pcc::ArrayAccessNode>(ctx, $1, $3);}
    | rvalue LBRACKET expression RBRACKET   {$$=std::make_shared<pcc::PointerAccessNode>(ctx, $1, $3);}
    | lvalue DOT IDENTIFIER                 {$$=std::make_shared<pcc::MemberAccessNode>(ctx, $1, std::move($3));}
    | standalone CARET                      {$$=std::make_shared<pcc::DereferenceNode>(ctx, $1);}
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
    | INTEGER_LITERAL   {$$=std::make_shared<pcc::IntegerLiteralNode>(ctx, ctx->GetTypeManager()->GetBuiltinType(BuiltinType::INT64), $1);}
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
