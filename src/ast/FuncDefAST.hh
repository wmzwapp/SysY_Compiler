#pragma once

#include "BaseAST.hh"
#include "BlockAST.hh"
#include "VarAST.hh"


/*
	FuncDef ::= FuncType IDENT "(" ")" Block;
*/
class FuncDefAST : public BaseAST {
  public:
  FuncDefAST(const char* ident, BaseAST* block): ident_(ident), block_((BlockAST*)block) {}
	~FuncDefAST() override { delete block_; }

	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

  public:
	// void setFuncTypeAST() { funcType_ = new TypeAST(); }
	// void setIdent(const char *s) { ident_ = new VarAST(s); }
	// void setBlockAST(BaseAST* ast) { block_ = (BlockAST*)ast; }

  private:
	TypeAST		funcType_;
	VarAST		ident_;
	BlockAST*	block_		{ nullptr };
};
