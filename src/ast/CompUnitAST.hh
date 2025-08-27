#pragma once

#include "BaseAST.hh"
#include "FuncDefAST.hh"
#include "ir/ProgramIR.hh"


/*
	CompUnit ::= FuncDef;
*/
class CompUnitAST : public BaseAST {
  public:
	void Dump() const override;
	void GenIR(ProgramIR* ir);

  public:
	void setFuncDefAST(BaseAST* ast) { funcDef_ = (FuncDefAST*)ast; }

private:
	FuncDefAST* funcDef_ { nullptr };
};
