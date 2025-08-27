#pragma once

#include "BaseAST.hh"
#include "ir/BlockIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/ValueIR.hh"

#include <iostream>

class ExpAST;
class UnaryExpAST;
class NumberAST;
class AddExpAST;
class MulExpAST;

/*
	Stmt ::= "return" Exp ";";
*/
class StmtAST : public BaseAST {
  public:
	void Dump() const override;

  public:
	void GenIR(FunctionIR* func, BlockIR* bbIR);

	void setRetExp(BaseAST* exp) { retExp_ = (ExpAST*)exp; }

public:
	ExpAST*	retExp_ { nullptr };
};

/*
	Exp ::= AddExp;
*/
class ExpAST : public BaseAST {
  public:
	void Dump() const override;
	
  public:
	ValueIR* GenIR(FunctionIR* func, BlockIR* bb);
	// void setUnaryExp(BaseAST* exp) { unaryExp_ = (UnaryExpAST*)exp; }
	void set_add_exp(BaseAST* exp) { addExp_ = (AddExpAST*)exp; }

  private:
	// UnaryExpAST *unaryExp_	{ nullptr };
	AddExpAST* addExp_ { nullptr };
};

/*
	PrimaryExp ::= "(" Exp ")" | Number
*/
class PrimaryExpAST : public BaseAST {
  public:
	void Dump() const override;
	
  public:
	ValueIR* GenIR(FunctionIR* func, BlockIR* bb);

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
	ValueIR* GenIR() { return new ValueIntIR(getIntType(), intNum_); }

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
	OP_END
};

// OpAST get_op_ast(const char *c);
inline OpAST get_op_ast(const char* op) {
    switch (*op) {
        case '-':   return OpAST::MINUS;
        case '+':   return OpAST::PLUS;
        case '!':   return OpAST::LNOT;
        case '*':   return OpAST::MUL;
        case '/':   return OpAST::DIV;
        case '%':   return OpAST::MOD;
        default:    return OpAST::OP_END;
    }
}

inline void dump_op_ast(OpAST op) {
    switch (op) {
        case OpAST::MINUS:  std::cout << "-"; break;
        case OpAST::PLUS:   std::cout << "+"; break;
        case OpAST::LNOT:   std::cout << "!"; break;
        case OpAST::MUL:    std::cout << "*"; break;
        case OpAST::DIV:    std::cout << "/"; break;
        case OpAST::MOD:    std::cout << "%"; break;
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
	ValueIR* GenIR(FunctionIR* func, BlockIR* bb);
	ValueIR* GenUnaryExp(FunctionIR* func, BlockIR* bb);
	ValueIR* GenPrimaryExp(FunctionIR* func, BlockIR* bb);

	void DumpPrimaryExp() const;
	void DumpUnaryExp() const;

	void setPrimaryExp(BaseAST* exp) { pExp_ = (PrimaryExpAST*)exp; }
	void setUnaryOp(const char* op) { op_ = get_op_ast(op); }
	void setUnaryExp(BaseAST* exp) { unaryExp_ = (UnaryExpAST*)exp; }
 
  private:
	PrimaryExpAST	*pExp_		{ nullptr };
	OpAST			op_			{ OpAST::OP_END };
	UnaryExpAST		*unaryExp_	{ nullptr };
};


/*
	AddExp ::= MulExp | AddExp ( "+" | "-" ) MulExp
*/
class AddExpAST : public BaseAST {
  public:
	void Dump() const override;

  public:
	ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);

	void set_mul_exp(BaseAST* exp) { mulExp_ = (MulExpAST*)exp; }
	void set_add_exp(BaseAST* exp) { addExp_ = (AddExpAST*)exp; }
	void set_op(const char* c) { op_ = get_op_ast(c); }

  private:
	MulExpAST*	mulExp_	{ nullptr };
	AddExpAST*  addExp_	{ nullptr };
	OpAST		op_		{ OpAST::OP_END };
};


/*
	MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp
*/
class MulExpAST : public BaseAST {
public:
	void Dump() const override;

public:
	ValueIR* gen_ir(FunctionIR* func, BlockIR* bb);

	void set_unary_exp(BaseAST* exp) { unaryExp_ = (UnaryExpAST*)exp; }
	void set_mul_exp(BaseAST* exp) { mulExp_ = (MulExpAST*)exp; }
	void set_op(const char* c) { op_ = get_op_ast(c); }

private:
	UnaryExpAST*	unaryExp_	{ nullptr };
	MulExpAST*		mulExp_		{ nullptr };
	OpAST			op_			{ OpAST::OP_END };
};
