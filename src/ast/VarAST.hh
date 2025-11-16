#pragma once

#include "ast/BaseAST.hh"
#include "ir/TypeIR.hh"

#include <string>
#include <iostream>


/*
    FuncType        ::= "int";
	BType			::= "int"
*/
class TypeAST {
public:
    void repr() const { std::cout << "<Type>int"; }

    std::string get_ty_str() { return "int"; }

    static TypeIntIR* get_int_type_ir() { return new TypeIntIR(); }
    static TypePtrIR* get_int_ptr_type_ir() { return new TypePtrIR(new TypeIntIR()); }
    static TypeUnitIR* get_unit_type_ir() { return new TypeUnitIR(); }
    static TypeFuncIR* get_func_type_ir() { return new TypeFuncIR(); }
};


/*
	FuncDef         ::= FuncType IDENT "(" ")" Block;
	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";"
    ConstDef		::= IDENT "=" ConstInitVal
    VarDef          ::= IDENT | IDENT "=" InitVal
*/
class VarAST {
public:
    VarAST(const char* ident) { ident_ = ident; }

public:
    void repr() const { std::cout << "<IDENT>" << ident_; }

    const char* get_ident() const { return ident_.c_str(); }

private:
    std::string ident_;
};

/*
	Number ::= INT_CONST
*/
class NumberAST {
  public:
    NumberAST(tyI32 val): val_(val) {}
	void repr() const { std::cout << "<Number>" << val_; }

  public:
	int val_{ 0 };
};



