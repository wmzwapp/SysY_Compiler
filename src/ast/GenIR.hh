#include "BaseAST.hh"


class ProgramIR;
class FunctionIR;
class BlockIR;
class ValueIR;
class AllocIR;

struct GenIRCtx : public AstVisitorContext {
	ProgramIR*	curProgram_ { nullptr };
	FunctionIR*	curFunc_	{ nullptr };
	BlockIR*	curBlock_	{ nullptr };
	ValueIR*	curValue_	{ nullptr };

  public:
	ProgramIR* get_current_programIR() { return curProgram_; }
	void set_current_programIR(ProgramIR* ir) { curProgram_ = ir; }

	FunctionIR*	get_current_functionIR() { return curFunc_; }
	void set_current_functionIR(FunctionIR* ir) { curFunc_ = ir; }

	BlockIR* get_current_blockIR() { return curBlock_; }
	void set_current_blockIR(BlockIR* ir) { curBlock_ = ir; }

	ValueIR* get_current_value() { return curValue_; }
	void set_current_value(ValueIR* ir) { curValue_ = ir; }
};


class GenIRVisitor : public AstVisitor {
public:
	using AstVisitor::visit;
	virtual void visit(FuncDefAST* func, VCtx* ctx);
	virtual void visit(BlockItemAST* blockItem, VCtx* ctx);
	virtual void visit(DeclAST* decl, VCtx* ctx);
	virtual void visit(StmtAST* stmt, VCtx* ctx);
	virtual void visit(ExpAST* expr, VCtx* ctx);
	virtual void visit(VarAST* var, VCtx* ctx);
	virtual void visit(NumberAST* num, VCtx* ctx);
};