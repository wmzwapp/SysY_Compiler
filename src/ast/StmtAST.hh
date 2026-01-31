#pragma once

#include "BaseAST.hh"
#include "VarAST.hh"

#include <cassert>
#include <cstring>
#include <string>
#include <variant>
#include <vector>


class ExpAST;
class TypeAST;

struct varDefAST {
	bool		isConst_	{ false };
	TypeAST		type_;
	VarAST*		var_		{ nullptr };
	ExpAST*		initExp_	{ nullptr };
};

/*
	Decl			::= ConstDecl | VarDecl;

	BType			::= "int";

	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";";
	ConstDef		::= IDENT "=" ConstInitVal;
	ConstInitVal	::= ConstExp;
	ConstExp      	::= Exp;

	VarDecl       	::= BType VarDef {"," VarDef} ";";
	VarDef        	::= IDENT | IDENT "=" InitVal;
	InitVal       	::= Exp;
*/
class DeclAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(DeclAST);
	DeclAST(std::vector<varDefAST*>* v) : defs_(std::move(*v)) { SET_TYPE_ID(DeclAST); }

public:
	void Dump(std::ostream& os) const override;
    void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	std::vector<varDefAST*>& get_defs() { return defs_; }

private:
	std::vector<varDefAST*> defs_;
};



struct LAssign {
	VarAST* lval_ { nullptr };
	ExpAST*	exp_ { nullptr };
};

struct RetStmt {
	ExpAST* retExp_ { nullptr };
};

struct IFExp {
	ExpAST* condExp_ { nullptr };
	StmtAST* ifStmt_ { nullptr };
	StmtAST* elseStmt_ { nullptr };

	bool hasElse() const noexcept { return elseStmt_ != nullptr; }
};

struct BCExp {
	bool isBreak_ { false };
	bool isContinue_ { false };
};

struct WhileExp{
	ExpAST* condExp_ { nullptr };
	StmtAST* stmt_ { nullptr };

	bool hasStmt() const noexcept { return stmt_ != nullptr; }
};

/*
	Stmt			::= "return" [ Exp ] ";"
						| LVal "=" Exp ";"
						| Block
						| [ Exp ] ";"
						| "if" "(" Exp ")" Stmt ["else" Stmt]
						| "while" "(" Exp ")" Stmt
						| "break" ";"
						| "continue" ";"
*/
class StmtAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(StmtAST);
	StmtAST() { SET_TYPE_ID(StmtAST); }

public:
	void Dump(std::ostream& os) const override;
    void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	bool isAssignExp() const { return exp_.index() == 0; }
	bool isRetExp() const { return exp_.index() == 1; }
	bool isBlock() const { return exp_.index() == 2; }
	bool isExp() const { return exp_.index() == 3; }
	bool isIFExp() const { return exp_.index() == 4; }
	bool isWhileExp() const { return exp_.index() == 5; }
	bool isBreakExp() const { return exp_.index() == 6 && std::get<6>(exp_).isBreak_; }
	bool isContinueExp() const { return exp_.index() == 6 && std::get<6>(exp_).isContinue_; }

	ExpAST* getRetExp() { return std::get<1>(exp_).retExp_; }
	const ExpAST* getRetExp() const { return std::get<1>(exp_).retExp_; }
	LAssign* getAssignExp() { return &std::get<0>(exp_); }
	const LAssign* getAssignExp() const { return &std::get<0>(exp_); }
	BlockAST* getBlock() { return std::get<2>(exp_); }
	const BlockAST* getBlock() const { return std::get<2>(exp_); }
	ExpAST* getExp() { return std::get<3>(exp_); }
	const ExpAST* getExp() const { return std::get<3>(exp_); }
	IFExp* getIFExp() { return &std::get<4>(exp_); }
	const IFExp* getIFExp() const { return &std::get<4>(exp_); }
	WhileExp* getWhileExp() { return &std::get<5>(exp_); }
	const WhileExp* getWhileExp() const { return &std::get<5>(exp_); }

	void setAssignExp(BaseAST* lval, BaseAST* exp) { exp_ = LAssign {(VarAST*)lval, (ExpAST*)exp}; }
	void setRetExp(BaseAST* exp) { exp_ = RetStmt {(ExpAST*)exp}; }
	void setBlock(BaseAST* block) { exp_ = (BlockAST*)block; }
	void setExp(BaseAST* exp) { exp_ = (ExpAST*)exp; }
	void setIFExp(BaseAST* condExp, BaseAST* ifStmt, BaseAST* elseStmt = nullptr)
		{ exp_ = IFExp { (ExpAST*)condExp, (StmtAST*)ifStmt, (StmtAST*)elseStmt }; }
	void setWhileExp(BaseAST* condExp, BaseAST* stmt)
		{ exp_ = WhileExp { (ExpAST*)condExp, (StmtAST*)stmt }; }
	void setBreakStmt() { exp_ = BCExp { true, false }; }
	void setContinueStmt() { exp_ = BCExp { false, true }; }

