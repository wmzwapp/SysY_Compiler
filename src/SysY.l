%option noyywrap
%option nounput
%option noinput

%{

#include <cstdlib>
#include <string.h>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "SysY.tab.hh"

using namespace std;

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*
LinesComment  "/*"([^*\n])*"*/"

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }
{LinesComment}  {}

"int"           { return INT; }
"return"        { return RETURN; }
"const"         { return CONST; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"+"             { yylval.str_val = strdup(yytext); return PLUS; }
"-"             { yylval.str_val = strdup(yytext); return MINUS; }
"!"             { yylval.str_val = strdup(yytext); return LNOT; }
"*"             { yylval.str_val = strdup(yytext); return MUL; }
"/"             { yylval.str_val = strdup(yytext); return DIV; }
"%"             { yylval.str_val = strdup(yytext); return MOD; }
"<"             { yylval.str_val = strdup(yytext); return LT; }
">"             { yylval.str_val = strdup(yytext); return GT; }
"<="            { yylval.str_val = strdup(yytext); return LE; }
">="            { yylval.str_val = strdup(yytext); return GE; }
"=="            { yylval.str_val = strdup(yytext); return EQ; }
"!="            { yylval.str_val = strdup(yytext); return NEQ; }
"&&"            { yylval.str_val = strdup(yytext); return LAND; }
"||"            { yylval.str_val = strdup(yytext); return LOR; }

{Identifier}    { yylval.str_val = strdup(yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); return INT_CONST; }

.               { return yytext[0]; }

%%