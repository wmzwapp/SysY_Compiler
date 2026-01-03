#pragma once

#include "BaseAST.hh"

#include <string>
#include <iostream>

namespace IR {
	class Type;
	class Symbol;
}

/*
    FuncType        ::= "int";
	BType			::= "int";
*/
class TypeAST {
public:
	void Dump(std::ostream& os) const { os << "<Type>int"; }
	std::string repr() const { return "int"; }
	IR::Type* get_ty_IR();
};


/*
	FuncDef         ::= FuncType IDENT "(" ")" Block;
	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";";
    ConstDef		::= IDENT "=" ConstInitVal;
    VarDef          ::= IDENT | IDENT "=" InitVal;
    LVal			::= IDENT;
    Stmt            ::= LVal "=" Exp ";";
*/
class VarAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(VarAST);
	VarAST(const char* ident) : ident_(ident) { SET_TYPE_ID(VarAST); }

public:
	void Dump(std::ostream& os) const override { os << "<IDENT>" << ident_; }
	std::string repr() const override { return ident_; }
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	void set_ident(std::string ident) { ident_ = ident; }

	void set_IR_var(IR::Symbol* var) { IRVar_ = var; }
	IR::Symbol* get_IR_var() { return IRVar_; }

private:
	std::string ident_;
	IR::Symbol* IRVar_ { nullptr};
};

/*
	Number ::= INT_CONST
*/
class NumberAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(NumberAST);
	NumberAST(tyI32 val): val_(val) { SET_TYPE_ID(NumberAST); }

public:
	void Dump(std::ostream& os) const override { os << "<Number>" << val_; }
	std::string repr() const override { return std::to_string(val_); }
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }	

public:
	int get_val() const { return val_; }

private:
	int val_{ 0 };
};



