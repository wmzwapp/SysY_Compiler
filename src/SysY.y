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
    std::vector<BlockItemAST*>* ast_block_items;
    std::vector<ConstDefAST*>*  ast_const_defs;
    std::vector<VarDefAST*>*    ast_var_defs;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token INT RETURN CONST
%token <str_val> IDENT PLUS MINUS LNOT MUL DIV MOD LT GT LE GE EQ NEQ LAND LOR
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <int_val> Number
%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp UnaryExp
%type <ast_val> AddExp MulExp RelExp EqExp LAndExp LOrExp
%type <ast_val> Decl ConstDecl BType ConstDef ConstInitVal BlockItem LVal ConstExp
%type <ast_val> VarDecl VarDef InitVal
%type <ast_block_items> BlockItemList
%type <ast_const_defs>  ConstDefList
%type <ast_var_defs>    VarDefList
%type <str_val> UnaryOp BinaryOp1 BinaryOp2 BinaryOp3 BinaryOp4

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
        auto* ast = new FuncDefAST($2, $5);
        free((void*)$2);
        $$ = ast;
    }
    ;

// 同上, 不再解释
FuncType
    : INT {
        /*
        auto* ast = new FuncTypeAST();
        ast->setFuncTypeStr("int");
        $$ = ast;
        */
    }
    ;

Block
    : '{' BlockItemList '}' {
        auto* ast = new BlockAST($2);
        $$ = ast;
    }
    ;

BlockItemList
    : /* empty */ {
        $$ = new std::vector<BlockItemAST*>();
    }
    | BlockItemList BlockItem {
        $1->push_back((BlockItemAST*)$2);
        $$ = $1;
    }

BlockItem
    : Decl {
        auto* ast = new BlockItemAST($1);
        $$ = ast;
    }
    | Stmt {
        auto* ast = new BlockItemAST($1);
        $$ = ast;
    }
    ;

Decl
    : ConstDecl {
        auto* ast = new DeclAST();
        ast->set_const_decl($1);
        $$ = ast;
    }
    | VarDecl {
        auto* ast = new DeclAST();
        ast->set_var_decl($1);
        $$ = ast;
    }
    ;

ConstDecl
    : CONST BType ConstDefList ';' {
        $$ = new ConstDeclAST($3);
    }
    ;

BType
    : INT {
        // empty
    }
    ;

ConstDefList
    : ConstDef {
        $$ = new std::vector<ConstDefAST*>();
        $$->push_back((ConstDefAST*)$1);
    }
    | ConstDefList ',' ConstDef {
        $1->push_back((ConstDefAST*)$3);
        $$ = $1;
    }

ConstDef
    : IDENT '=' ConstInitVal {
        $$ = new ConstDefAST($1, $3);
        tyI32 initval;
        try {
            initval = ((ConstInitValAST*)$3)->eval();
        } catch (const ASTExpEvalFailed& e) {
            yyerror($3, "Not a constant expression.\n");
            free((void*)$1);
            delete $$;
            YYABORT;
        }
        gSymTable_.add_sym($1, initval);
        free((void*)$1);
    }
    ;

ConstInitVal
    : ConstExp {
        $$ = new ConstInitValAST($1);
    }
    ;

VarDecl
    : BType VarDefList ';' {
        $$ = new VarDeclAST($2);
    }
    ;

VarDefList
    : VarDef {
        $$ = new std::vector<VarDefAST*>();
        $$->push_back((VarDefAST*)$1);
    }
    | VarDefList ',' VarDef {
        $1->push_back((VarDefAST*)$3);
        $$ = $1;
    }
    ;

VarDef
    : IDENT {
        if (gSymTable_.has_sym($1)) {
            yyerror(nullptr, "Re-definition of symbol '%s'.\n", $1);
            free((void*)$1);
            YYABORT;
        }
        gSymTable_.add_sym($1, nullptr);
        $$ = new VarDefAST($1);
        free((void*)$1);
    }
    | IDENT '=' InitVal {
        if (gSymTable_.has_sym($1)) {
            yyerror(nullptr, "Re-definition of symbol '%s'.\n", $1);
            free((void*)$1);
            YYABORT;
        }
        gSymTable_.add_sym($1, nullptr);
        $$ = new VarDefAST($1, $3);
        free((void*)$1);
    }
    ;

InitVal
    : Exp {
        $$ = new InitValAST($1);
    }
    ;

Stmt
    : RETURN Exp ';' {
        auto *ast = new StmtAST();
        ast->setRetExp($2);
        $$ = ast;
    }
    | LVal '=' Exp ';' {
        auto sym = ((LValAST*)$1)->repr();
        if (auto& v = gSymTable_.get_sym(sym); v.is_const()) {
            yyerror(nullptr, "Error: constant as lValue '%s'.", sym.c_str());
            free((void*)$1);
            YYABORT;
        }
        auto *ast = new StmtAST();
        ast->setAssignExp($1, $3);
        $$ = ast;
    }
    ;

Exp
    : LOrExp {
        auto* ast = new ExpAST();
        ast->set_exp($1);
        $$ = ast;
    }
    ;

LVal
    : IDENT {
        if (!gSymTable_.has_sym($1)) {
            yyerror(nullptr, "Error: undefined symbol '%s'.", $1);
            free((void*)$1);
            YYABORT;
        }
        $$ = new LValAST($1);
        free((void*)$1);
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
    | LVal {
        auto* ast = new PrimaryExpAST();
        ast->setLval($1);
        $$ = ast;
    }
    ;

Number
    : INT_CONST {
        $$ = $1;
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

BinaryOp3
    : LT | GT | LE | GE

BinaryOp4
    : EQ | NEQ

AddExp
    : MulExp {
        auto* ast = BinaryExpAST::create_add_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | AddExp BinaryOp1 MulExp {
        auto* ast = BinaryExpAST::create_add_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

MulExp
    : UnaryExp {
        auto* ast = BinaryExpAST::create_mul_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | MulExp BinaryOp2 UnaryExp {
        auto* ast = BinaryExpAST::create_mul_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

RelExp
    : AddExp {
        auto* ast = BinaryExpAST::create_rel_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | RelExp BinaryOp3 AddExp {
        auto* ast = BinaryExpAST::create_rel_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

EqExp
    : RelExp {
        auto* ast = BinaryExpAST::create_eq_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | EqExp BinaryOp4 RelExp {
        auto* ast = BinaryExpAST::create_eq_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

LAndExp
    : EqExp {
        auto* ast = BinaryExpAST::create_land_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | LAndExp LAND EqExp {
        auto* ast = BinaryExpAST::create_land_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

LOrExp
    : LAndExp {
        auto* ast = BinaryExpAST::create_lor_exp();
        assert(is_exp_family($1));
        ast->set_other_exp($1);
        $$ = ast;
    }
    | LOrExp LOR LAndExp {
        auto* ast = BinaryExpAST::create_lor_exp();
        ast->set_binary_opnd1($1);
        ast->set_binary_op($2);
        ast->set_binary_opnd2($3);
        free((void*)$2);
        $$ = ast;
    }
    ;

ConstExp
    : Exp {
        $$ = new ConstExpAST($1);
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