#include "BaseAST.hh"

namespace IR {
	class Program;
	class Function;
	class Block;
	class Value;
}
struct OperExpAST;
struct WhileExp;

struct GenIRCtx : public AstVisitorContext {
	IR::Program*	curProgram_ { nullptr };
	IR::Function*	curFunc_	{ nullptr };
	IR::Block*	curBlock_	{ nullptr };
	IR::Value*	curValue_	{ nullptr };

	IR::Block* whileEntryB_ { nullptr };
	IR::Block* whileMergeB_ { nullptr };

public:
	IR::Program* get_current_programIR() { return curProgram_; }
	void set_current_programIR(IR::Program* ir) { curProgram_ = ir; }

	IR::Function*	get_current_functionIR() { return curFunc_; }
	void set_current_functionIR(IR::Function* ir) { curFunc_ = ir; }

	IR::Block* get_current_blockIR() { return curBlock_; }
	void set_current_blockIR(IR::Block* ir) { curBlock_ = ir; }

	IR::Value* get_current_value() { return curValue_; }
	void set_current_value(IR::Value* ir) { curValue_ = ir; }

	IR::Block* get_while_entry_block() { return whileEntryB_; }
	IR::Block* get_while_merge_block() { return whileMergeB_; }
	void set_while_entry_block(IR::Block* b) { whileEntryB_ = b; }
	void set_while_merge_block(IR::Block* b) { whileMergeB_ = b; }
};


class GenIRVisitor : public AstVisitor {
public:
	using AstVisitor::visit;
	void visit(FuncDefAST* func, VCtx* ctx) override;
	void visit(BlockItemAST* blockItem, VCtx* ctx) override;
	void visit(DeclAST* decl, VCtx* ctx) override;
	void visit(StmtAST* stmt, VCtx* ctx) override;
	void visit(ExpAST* expr, VCtx* ctx) override;
	void visit(VarAST* var, VCtx* ctx) override;
	void visit(NumberAST* num, VCtx* ctx) override;

public:
	void process_return_stmt(StmtAST* stmt, GenIRCtx* ctx);
	void process_ifelse_stmt(StmtAST* stmt, GenIRCtx* ctx);
	void process_short_circuit_eval(OperExpAST* expr, GenIRCtx* ctx);
	void process_while_stmt(WhileExp* stmt, GenIRCtx* ctx);
	void process_break_continue_stmt(bool isBreak, GenIRCtx* ctx);
};