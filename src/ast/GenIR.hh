#include "BaseAST.hh"

namespace IR {
	class Program;
	class Function;
	class Block;
	class Value;
}

struct GenIRCtx : public AstVisitorContext {
	IR::Program*	curProgram_ { nullptr };
	IR::Function*	curFunc_	{ nullptr };
	IR::Block*	curBlock_	{ nullptr };
	IR::Value*	curValue_	{ nullptr };

  public:
	IR::Program* get_current_programIR() { return curProgram_; }
	void set_current_programIR(IR::Program* ir) { curProgram_ = ir; }

	IR::Function*	get_current_functionIR() { return curFunc_; }
	void set_current_functionIR(IR::Function* ir) { curFunc_ = ir; }

	IR::Block* get_current_blockIR() { return curBlock_; }
	void set_current_blockIR(IR::Block* ir) { curBlock_ = ir; }

	IR::Value* get_current_value() { return curValue_; }
	void set_current_value(IR::Value* ir) { curValue_ = ir; }
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