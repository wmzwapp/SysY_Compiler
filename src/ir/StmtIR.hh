#pragma once

#include "BaseIR.hh"
#include "ValueIR.hh"
#include <ostream>


class StmtIR : public IRBase {
public:
	StmtIR(TypeIR* ir) : IRBase(ir) {}

	virtual bool isReturn() const { return false; }
	virtual bool isSymbolDef() const { return false; }
	virtual bool isBranch() const { return false; }
	virtual bool isJump() const { return false; }
	virtual bool isStore() const { return false; }
	virtual bool isFuncCall() const { return false; }

	bool isEndStatement() const { return isBranch() || isJump() || isReturn(); }

	virtual SymbolIR* get_def_var() { return nullptr; }
};


class StmtRetIR : public StmtIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::StmtRetIR };
	StmtRetIR(ValueIR* retV): StmtIR(mmpool_.make<TypeUnitIR>()), value_(retV) { SET_TYPE_ID(StmtRetIR); }

public:
	bool isReturn() const override { return true; }
	void dump(std::ostream& os) const override {
		os << "\tret ";
		if (value_ != nullptr) {
			value_->dump(os);
		}
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	ValueIR* get_value() { return value_; }

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
		StmtIR(mmpool_.make<TypeIntIR>()),
		op_(op), result_(ret), opnd1_(opnd1), opnd2_(opnd2) {
		SET_TYPE_ID(StmtBinaryExprIR);
	}

public:
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
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	SymbolIR* get_def_var() override { return result_; }

public:
	ValueIR* get_opnd1() { return opnd1_; }
	ValueIR* get_opnd2() { return opnd2_; }
	auto get_op() { return op_; }
	inline void dumpOp(std::ostream& os) const;

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


class AllocIR : public StmtIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::AllocIR };
	AllocIR(SymbolIR* ret, TypeIR* ty):
		StmtIR(ty), result_(ret)
		{ SET_TYPE_ID(AllocIR); }

public:
	bool isSymbolDef() const override { return true; }
	void dump(std::ostream& os) const override {
		os << '\t';
		result_->dump(os);
		os << " = alloc ";
		ty_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	SymbolIR* get_def_var() override { return result_; }

private:
	SymbolIR*	result_	{ nullptr };
};


class StoreIR : public StmtIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::StoreIR };
	StoreIR(ValueIR* src, SymbolIR* des):
		StmtIR(mmpool_.make<TypeUnitIR>()),
		des_(des), src_(src)
		{ SET_TYPE_ID(StoreIR); }

public:
	bool isStore() const override { return true; }
	void dump(std::ostream& os) const override {
		os << '\t';
		os << "store ";
		src_->dump(os);
		os << ", ";
		des_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	SymbolIR* get_des() { return des_; }
	ValueIR* get_src() { return src_; }

private:
	SymbolIR*	des_	{ nullptr };
	ValueIR*	src_	{ nullptr };
};


class LoadIR : public StmtIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::LoadIR };
	LoadIR(SymbolIR* des, SymbolIR* src, TypeIR* ty):
		StmtIR(ty),
		des_(des), src_(src)
		{ SET_TYPE_ID(LoadIR); }

public:
	bool isStore() const override { return true; }
	void dump(std::ostream& os) const override {
		os << '\t';
		des_->dump(os);
		os << " = load ";
		src_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	SymbolIR* get_def_var() override { return des_; }

public:
	SymbolIR* get_des() { return des_; }
	ValueIR* get_src() { return src_; }

private:
	SymbolIR*	des_	{ nullptr };
	SymbolIR*	src_	{ nullptr };
};