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
#include <cstdarg>
#include <cstdio>
#include <cassert>
#include "ast/BaseAST.hh"
#include "ast/CompUnitAST.hh"
#include "ast/FuncDefAST.hh"
#include "ast/BlockAST.hh"
#include "ast/StmtAST.hh"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(BaseAST *ast, const char* fmt, ...);

using namespace std;

extern CompUnitAST __AST_TOP__;
extern MArena mmpool_;

%}

%debug

// 定义 parser 函数的附加参数
%parse-param { BaseAST *&ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
%union {
    char*                       str_val;
    int                         int_val;
    BaseAST*                    ast_val;
    varDefAST*                  var_def;
    ExpAST*                     exp_val;
    std::vector<varDefAST*>*    var_def_v;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN CONST
%token <str_val> IDENT PLUS MINUS LNOT MUL DIV MOD LT GT LE GE EQ NEQ LAND LOR
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Decl
%type <ast_val> BlockItemList BlockItem
%type <ast_val> BType LVal Number

%type <exp_val> Exp ConstInitVal InitVal ConstExp
%type <exp_val> UnaryExp AddExp MulExp RelExp EqExp LAndExp LOrExp PrimaryExp

%type <var_def> ConstDef VarDef
%type <var_def_v>  ConstDecl ConstDefList VarDecl VarDefList


%type <str_val> UnaryOp BinaryOp1 BinaryOp2 BinaryOp3 BinaryOp4

%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
    : FuncDef {
        __AST_TOP__.setFuncDefAST($1);
    }
    ;

// FuncDef ::= FuncType IDENT '(' ')' Block;
// 我们这里可以直接写 '(' 和 ')', 因为之前在 lexer 里已经处理了单个字符的情况
// 解析完成后, 把这些符号的结果收集起来, 然后拼成一个新的字符串, 作为结果返回
// $$ 表示非终结符的返回值, 我们可以通过给这个符号赋值的方法来返回结果
FuncDef
    : FuncType IDENT '(' ')' Block {
        auto* ast = mmpool_.make<FuncDefAST>($2, $5);
        free((void*)$2);
        $$ = ast;
    }
    ;

// 同上, 不再解释
FuncType
    : INT {
        // empty
    }
    ;

Block
    : '{' BlockItemList '}' {
        $$ = $2;
    }
    ;

BlockItemList
    : /* empty */ {
        $$ = mmpool_.make<BlockAST>();
    }
    | BlockItemList BlockItem {
        if ($2 != nullptr) {
            ((BlockAST*)$1)->add_item((BlockItemAST*)$2);
        }
        $$ = $1;
    }

BlockItem
    : Decl {
        auto* ast = mmpool_.make<BlockItemAST>($1);
        $$ = ast;
    }
    | Stmt {
        if ($1 != nullptr) {
            auto* ast = mmpool_.make<BlockItemAST>($1);
            $$ = ast;
        } else {
            $$ = nullptr;
        }
    }
    ;

Decl
    : ConstDecl {
        auto* ast = mmpool_.make<DeclAST>($1);
        delete $1;
        $$ = ast;
    }
    | VarDecl {
        auto* ast = mmpool_.make<DeclAST>($1);
        delete $1;
        $$ = ast;
    }
    ;

ConstDecl
    : CONST BType ConstDefList ';' {
        for (auto* def : *$3) {
            def->isConst_ = true;
        }
        $$ = $3;
    }
    ;

BType
    : INT {
        // empty
    }
    ;

ConstDefList
    : ConstDef {
        $$ = new std::vector<varDefAST*>;
        $$->push_back($1);
    }
    | ConstDefList ',' ConstDef {
        $1->push_back($3);
        $$ = $1;
    }

ConstDef
    : IDENT '=' ConstInitVal {
        auto* vardef = mmpool_.make<varDefAST>();
        vardef->var_ = mmpool_.make<VarAST>($1);
        if ($3 == nullptr) {
            yyerror(nullptr, "The constant definition lacks an initialization expression.");
            free((void*)$1);
            YYABORT;
        }
        vardef->initExp_ = $3;
        $$ = vardef;
        free((void*)$1);
    }
    ;

ConstInitVal
    : ConstExp {
        $$ = $1;
    }
    ;

VarDecl
    : BType VarDefList ';' {
        $$ = $2;
    }
    ;

VarDefList
    : VarDef {
        $$ = new std::vector<varDefAST*>;
        $$->push_back($1);
    }
    | VarDefList ',' VarDef {
        $1->push_back($3);
        $$ = $1;
    }
    ;

VarDef
    : IDENT {
        auto* vardef = mmpool_.make<varDefAST>();
        vardef->var_ = mmpool_.make<VarAST>($1);
        $$ = vardef;
        free((void*)$1);
    }
    | IDENT '=' InitVal {
        auto* vardef = mmpool_.make<varDefAST>();
        vardef->var_ = mmpool_.make<VarAST>($1);
        vardef->initExp_ = $3;
        $$ = vardef;
        free((void*)$1);
    }
    ;

InitVal
    : Exp {
        $$ = $1;
    }
    ;

Stmt
    : RETURN Exp ';' {
        auto *stmt = mmpool_.make<StmtAST>();
        stmt->setRetExp($2);
        $$ = stmt;
    }
    | RETURN ';' {
        auto *stmt = mmpool_.make<StmtAST>();
        stmt->setRetExp(nullptr);
        $$ = stmt;
    }
    | LVal '=' Exp ';' {
        auto *stmt = mmpool_.make<StmtAST>();
        stmt->setAssignExp($1, $3);
        $$ = stmt;
    }
    | Block {
        auto* stmt = mmpool_.make<StmtAST>();
        stmt->setBlock($1);
        $$ = stmt;
    }
    | Exp ';' {
        auto* stmt = mmpool_.make<StmtAST>();
        stmt->setExp($1);
        $$ = stmt;
    }
    | ';' {
        // empty
        $$ = nullptr;
    }
    ;

Exp
    : LOrExp {
        $$ = $1;
    }
    ;

LVal
    : IDENT {
        $$ = mmpool_.make<VarAST>($1);
        free((void*)$1);
    }
    ;

PrimaryExp
    : '(' Exp ')' {
        $$ = $2;
    }
    | Number {
        auto* exp = mmpool_.make<ExpAST>();
        exp->set_num((NumberAST*)$1);
        $$ = exp;
    }
    | LVal {
        auto* exp = mmpool_.make<ExpAST>();
        exp->set_lval((VarAST*)$1);
        $$ = exp;
    }
    ;

Number
    : INT_CONST {
        $$ = mmpool_.make<NumberAST>($1);
    }
    ;

UnaryExp
    : PrimaryExp {
        $$ = $1;
    }
    | UnaryOp UnaryExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($1, opast);
        exp->set_exp(nullptr, opast, (ExpAST*)$2);
        $$ = exp;

        free((void*)$1);
    }
    ;

UnaryOp
    : PLUS | MINUS | LNOT

BinaryOp1
    : PLUS | MINUS

BinaryOp2
    : MUL | DIV | MOD

BinaryOp3
    : LT | GT | LE | GE

BinaryOp4
    : EQ | NEQ

AddExp
    : MulExp {
        $$ = $1;
    }
    | AddExp BinaryOp1 MulExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

MulExp
    : UnaryExp {
        $$ = $1;
    }
    | MulExp BinaryOp2 UnaryExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

RelExp
    : AddExp {
        $$ = $1;
    }
    | RelExp BinaryOp3 AddExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

EqExp
    : RelExp {
        $$ = $1;
    }
    | EqExp BinaryOp4 RelExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

LAndExp
    : EqExp {
        $$ = $1;
    }
    | LAndExp LAND EqExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

LOrExp
    : LAndExp {
        $$ = $1;
    }
    | LOrExp LOR LAndExp {
        auto* exp = mmpool_.make<ExpAST>();
        OpAST opast;
        OpAST::set_op_ast($2, opast);
        exp->set_exp((ExpAST*)$1, opast, (ExpAST*)$3);
        $$ = exp;

        free((void*)$2);
    }
    ;

ConstExp
    : Exp {
        $$ = $1;
    }
    ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(BaseAST* ast, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    //cerr << "error: " << s << endl;
}