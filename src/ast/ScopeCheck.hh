#include "BaseAST.hh"
#include <vector>
#include <unordered_map>


struct ScopeCheckCtx : public AstVisitorContext {
	SymTabAST* curSymTab_ { nullptr };
	std::vector<BlockAST*>	scopes_;
	std::unordered_map<std::string, std::vector<SymTabAST*>> namedVarRecord_;

	SymTabAST* get_cur_symtab() { return curSymTab_; }

	void push_scope(BlockAST* block);
	void pop_scope();

	bool has_sym(std::string sym, bool glb = true);
	VarAST* get_sym_var(std::string sym, bool glb = true);
	bool sym_is_const(std::string sym);
	void add_sym(std::string sym, int val) { curSymTab_->add_sym(sym, val); }
	void add_sym(std::string sym, VarAST* var) { curSymTab_->add_sym(sym, var); namedVarRecord_[sym].push_back(curSymTab_); }
	void handle_same_name_var_within_func();
};

class ScopeCheckerVisitor : public AstVisitor {
  public:
	using AstVisitor::visit;
	void visit(FuncDefAST* func, VCtx* ctx) override;
	void visit(BlockAST* block, VCtx* ctx) override;
	void visit(DeclAST* decl, VCtx* ctx) override;
	void visit(StmtAST* stmt, VCtx* ctx) override;
	void visit(ExpAST* expr, VCtx* ctx) override;
};