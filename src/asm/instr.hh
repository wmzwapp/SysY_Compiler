#pragma once

#include "var.hh"

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <type_traits>

enum class InstrOp {
	LI,				// <li rd, imm> : 将立即数 imm 加载到寄存器 rd 中.
	XOR,			// <xor rd, rs1, rs2> : 计算 rs1 寄存器和 rs2 寄存器 (xor) 按位异或的值, 存入 rd 寄存器.
	XORI,			// <xori rd, rs1, imm12> : 计算 rs1 寄存器和 imm12 (xori) 按位异或的值, 存入 rd 寄存器.
	SEQZ,			// <seqz rd, rs> : 判断 rs 寄存器是否等于 (seqz) 0, 如果判断条件成立, 则将 1 写入 rd 寄存器, 否则写入 0
	SNEZ,			// <snez rd, rs> : 判断 rs 寄存器是否不等于 (snez) 0, 如果判断条件成立, 则将 1 写入 rd 寄存器, 否则写入 0
	SUB,			// <sub rd, rs1, rs2> : 计算 rs1 寄存器和 rs2 寄存器相减的值, 存入 rd 寄存器
	MV,				// <mv rd, rs> : 将寄存器 rs 的值复制到寄存器 rd
	RET,			// <ret> : 无条件转移到 ra 寄存器中保存的地址处
	MUL,			// <mul rd, rs1, rs2> : 计算寄存器 rs1 和寄存器 rs2 相乘 (mul) 的值, 存入 rd 寄存器.
	DIV,			// <div rd, rs1, rs2> : 计算寄存器 rs1 除以 (div) 寄存器 rs2 的值, 存入 rd 寄存器.
	REM,			// <rem rd, rs1, rs2> : 计算寄存器 rs1 和寄存器 rs2 取余 (rem)的值, 存入 rd 寄存器.
	ADD,			// <add rd, rs1, rs2> : 计算 rs1 寄存器和 rs2 寄存器 (add) 相加的值, 存入 rd 寄存器.
	ADDI,			// <add rd, rs1, imm12> : 计算 rs1 寄存器和 imm12 (add) 相加的值, 存入 rd 寄存器.
	SLT,			// <slt rd, rs1, rs2> : 判断 rs1 寄存器是否小于 (slt) rs2 寄存器, 如果判断条件成立, 则将 1 写入 rd 寄存器, 否则写入 0.
	SGT,			// <sgt rd, rs1, rs2> : 判断 rs1 寄存器是否大于 (sgt) rs2 寄存器, 如果判断条件成立, 则将 1 写入 rd 寄存器, 否则写入 0.
	AND,			// <and rd, rs1, rs2> : 计算 rs1 寄存器和 rs2 寄存器 (and) 按位与的值, 存入 rd 寄存器.
	ANDI,			// <andi rd, rs1, imm12> : 计算 rs1 寄存器和 imm12 (andi) 按位与的值, 存入 rd 寄存器.
	OR,				// <or rd, rs1, rs2> : 计算 rs1 寄存器和 rs2 寄存器 (or)按位或的值, 存入 rd 寄存器.
	ORI,			// <ori rd, rs1, imm12> : 计算 rs1 寄存器和 imm12 (ori) 按位或的值, 存入 rd 寄存器.
};


class Instruction {
  public:
	Instruction() {
		static uint64_t count_ {0};
		debugId_ = count_++;
	}

  public:
	virtual VarASM* get_ret() { return nullptr; }
	virtual void dump(std::ostream& os) = 0;
	std::string p() { std::stringstream ss; dump(ss); return ss.str(); }

	void setPrev(Instruction* i) { prev_ = i; }
	void setNext(Instruction* i) { next_ = i; }

	inline std::string_view dumpOp(InstrOp op);

  private:
	Instruction*	prev_ { nullptr };
	Instruction*	next_ { nullptr };
	uint64_t		debugId_;
};


class Instr0 : public Instruction {
  public:
	Instr0(InstrOp op) : Instruction(), op_(op) {}

	void dump(std::ostream& os) override { os << dumpOp(op_); }

  private:
	InstrOp		op_;
};


template<typename T1>
class Instr2 : public Instruction {
  public:
	Instr2(InstrOp op, VarASM* ret, T1 opnd):
		Instruction(), op_(op), ret_(ret), opnd1_(opnd) {}

	void dump(std::ostream& os) override {
		os << std::left;
		os << std::setw(7) << dumpOp(op_);
		os << std::setw(4) << *ret_ << ", ";
		if constexpr (std::is_pointer_v<T1>) {
			os << std::setw(4) << *opnd1_;
		} else {
			os << std::setw(4) << opnd1_;
		}
		os << std::right;
	}

	VarASM* get_ret() override { return ret_; }

  private:
	InstrOp     op_;
	VarASM*     ret_;
	T1          opnd1_;
};


template<typename T1, typename T2>
class Instr3: public Instruction {
  public:
	Instr3(InstrOp op, VarASM* ret, T1 opnd1, T2 opnd2):
		Instruction(), op_(op), ret_(ret), opnd1_(opnd1), opnd2_(opnd2) {}

	void dump(std::ostream& os) override {
		os << std::left;
		os << std::setw(7) << dumpOp(op_);
		os << std::setw(4) << *ret_ << ", ";
		static_assert(std::is_pointer_v<T1>, "None RegVar type var was used as destination value.");
		os << std::setw(4) << *opnd1_ << ", ";
		if constexpr (std::is_pointer_v<T2>) {
			os << std::setw(4) << *opnd2_;
		} else {
			os << std::setw(4) << opnd2_;
		}
		os << std::right;
	}

	VarASM* get_ret() override { return ret_; }

  private:
	InstrOp     op_;
	VarASM*     ret_;
	T1          opnd1_;
	T2          opnd2_;
};

using Instr2RI = Instr2<uint32_t>;
using Instr2RR = Instr2<VarASM*>;
using Instr3RRR = Instr3<VarASM*, VarASM*>;
using Instr3RRI = Instr3<VarASM*, uint32_t>;


std::string_view Instruction::dumpOp(InstrOp op) {
	switch (op) {
		case InstrOp::LI : return "li";
		case InstrOp::XOR: return "xor";
		case InstrOp::XORI: return "xori";
		case InstrOp::SEQZ: return "seqz";
		case InstrOp::SNEZ: return "snez";
		case InstrOp::SUB: return "sub";
		case InstrOp::MV: return "mv";
		case InstrOp::RET: return "ret";
		case InstrOp::MUL: return "mul";
		case InstrOp::DIV: return "div";
		case InstrOp::REM: return "rem";
		case InstrOp::ADD: return "add";
		case InstrOp::ADDI: return "addi";
		case InstrOp::SLT: return "slt";
		case InstrOp::SGT: return "sgt";
		case InstrOp::AND: return "and";
		case InstrOp::ANDI: return "andi";
		case InstrOp::OR: return "or";
		case InstrOp::ORI: return "ori";
		default:
			return "";
	}
}