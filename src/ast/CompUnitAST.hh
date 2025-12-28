#pragma once

#include "BaseAST.hh"

class FuncDefAST;

/*
	CompUnit ::= FuncDef;
*/
class CompUnitAST : public BaseAST {
  public:
	void Dump(std::ostream& os) const override;
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

  public:
	void setFuncDefAST(BaseAST* ast) { funcDef_ = (FuncDefAST*)ast; }
	FuncDefAST* get_func_def() { return funcDef_; }

  private:
	FuncDefAST* funcDef_ { nullptr };
};

inline CompUnitAST __AST_TOP__;
