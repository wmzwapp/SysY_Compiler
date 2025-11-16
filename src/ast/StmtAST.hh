#pragma once

#include "BaseAST.hh"
#include "VarAST.hh"
#include "common/utils.hh"

#include <cassert>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

class DeclAST;
class StmtAST;
class ConstDeclAST;
class ConstDefAST;
class ConstInitValAST;
class LValAST;
class ExpAST;
class UnaryExpAST;
class NumberAST;
class BinaryExpAST;
class ConstExpAST;
class VarDeclAST;
class VarDefAST;
class InitValAST;

/*
	Decl			::= ConstDecl | VarDecl

	BType			::= "int"

	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";"
	ConstDef		::= IDENT "=" ConstInitVal
	ConstInitVal	::= ConstExp
	ConstExp      	::= Exp

	VarDecl       	::= BType VarDef {"," VarDef} ";"
	VarDef        	::= IDENT | IDENT "=" InitVal
	InitVal       	::= Exp
*/
class DeclAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(DeclAST);
	DeclAST() { SET_TYPE_ID(DeclAST); }
	~DeclAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

  public:
	bool is_const_decl() const { return decl_.index() == 0; }
	bool is_var_decl() const { return decl_.index() == 1; }

	ConstDeclAST* get_const_decl() { return std::get<0>(decl_); }
	VarDeclAST* get_var_decl() { return std::get<1>(decl_); }

	const ConstDeclAST* get_const_decl() const { return std::get<0>(decl_); }
	const VarDeclAST* get_var_decl() const { return std::get<1>(decl_); }

	void set_const_decl(BaseAST* ast) { decl_ = (ConstDeclAST*)ast; }
	void set_var_decl(BaseAST* ast) { decl_ = (VarDeclAST*)ast; }

  private:
	std::variant<ConstDeclAST*, VarDeclAST*> decl_;
};


/*
	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";"
*/
class ConstDeclAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(ConstDeclAST);
	ConstDeclAST(std::vector<ConstDefAST*>* defs)
		: defs_(std::move(*defs)) { SET_TYPE_ID(ConstDeclAST); }
	~ConstDeclAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override { /* do nothing */ }

  private:
	std::vector<ConstDefAST*> defs_;
};


/*
	ConstDecl		::= "const" BType ConstDef {"," ConstDef} ";"
	ConstDef		::= IDENT "=" ConstInitVal
*/
class ConstDefAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(ConstDefAST);
	ConstDefAST(const char* ident, BaseAST* ast)
		:ident_(ident), constInitVal_((ConstInitValAST*)ast) { SET_TYPE_ID(ConstDefAST); }
	~ConstDefAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override { /* do nothing */ }

  private:
	TypeAST				type_;
	VarAST				ident_;
	ConstInitValAST*	constInitVal_ { nullptr };
};


/*
	ConstInitVal	::= ConstExp
*/
class ConstInitValAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(ConstInitValAST);
	ConstInitValAST(BaseAST* ast)
		: constExp_((ConstExpAST*)ast) { SET_TYPE_ID(ConstInitValAST); }
	~ConstInitValAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override { /* do nothing */ }

	tyI32 eval() const;

  private:
	ConstExpAST* constExp_ { nullptr };
};


/*
	VarDecl       ::= BType VarDef {"," VarDef} ";"
*/
class VarDeclAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(VarDeclAST);
	VarDeclAST(std::vector<VarDefAST*>* defs)
		: defs_(std::move(*defs)) { SET_TYPE_ID(VarDeclAST); }

	void Dump() const override;

	void gen_ir(GenIRCfg* cfg) override;

public:


private:
	std::vector<VarDefAST*> defs_;
};


/*
	VarDef        ::= IDENT | IDENT "=" InitVal
*/
class VarDefAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(VarDefAST);
	VarDefAST(const char* ident, BaseAST* ast = nullptr)
		: ident_(ident), initVal_((InitValAST*)ast) { SET_TYPE_ID(VarDefAST); }

public:
	void Dump() const override;

	void gen_ir(GenIRCfg* cfg) override;

private:
	TypeAST				type_;
	VarAST				ident_;
	InitValAST*			initVal_ { nullptr };
};


