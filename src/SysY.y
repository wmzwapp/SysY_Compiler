%code requires {
    #include <memory>
    #include <string>
    #include "ast/BaseAST.hh"
    #include "ast/CompUnitAST.hh"
    #include "ast/FuncDefAST.hh"
    #include "ast/BlockAST.hh"
    #include "ast/StmtAST.hh"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast/BaseAST.hh"
#include "ast/CompUnitAST.hh"
#include "ast/FuncDefAST.hh"
#include "ast/BlockAST.hh"
#include "ast/StmtAST.hh"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(BaseAST *ast, const char *s);

using namespace std;

%}

%debug

// 定义 parser 函数的附加参数
%parse-param { BaseAST *&ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
%union {
    char*       str_val;
    int         int_val;
    BaseAST*    ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN
%token <str_val> IDENT PLUS MINUS LNOT MUL DIV MOD
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp Number
%type <ast_val> AddExp MulExp
%type <str_val> UnaryOp BinaryOp1 BinaryOp2

%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
    : FuncDef {
        ast = new CompUnitAST();
        ((CompUnitAST*)ast)->setFuncDefAST($1);
    }
    ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
FuncDef
    : FuncType IDENT '(' ')' Block {
        auto* ast = new FuncDefAST();
        ast->setFuncTypeAST($1);
        ast->setIdent($2);
        free((void*)$2);
        ast->setBlockAST($5);
        $$ = ast;
    }
    ;

// 同上, 不再解释
FuncType
    : INT {
        auto* ast = new FuncTypeAST();
        ast->setFuncTypeStr("int");
        $$ = ast;
    }
    ;

Block
    : '{' Stmt '}' {
        auto* ast = new BlockAST();
        ast->setStmtAST($2);
        $$ = ast;
    }
    ;

Stmt
    : RETURN Exp ';' {
        auto *ast = new StmtAST();
        ast->setRetExp($2);
        $$ = ast;
    }
    ;

Exp
    : AddExp {
        auto* ast = new ExpAST();
        ast->set_add_exp($1);
        $$ = ast;
    }
    ;

PrimaryExp
    : '(' Exp ')' {
        auto* ast = new PrimaryExpAST();
        ast->setExp($2);
        $$ = ast;
    }
    | Number {
        auto* ast = new PrimaryExpAST();
        ast->setNumber($1);
        $$ = ast;
    }
    ;

Number
    : INT_CONST {
        auto* ast = new NumberAST();
        ast->setValue($1);
        $$ = ast;
    }
    ;

UnaryExp
    : PrimaryExp {
        auto* ast = new UnaryExpAST();
        ast->setPrimaryExp($1);
        $$ = ast;
    }
    | UnaryOp UnaryExp {
        auto* ast = new UnaryExpAST();
        ast->setUnaryOp($1);
        ast->setUnaryExp($2);
        free((void*)$1);
        $$ = ast;
    }
    ;

UnaryOp
    : PLUS | MINUS | LNOT

BinaryOp1
    : PLUS | MINUS

BinaryOp2
    : MUL | DIV | MOD

AddExp
    : MulExp {
        auto* ast = new AddExpAST();
        ast->set_mul_exp($1);
        $$ = ast;
    }
    | AddExp BinaryOp1 MulExp {
        auto* ast = new AddExpAST();
        ast->set_add_exp($1);
        ast->set_op($2);
        ast->set_mul_exp($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

MulExp
    : UnaryExp {
        auto* ast = new MulExpAST();
        ast->set_unary_exp($1);
        $$ = ast;
    }
    | MulExp BinaryOp2 UnaryExp {
        auto* ast = new MulExpAST();
        ast->set_mul_exp($1);
        ast->set_op($2);
        ast->set_unary_exp($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(BaseAST *ast, const char *s) {
    cerr << "error: " << s << endl;
}