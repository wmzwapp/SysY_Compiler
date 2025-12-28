#include "BaseAST.hh"
#include <vector>

struct ConstPropCtx : public AstVisitorContext {
	SymTabAST* curSymTab_ { nullptr };
	std::vector<BlockAST*>	scopes_;

	BaseAST* retNode_ { nullptr };

	int cal_unary_exp(OpAST op, NumberAST* opnd);
	int cal_binary_exp(OpAST op, NumberAST* opnd1, NumberAST* opnd2);

	void push_scope(BlockAST* block);
	void pop_scope();

	BaseAST* get_ret_node() { return retNode_; }
	void set_ret_node(BaseAST* node) { retNode_ = node; }

	void add_sym(std::string sym, int val) { curSymTab_->add_sym(sym, val); }

	bool sym_is_const(std::string sym);
	int get_sym_const(std::string sym);
};

class ConstPropVisitor : public AstVisitor {
  public:
	using AstVisitor::visit;
	void visit(BlockAST* block, VCtx* ctx) override;
	void visit(DeclAST* decl, VCtx* ctx) override;
	void visit(StmtAST* item, VCtx* ctx) override;
	void visit(ExpAST* decl, VCtx* ctx) override;
};