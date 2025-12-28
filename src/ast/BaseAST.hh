#pragma once

#include "common/utils.hh"
#include "common/mmp.hh"
#include <string>
#include <sstream>
#include <unordered_map>
#include <variant>


using tyI32 = int;

class BaseAST; 
class CompUnitAST;
class FuncDefAST;
class BlockAST;
class BlockItemAST;
class DeclAST;
class StmtAST;
class ExpAST;
class VarAST;
class NumberAST;
struct OpAST;

extern MArena mmpool_;

struct AstVisitorContext {
	// empty
};

using VCtx = AstVisitorContext;

class AstVisitor {
  public:
	virtual void visit(CompUnitAST* unit, VCtx* ctx);
	virtual void visit(FuncDefAST* func, VCtx* ctx);
	virtual void visit(BlockAST* block, VCtx* ctx);
	virtual void visit(BlockItemAST* blockItem, VCtx* ctx);
	virtual void visit(DeclAST* decl, VCtx* ctx);
	virtual void visit(StmtAST* stmt, VCtx* ctx);
	virtual void visit(ExpAST* expr, VCtx* ctx);
	virtual void visit(VarAST* var, VCtx* ctx);
	virtual void visit(NumberAST* num, VCtx* ctx);
};

class BaseAST {
	ObjType type_;
public:
	virtual ~BaseAST() = default;

	void set_type_id(ObjType t) { type_ = t; }
	ObjType get_type_id() { return type_; }

public:
	virtual void Dump(std::ostream& os) const = 0;
	virtual std::string repr() const {
		std::stringstream ss;
		Dump(ss);
		return ss.str();
	}
	virtual void accept(AstVisitor* v, VCtx* ctx) = 0;
};


struct symTableValue {
	bool is_const() const { return v_.index() == 0; }
	bool is_var() const { return v_.index() == 1; }

	tyI32 get_const() { return std::get<0>(v_); }
	VarAST* get_var() { return std::get<1>(v_); }

	std::variant<tyI32, VarAST*> v_;
};


class SymTabAST {
  public:
	void add_sym(std::string sym, tyI32 val) { table_[sym] = {val}; }
	void add_sym(std::string sym, VarAST* var) { table_[sym] = {var}; }
	void remove_sym(std::string sym) { table_.erase(sym); }

	bool has_sym(std::string sym) { return table_.find(sym) != table_.end(); }
	bool is_const(std::string sym) { return table_.at(sym).is_const(); }
	bool is_lval(std::string sym) { return table_.at(sym).is_var(); }

	int get_const_val(std::string sym) { return table_.at(sym).get_const(); }
	VarAST* get_var(std::string sym) { return table_.at(sym).get_var(); }

  private:
	std::unordered_map<std::string, symTableValue> table_;
};


class ASTCheckFailed : public std::exception {
  public:
	explicit ASTCheckFailed(const std::string& msg) : msg_(msg) {}
	ASTCheckFailed(const char* fmrstr, ...);

	const char* what() const noexcept override {
		return msg_.c_str();
	}

  private:
	std::string msg_;
};

