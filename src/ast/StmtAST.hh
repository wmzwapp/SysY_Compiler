#pragma once

#include "BaseAST.hh"
#include "common/utils.hh"
#include "ir/TypeIR.hh"
#include "ir/ValueIR.hh"

#include <cstring>
#include <iostream>
#include <variant>

class ExpAST;
class UnaryExpAST;
class NumberAST;
class BinaryExpAST;

/*
	Stmt ::= "return" Exp ";";
*/
class StmtAST : public BaseAST {
  public:
	void Dump() const override;

  public:
	void gen_ir(GenIRCfg* cfg) override;

	void setRetExp(BaseAST* exp) { retExp_ = (ExpAST*)exp; }

public:
	ExpAST*	retExp_ { nullptr };
};

/*
	Exp ::= LOrExp;
*/
class ExpAST : public BaseAST {
  public:
	void Dump() const override;
	
  public:
	void gen_ir(GenIRCfg* cfg) override;
	void set_exp(BaseAST* exp) { BinaryExp_ = (BinaryExpAST*)exp; }

  private:
	BinaryExpAST* BinaryExp_ { nullptr };
};

/*
	PrimaryExp ::= "(" Exp ")" | Number
*/
class PrimaryExpAST : public BaseAST {
  public:
	void Dump() const override;
	
  public:
	void gen_ir(GenIRCfg* cfg) override;

	void setExp(BaseAST* exp) { exp_ = (ExpAST*)exp; }
	void setNumber(BaseAST* num) { num_ = (NumberAST*)num; }

  private:
	ExpAST*		exp_	{ nullptr };
	NumberAST*	num_	{ nullptr };
};

/*
	Number ::= INT_CONST;
*/
class NumberAST : public BaseAST {
  public:
	void Dump() const override { }
	void gen_ir(GenIRCfg* cfg) override {
		auto* ir = new ValueIntIR(getIntType(), intNum_);
		cfg->set_return_value(ir);
	}
	// ValueIR* gen_ir() { return new ValueIntIR(getIntType(), intNum_); }

  public:
	int getValue() { return intNum_; }
	void setValue(int val) { intNum_ = val; }

  private:
	int intNum_ { 0 };
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
// void dump_op_ast(OpAST op);

/*
	UnaryExp ::= PrimaryExp | UnaryOp UnaryExp
*/
class UnaryExpAST : public BaseAST {
  public:
	void Dump() const override;

  public:
	void gen_ir(GenIRCfg* cfg) override;
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
	OpAST get_unary_op() { return std::get<1>(subExp_).op_; }
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
class BinaryExpAST : public BaseAST {
  public:
	static BinaryExpAST* create_mul_exp()	{ NEW_BINARY_EXP(MulExpAST); }
	static BinaryExpAST* create_add_exp()	{ NEW_BINARY_EXP(AddExpAST); }
	static BinaryExpAST* create_rel_exp()	{ NEW_BINARY_EXP(RelExpAST); }
	static BinaryExpAST* create_eq_exp()	{ NEW_BINARY_EXP(EqExpAST); }
	static BinaryExpAST* create_land_exp()	{ NEW_BINARY_EXP(LAndExpAST); }
	static BinaryExpAST* create_lor_exp()	{ NEW_BINARY_EXP(LOrExpAST); }

  public:
	void Dump() const override;

  public:
	void gen_ir(GenIRCfg* cfg) override;
	// ValueIR*	gen_ir(FunctionIR* func, BlockIR* bb);

	void set_other_exp(BaseAST* exp) { subExp_ = exp; }
	bool is_other_exp() const { return subExp_.index() == 0; }
	BaseAST* get_other_exp() { return std::get<0>(subExp_); }
	const BaseAST* get_other_exp() const { return std::get<0>(subExp_); }

	void set_binary_opnd1(BaseAST* exp) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).opnd1_ = exp;
	}
	void set_binary_opnd2(BaseAST* exp) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).opnd2_ = exp;
	}
	void set_binary_op(const char* c) {
		if (subExp_.index() == 0)
			subExp_.emplace<1>();
		std::get<1>(subExp_).op_ = get_op_ast(c);
	}
	bool is_binary_exp() const { return subExp_.index() == 1; }
	BaseAST* get_binary_opnd1() { return std::get<1>(subExp_).opnd1_; }
	const BaseAST* get_binary_opnd1() const { return std::get<1>(subExp_).opnd1_; }
	BaseAST* get_binary_opnd2() { return std::get<1>(subExp_).opnd2_; }
	const BaseAST* get_binary_opnd2() const { return std::get<1>(subExp_).opnd2_; }
	OpAST get_binary_op() const { return std::get<1>(subExp_).op_; }

  private:
	BinaryExpAST() { }
	void set_mul_exp_type() { SET_TYPE_ID(MulExpAST); }

  private:
	struct SubBinaryExpAST {
		OpAST		op_		{ OpAST::OP_END };
		BaseAST*	opnd1_	{ nullptr };
		BaseAST*	opnd2_	{ nullptr };
	};

	std::variant<BaseAST*, SubBinaryExpAST> subExp_;
};


/*
	MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp
*/
// class MulExpAST : public BaseAST {
// public:
// 	void Dump() const override;

// public:
// 	ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);

// 	void set_unary_exp(BaseAST* exp) { unaryExp_ = (UnaryExpAST*)exp; }
// 	void set_mul_exp(BaseAST* exp) { mulExp_ = (MulExpAST*)exp; }
// 	void set_op(const char* c) { op_ = get_op_ast(c); }

// private:
// 	UnaryExpAST*	unaryExp_	{ nullptr };
// 	MulExpAST*		mulExp_		{ nullptr };
// 	OpAST			op_			{ OpAST::OP_END };
// };


/*
	AddExp ::= MulExp | AddExp ( "+" | "-" ) MulExp
*/
// class AddExpAST : public BaseAST {
//   public:
// 	void Dump() const override;

//   public:
// 	ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);

// 	void set_mul_exp(BaseAST* exp) { mulExp_ = (MulExpAST*)exp; }
// 	void set_add_exp(BaseAST* exp) { addExp_ = (AddExpAST*)exp; }
// 	void set_op(const char* c) { op_ = get_op_ast(c); }

//   private:
// 	MulExpAST*	mulExp_	{ nullptr };
// 	AddExpAST*  addExp_	{ nullptr };
// 	OpAST		op_		{ OpAST::OP_END };
// };


/*
	RelExp ::= AddExp | RelExp ( '<' | '>' | '<=' | '>=' ) AddExp
*/
// class RelExpAST : public BaseAST {
// public:
// 	void Dump() const override;

// public:
// 	ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);

// 	void set_add_exp(BaseAST* exp) { addExp_ = (AddExpAST*)exp; }
// 	void set_rel_exp(BaseAST* exp) { relExp_ = (RelExpAST*)exp; }
// 	void set_op(const char* c) { op_ = get_op_ast(c); }

// private:
// 	AddExpAST*		addExp_		{ nullptr };
// 	RelExpAST*		relExp_		{ nullptr };
// 	OpAST			op_			{ OpAST::OP_END };
// };
