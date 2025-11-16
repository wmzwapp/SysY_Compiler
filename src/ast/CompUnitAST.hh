#pragma once

#include "BaseAST.hh"
#include "FuncDefAST.hh"


/*
	CompUnit ::= FuncDef;
*/
class CompUnitAST : public BaseAST {
  public:
	~CompUnitAST() override { delete funcDef_; }

	void Dump() const override {
		std::cout << "<CompUnit> { ";
		funcDef_->Dump();
		std::cout << " }";
	}

	void gen_ir(GenIRCfg* cfg) override { funcDef_->gen_ir(cfg); }

  public:
	void setFuncDefAST(BaseAST* ast) { funcDef_ = (FuncDefAST*)ast; }

private:
	FuncDefAST* funcDef_ { nullptr };
};
