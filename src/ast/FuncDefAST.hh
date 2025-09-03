#pragma once

#include "BaseAST.hh"
#include "BlockAST.hh"
// #include "ir/ValueIR.hh"


/*
	FuncType ::= "int";
*/
class FuncTypeAST : public BaseAST {
  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override { }

  public:
	TypeIR* getType();

	void setFuncTypeStr(const char* s) { funcType_ = std::string(s); }

  private:
	std::string funcType_;
};


/*
	FuncDef ::= FuncType IDENT "(" ")" Block;
*/
class FuncDefAST : public BaseAST {
  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

	// IRBase* GenIR();

  public:
	// void GenRawFunc(koopa_raw_function_t &fptr);

	void setFuncTypeAST(BaseAST* ast) { funcType_ = (FuncTypeAST*)ast; }
	void setIdent(const char *s) { ident_ = std::string(s); }
	void setBlockAST(BaseAST* ast) { block_ = (BlockAST*)ast; }

  private:
	FuncTypeAST* funcType_ { nullptr };
	std::string ident_;
	BlockAST* block_ { nullptr };
};