private:
	std::variant<LAssign, RetStmt, BlockAST*, ExpAST*, IFExp, WhileExp, BCExp>	exp_ { LAssign() };
};



/*
	expression operator
*/
struct OpAST {
public:
	enum op_t {
		OP_BAD,
		PLUS,
		MINUS,
		LNOT,
		MUL,
		DIV,
		MOD,
		LT,
		GT,
		LE,
		GE,
		EQ,
		NEQ,
		LAND,
		LOR,
	};

public:
	static void set_op_ast(const char* op, OpAST& opAST);
	static void dump_op_ast(std::ostream&os, OpAST opAST);
	static std::string repr_op_ast(OpAST opAST);
	auto get_op_type() const { return op_; }
	bool is_valid_op() const { return op_ != OP_BAD; }

private:
	op_t op_;
};


/*
	expression container
*/
struct OperExpAST {
	bool is_unary_exp() const { return opnd1_ == nullptr && opnd2_ != nullptr; }
	bool is_binary_exp() const { return !is_unary_exp(); }

	ExpAST*		opnd1_	{ nullptr };
	OpAST		op_;
	ExpAST*		opnd2_	{ nullptr };
};


/*
	Exp				::= LOrExp;
	LVal			::= IDENT;
	Number			::= INT_CONST;
	PrimaryExp		::= "(" Exp ")" | LVal | Number;
	UnaryExp		::= PrimaryExp | UnaryOp UnaryExp;
	UnaryOp			::= "+" | "-" | "!";
	MulExp			::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
	AddExp			::= MulExp | AddExp ("+" | "-") MulExp;
	RelExp			::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
	EqExp			::= RelExp | EqExp ("==" | "!=") RelExp;
	LAndExp			::= EqExp | LAndExp "&&" EqExp;
	LOrExp			::= LAndExp | LOrExp "||" LAndExp;
*/
class ExpAST : public BaseAST{
public:
	STATIC_TYPE_ID_DECL(ExpAST);
	ExpAST() { SET_TYPE_ID(ExpAST); }

public:
	void Dump(std::ostream& os) const override;
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	bool is_exp() const { return opnd_.index() == 0;}
	bool is_lval() const { return opnd_.index() == 1; }
	bool is_num() const { return opnd_.index() == 2; }

	OperExpAST* get_exp() { return &std::get<0>(opnd_); }
	const OperExpAST* get_exp() const { return &std::get<0>(opnd_); }
	VarAST* get_lval() { return std::get<1>(opnd_); }
	const VarAST* get_lval() const { return std::get<1>(opnd_); }
	NumberAST* get_num() { return std::get<2>(opnd_); }
	const NumberAST* get_num() const { return std::get<2>(opnd_); }

	void set_exp(ExpAST* opnd1, OpAST op, ExpAST* opnd2) { opnd_ = OperExpAST { opnd1, op, opnd2}; }
	void set_lval(VarAST* var) { opnd_ = var; }
	void set_num(NumberAST* num) { opnd_ = num; }

private:
	std::variant<OperExpAST, VarAST*, NumberAST*> opnd_;
};