/*
	Stmt			::= "return" Exp ";" | LVal "=" Exp ";"
*/
class StmtAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(StmtAST);
	StmtAST() { SET_TYPE_ID(StmtAST); }

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

  public:
	struct lassign {
		lassign(LValAST* lval = nullptr, ExpAST* ast = nullptr) noexcept
			: lval_(lval), exp_(ast) {}
		LValAST* lval_ { nullptr };
		ExpAST*	exp_ { nullptr };
	};

	bool isRetExp() const { return exp_.index() == 1; }
	bool isAssignExp() const { return exp_.index() == 0; }
	ExpAST* getRetExp() { return std::get<1>(exp_); }
	const ExpAST* getRetExp() const { return std::get<1>(exp_); }
	lassign* getAssignExp() { return &std::get<0>(exp_); }
	const lassign* getAssignExp() const { return &std::get<0>(exp_); }
	void setRetExp(BaseAST* exp) { exp_ = (ExpAST*)exp; }
	void setAssignExp(BaseAST* lval, BaseAST* exp) { exp_ = lassign((LValAST*)lval, (ExpAST*)exp); }

  private:
	std::variant<lassign, ExpAST*>	exp_ { lassign() };
};

/*
	Exp ::= LOrExp
*/
class ExpAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(ExpAST);
	ExpAST() { SET_TYPE_ID(ExpAST); }

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;
	void set_exp(BaseAST* exp) { binaryExp_ = (BinaryExpAST*)exp; }

	virtual tyI32 try_eval() const;
	virtual std::string repr() const;

  private:
	BinaryExpAST* binaryExp_ { nullptr };
};

/*
	PrimaryExp ::= "(" Exp ")" | Number | LVal
*/
class PrimaryExpAST : public ExpAST {
  public:
	STATIC_TYPE_ID_DECL(PrimaryExpAST);
	PrimaryExpAST() { SET_TYPE_ID(PrimaryExpAST); }

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;
	tyI32 try_eval() const override;
	std::string repr() const override;

  public:
	bool is_exp_sub() const { return item_.index() == 0; }
	ExpAST* get_exp_sub() { return std::get<0>(item_); }
	const ExpAST* get_exp_sub() const { return std::get<0>(item_); }

	bool is_num_sub() const { return item_.index() == 1; }
	NumberAST* get_num_sub() { return std::get<1>(item_); }
	const NumberAST* get_num_sub() const { return std::get<1>(item_); }

	bool is_lval_sub() const { return item_.index() == 2; }
	LValAST* get_lval_sub() { return std::get<2>(item_); }
	const LValAST* get_lval_sub() const { return std::get<2>(item_); }

	void setExp(BaseAST* exp) { item_ = (ExpAST*)exp; }
	void setNumber(tyI32 num) { item_ = new NumberAST(num); }
	void setLval(BaseAST* lval) { item_ = (LValAST*)lval; }

  private:
	std::variant<ExpAST*, NumberAST*, LValAST*>	item_;
};


/*
	LVal          ::= IDENT
*/
class LValAST : public BaseAST {
public:
	LValAST(const char* ident) : ident_(ident) {}

	void Dump() const override { ident_.repr(); }
	void gen_ir(GenIRCfg* cfg) override;

	std::string repr() const { return ident_.get_ident(); }

private:
	VarAST ident_;
};


/*
	UnaryOp ::= "+" | "-" | "!"
*/
enum class OpAST {
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
	OP_END
};


// OpAST get_op_ast(const char *c);
inline OpAST get_op_ast(const char* op) {
    switch (*op) {
        case '-':   return OpAST::MINUS;
        case '+':   return OpAST::PLUS;
        case '!': {
			if (strcmp(op, "!") == 0) {
				return OpAST::LNOT;
			} else if (strcmp(op, "!=") == 0) {
				return OpAST::NEQ;
			}
			break;
		}
        case '*':   return OpAST::MUL;
        case '/':   return OpAST::DIV;
        case '%':   return OpAST::MOD;
		case '<': {
			if(strcmp(op, "<") == 0) {
				return OpAST::LT;
			} else if (strcmp(op, "<=") == 0) {
				return OpAST::LE;
			}
			break;
		}
		case '>': {
			if (strcmp(op, ">") == 0) {
				return OpAST::GT;
			} else if (strcmp(op, ">=") == 0) {
				return OpAST::GE;
			}
		}
		case '=': {
			if (strcmp(op, "==") == 0) {
				return OpAST::EQ;
			}
			break;
		}
		case '&': {
			if (strcmp(op, "&&") == 0) {
				return OpAST::LAND;
			}
			break;
		}
		case '|': {
			if (strcmp(op, "||") == 0) {
				return OpAST::LOR;
			}
			break;
		}
        default:    return OpAST::OP_END;
    }
	return OpAST::OP_END;
}

