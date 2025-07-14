#pragma once

#include "ir.hh"
#include "koopa.h"
#include <string>

class BaseAST {
  public:
    virtual ~BaseAST() = default;

  public:
    virtual void Dump() const = 0;
    virtual IRBase* GenIR() = 0;
};

class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class NumberAST;

/*
    CompUnit ::= FuncDef;
*/
class CompUnitAST : public BaseAST {
public:
    virtual ~CompUnitAST() {};

    void Dump() const override;

    IRBase* GenIR() override;

public:
    void GenRawProgram(koopa_raw_program_t &p);

    void            setFuncDefAST(BaseAST* ast)     { funcDef_ = (FuncDefAST*)ast; }

private:
    FuncDefAST*     funcDef_    { nullptr };
};

/*
    FuncDef ::= FuncType IDENT "(" ")" Block;
*/
class FuncDefAST : public BaseAST {
public:
    void Dump() const override;

    IRBase* GenIR() override;

public:
    void GenRawFunc(koopa_raw_function_t &fptr);

    void            setFuncTypeAST(BaseAST* ast)    { funcType_ = (FuncTypeAST*)ast; }
    void            setIdent(const char *s)         { ident_ = std::string(s); }
    void            setBlockAST(BaseAST* ast)       { block_ = (BlockAST*)ast; }

private:
    FuncTypeAST*        funcType_   { nullptr };
    std::string         ident_;
    BlockAST*           block_      { nullptr };
};

/*
    FuncType ::= "int";
*/
class FuncTypeAST : public BaseAST {
public:
    void Dump() const override;

    IRBase* GenIR() override;

public:
    void GenRawFuncRetType(koopa_raw_type_kind &t);

    void            setFuncTypeStr(const char* s)   { funcType_ = std::string(s); }

private:
    std::string     funcType_;
};

/* 
    Block ::= "{" Stmt "}";
*/
class BlockAST : public BaseAST {
public:
	void Dump() const override;

    IRBase* GenIR() override;

public:
    void GenRawBasicBlock(koopa_raw_basic_block_t &bbPtr);

    void        setStmtAST(BaseAST* ast)    { stmt_ = (StmtAST*)ast; }
    StmtAST*    getStmtAST()                { return stmt_; }

private:
    StmtAST*            stmt_       { nullptr };
};

/*
    Stmt ::= "return" Number ";";
    Number ::= INT_CONST;
*/
class StmtAST : public BaseAST {
public:
    void Dump() const override;
    IRBase* GenIR() override;


public:
    void GenRawInstr(koopa_raw_value_t &v);

    void setNum(int num) { num_ = num; }

public:
    int  num_ { 0 };
};
