#pragma once

#include "BaseAST.hh"
#include "StmtAST.hh"
#include "ir/FunctionIR.hh"


/* 
	Block ::= "{" Stmt "}";
*/
class BlockAST : public BaseAST {
  public:
	void Dump() const override;

	void GenIR(FunctionIR* ir);

  public:
	void setStmtAST(BaseAST* ast) { stmt_ = (StmtAST*)ast; }
	StmtAST* getStmtAST() { return stmt_; }

  private:
	StmtAST* stmt_ { nullptr };
};