inline void dump_op_ast(OpAST op) {
    switch (op) {
        case OpAST::MINUS:  std::cout << "-"; break;
        case OpAST::PLUS:   std::cout << "+"; break;
        case OpAST::LNOT:   std::cout << "!"; break;
        case OpAST::MUL:    std::cout << "*"; break;
        case OpAST::DIV:    std::cout << "/"; break;
        case OpAST::MOD:    std::cout << "%"; break;
		case OpAST::LT:		std::cout << "<"; break;
		case OpAST::LE:		std::cout << "<="; break;
		case OpAST::GT:		std::cout << ">"; break;
		case OpAST::GE:		std::cout << ">="; break;
		case OpAST::EQ:		std::cout << "=="; break;
		case OpAST::NEQ:	std::cout << "!="; break;
		case OpAST::LAND:	std::cout << "&&"; break;
		case OpAST::LOR:	std::cout << "||"; break;
		case OpAST::OP_END: std::cout << "<BAD OP>"; break;
        default:    break;
    }
}

inline std::string repr_op_ast(OpAST op) {
	switch (op) {
        case OpAST::MINUS:	return "-";
        case OpAST::PLUS:	return "+";
        case OpAST::LNOT:	return "!";
        case OpAST::MUL:	return "*";
        case OpAST::DIV:	return "/";
        case OpAST::MOD:	return "%";
		case OpAST::LT:		return "<";
		case OpAST::LE:		return "<=";
		case OpAST::GT:		return ">";
		case OpAST::GE:		return ">=";
		case OpAST::EQ:		return "==";
		case OpAST::NEQ:	return "!=";
		case OpAST::LAND:	return "&&";
		case OpAST::LOR:	return "||";
		case OpAST::OP_END:
        default:			return "<BAD OP>";
    }
}
// void dump_op_ast(OpAST op);

/*
	UnaryExp ::= PrimaryExp | UnaryOp UnaryExp
*/
class UnaryExpAST : public ExpAST {
  public:
	STATIC_TYPE_ID_DECL(UnaryExpAST);
	UnaryExpAST() { SET_TYPE_ID(UnaryExpAST); }

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;
	tyI32 try_eval() const override;
	std::string repr() const override {
		if (sub_is_primary()) {
			return get_primary_exp()->repr();
		} else if (sub_is_unary()) {
			return repr_op_ast(get_unary_op()) + get_unary_exp()->repr();
		}
		return "";
	}

  public:
	// ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);
	void gen_unary_exp_ir(GenIRCfg* cfg);
	void gen_primary_exp_ir(GenIRCfg* cfg);


	bool sub_is_primary() const { return subExp_.index() == 0; }
	void setPrimaryExp(BaseAST* exp) { subExp_ = (PrimaryExpAST*)exp; }
	PrimaryExpAST* get_primary_exp() { return std::get<0>(subExp_); }
	const PrimaryExpAST* get_primary_exp() const { return std::get<0>(subExp_); }

	bool sub_is_unary() const { return subExp_.index() == 1; }
	void setUnaryOp(const char* op) {
		if (subExp_.index() != 1)
			subExp_.emplace<1>();
		std::get<1>(subExp_).op_ = get_op_ast(op);
	}
	OpAST get_unary_op() const { return std::get<1>(subExp_).op_; }
	void setUnaryExp(BaseAST* exp) { 
		if (subExp_.index() != 1)
			subExp_.emplace<1>();
		std::get<1>(subExp_).exp_ = (UnaryExpAST*)exp;
	}
	UnaryExpAST* get_unary_exp() { return std::get<1>(subExp_).exp_; }
	const UnaryExpAST* get_unary_exp() const { return std::get<1>(subExp_).exp_; }
 
  private:
	struct SubUnaryExpAST{
		OpAST			op_		{ OpAST::OP_END };
		UnaryExpAST		*exp_	{ nullptr };
	};

	std::variant<PrimaryExpAST*, SubUnaryExpAST> subExp_;
};


