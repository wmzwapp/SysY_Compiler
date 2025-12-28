#pragma once

#include "BaseAST.hh"
#include "VarAST.hh"


class BlockAST;

/*
	FuncDef ::= FuncType IDENT "(" ")" Block;
*/
class FuncDefAST : public BaseAST {
public:
  	STATIC_TYPE_ID_DECL(FuncDefAST);
	FuncDefAST(const char* ident, BaseAST* block)
		: ident_(ident), block_((BlockAST*)block)
		{ SET_TYPE_ID(FuncDefAST); }

public:
	void Dump(std::ostream& os) const override;
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	BlockAST* get_block() { return block_; }
	VarAST* get_ident() { return &ident_; }
	TypeAST* get_ret_type() { return &funcType_; }

private:
	TypeAST		funcType_;
	VarAST		ident_;
	BlockAST*	block_		{ nullptr };
};
