#pragma once

#include "BaseIR.hh"
#include "ValueIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include "asm/var.hh"
#include <ostream>


class StmtIR : public IRBase {
  public:
	virtual bool isReturn() const { return false; }
	virtual bool isSymboDef() const { return false; }
	virtual bool isBranch() const { return false; }
	virtual bool isJump() const { return false; }
	virtual bool isStore() const { return false; }
	virtual bool isFuncCall() const { return false; }

	bool isEndStatement() const { return isBranch() || isJump() || isReturn(); }

	virtual Instruction* gen_asm(BasicBlockASM*) { return nullptr; }
};


class StmtRetIR : public StmtIR {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::StmtRetIR };

	StmtRetIR(ValueIR* retV): value_(retV) { SET_TYPE_ID(StmtRetIR); }

  public:
	bool isReturn() const override { return true; }
	void dump(std::ostream& os) const override {
		os << "\tret ";
		if (value_ != nullptr) {
			value_->dump(os);
		}
		os << std::endl;
	}

	Instruction* gen_asm(BasicBlockASM* bb) override;

  private:
	ValueIR* value_ { nullptr };
};


enum class BinaryOp {
	NE,
	EQ,
	GT,
	LT,
	GE,
	LE,
	ADD,
	SUB,
	MUL,
	DIV,
	MOD,
	AND,
	OR,
	XOR,
	SHL,
	SHR,
	SAR,
	BAD
};


class StmtBinaryExprIR : public StmtIR {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::StmtBinaryExprIR };

	StmtBinaryExprIR(BinaryOp op, SymbolIR* ret, ValueIR* opnd1, ValueIR* opnd2):
		op_(op), result_(ret), opnd1_(opnd1), opnd2_(opnd2) {
		SET_TYPE_ID(StmtBinaryExprIR);
	}

  public:
	bool isSymboDef() const override { return true; }
	void dump(std::ostream& os) const override {
		os << "\t";
		result_->dump(os);
		os << " = ";
		dumpOp(os);
		os << " ";
		opnd1_->dump(os);
		os << ", ";
		opnd2_->dump(os);
		os << std::endl;
	}

	Instruction* gen_asm(BasicBlockASM* bb) override;

	inline void dumpOp(std::ostream& os) const;

  public:
	Instruction* gen_asm_eq(BasicBlockASM* bb);
	Instruction* gen_asm_ne(BasicBlockASM* bb);
	Instruction* gen_asm_sub(BasicBlockASM* bb);
	Instruction* gen_asm_add(BasicBlockASM* bb);
	Instruction* gen_asm_mul(BasicBlockASM* bb);
	Instruction* gen_asm_div(BasicBlockASM* bb);
	Instruction* gen_asm_mod(BasicBlockASM* bb);
	Instruction* gen_asm_lt(BasicBlockASM* bb);
	Instruction* gen_asm_gt(BasicBlockASM* bb);
	Instruction* gen_asm_le(BasicBlockASM* bb);
	Instruction* gen_asm_ge(BasicBlockASM* bb);
	Instruction* gen_asm_and(BasicBlockASM* bb);
	Instruction* gen_asm_or(BasicBlockASM* bb);

	// VarASM* i2r(BasicBlockASM* bb, uint32_t i) {
	// 	auto* reg = bb->get_temp_var();
	// 	bb->push_back_instr(new Instr2RI(InstrOp::LI, reg, i));
	// 	return reg;
	// }

	void set_ret_reg_var(VarASM* var) { result_->set_reg_var(var); }

  private:
	BinaryOp	op_		{ BinaryOp::BAD };	// operator
	SymbolIR*	result_	{ nullptr };
	ValueIR*	opnd1_	{ nullptr };		// operand1
	ValueIR*	opnd2_	{ nullptr };		// operand2
};


void StmtBinaryExprIR::dumpOp(std::ostream& os) const {
	switch (op_) {
		case BinaryOp::ADD: {
			os << "add";
			break;
		}
		case BinaryOp::AND: {
			os << "and";
			break;
		}
		case BinaryOp::DIV: {
			os << "div";
			break;
		}
		case BinaryOp::EQ: {
			os << "eq";
			break;
		}
		case BinaryOp::GE: {
			os << "ge";
			break;
		}
		case BinaryOp::GT: {
			os << "gt";
			break;
		}
		case BinaryOp::LE: {
			os << "le";
			break;
		}
		case BinaryOp::LT: {
			os << "lt";
			break;
		}
		case BinaryOp::MOD: {
			os << "mod";
			break;
		}
		case BinaryOp::MUL: {
			os << "mul";
			break;
		}
		case BinaryOp::NE: {
			os << "ne";
			break;
		}
		case BinaryOp::OR: {
			os << "or";
			break;
		}
		case BinaryOp::SAR: {
			os << "sar";
			break;
		}
		case BinaryOp::SHL: {
			os << "shl";
			break;
		}
		case BinaryOp::SHR: {
			os << "shr";
			break;
		}
		case BinaryOp::SUB: {
			os << "sub";
			break;
		}
		case BinaryOp::XOR: {
			os << "xor";
			break;
		}
		default: {
			os << "badop";
			break;
		}
 	}
}