#define NEW_BINARY_EXP(T) auto* exp = new BinaryExpAST();\
	exp->set_type_id(ObjType::T);\
	return exp;\

/*
	MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp
	AddExp ::= MulExp | AddExp ( "+" | "-" ) MulExp
	RelExp ::= AddExp | RelExp ( '<' | '>' | '<=' | '>=' ) AddExp
	EqExp ::= RelExp | EqExp ("==" | "!=") RelExp;
	LAndExp ::= EqExp | LAndExp "&&" EqExp;
	LOrExp ::= LAndExp | LOrExp "||" LAndExp;
*/
class BinaryExpAST : public ExpAST {
  public:
	static BinaryExpAST* create_mul_exp()	{ NEW_BINARY_EXP(MulExpAST); }
	static BinaryExpAST* create_add_exp()	{ NEW_BINARY_EXP(AddExpAST); }
	static BinaryExpAST* create_rel_exp()	{ NEW_BINARY_EXP(RelExpAST); }
	static BinaryExpAST* create_eq_exp()	{ NEW_BINARY_EXP(EqExpAST); }
	static BinaryExpAST* create_land_exp()	{ NEW_BINARY_EXP(LAndExpAST); }
	static BinaryExpAST* create_lor_exp()	{ NEW_BINARY_EXP(LOrExpAST); }

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;
	tyI32 try_eval() const override;
	std::string repr() const override;

  public:
	bool is_other_exp() const { return subExp_.index() == 0; }
	void set_other_exp(BaseAST* exp) { subExp_ = (ExpAST*)exp; }
	ExpAST* get_other_exp() { return std::get<0>(subExp_); }
	const ExpAST* get_other_exp() const { return std::get<0>(subExp_); }

	void set_binary_opnd1(BaseAST* exp) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).opnd1_ = (ExpAST*)exp;
	}
	void set_binary_opnd2(BaseAST* exp) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).opnd2_ = (ExpAST*)exp;
	}
	void set_binary_op(const char* c) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).op_ = get_op_ast(c);
	}
	bool is_binary_exp() const { return subExp_.index() == 1; }
	ExpAST* get_binary_opnd1() { return std::get<1>(subExp_).opnd1_; }
	const ExpAST* get_binary_opnd1() const { return std::get<1>(subExp_).opnd1_; }
	ExpAST* get_binary_opnd2() { return std::get<1>(subExp_).opnd2_; }
	const ExpAST* get_binary_opnd2() const { return std::get<1>(subExp_).opnd2_; }
	OpAST get_binary_op() const { return std::get<1>(subExp_).op_; }

  private:
	BinaryExpAST() { }
	void set_mul_exp_type() { SET_TYPE_ID(MulExpAST); }

  private:
	struct SubBinaryExpAST {
		OpAST		op_		{ OpAST::OP_END };
		ExpAST*	opnd1_	{ nullptr };
		ExpAST*	opnd2_	{ nullptr };
	};

	std::variant<ExpAST*, SubBinaryExpAST> subExp_;
};


/*
	ConstExp      ::= Exp;
*/
class ConstExpAST : public ExpAST {
  public:
	STATIC_TYPE_ID_DECL(ConstExpAST);
	ConstExpAST(BaseAST* ast): exp_((ExpAST*)ast) { SET_TYPE_ID(ConstExpAST); }

  public:
	void Dump() const override { exp_->Dump(); }
	void gen_ir(GenIRCfg* cfg) override { /* do nothing */ }
	std::string repr() const override { return exp_->repr(); }
	tyI32 try_eval() const override { return exp_->try_eval(); }

  private:
	ExpAST* exp_ { nullptr };
};

/*
	InitVal       ::= Exp
*/
class InitValAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(InitValAST);
	InitValAST(BaseAST* ast)
		: exp_((ExpAST*)ast) { SET_TYPE_ID(InitValAST); }

public:
	void Dump() const override { exp_->Dump(); }
	void gen_ir(GenIRCfg* cfg) override { exp_->gen_ir(cfg); }

private:
	ExpAST* exp_ { nullptr };
};


class ASTExpEvalFailed : public std::exception {
  public:
	explicit ASTExpEvalFailed(const std::string& msg) : msg_(msg) {}

	const char* what() const noexcept override {
		return msg_.c_str();
	}

  private:
	std::string msg_;
};
