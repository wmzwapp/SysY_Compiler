#pragma once

#include "BaseIR.hh"
#include "ValueIR.hh"
#include <ostream>


namespace IR {

class InstrBase : public BaseIR {
public:
	InstrBase() : BaseIR() {}

public:
	virtual bool isReturn() const { return false; }
	virtual bool isSymbolDef() const { return false; }
	virtual bool isBranch() const { return false; }
	virtual bool isJump() const { return false; }
	virtual bool isStore() const { return false; }
	virtual bool isFuncCall() const { return false; }

	bool isEndStatement() const { return isBranch() || isJump() || isReturn(); }

public:
	InstrBase* get_pre_instr() { return pre_; }	
	void set_pre_instr(InstrBase* instr) { pre_ = instr; }

	InstrBase* get_next_instr() { return next_; }
	void set_next_instr(InstrBase* instr) { next_ = instr; }

public:
	InstrBase* pre_ { nullptr };
	InstrBase* next_ { nullptr };
};

class InstrSymDef : public InstrBase {
public:
	InstrSymDef(Symbol* sym) : InstrBase(), def_(sym) { }
	bool isSymbolDef() const override { return true; }

	Symbol* get_def() { return def_; }
	const Symbol* get_def() const { return def_; }

protected:
	Symbol* def_ { nullptr };
};


class InstrRet : public InstrBase {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::InstrRetIR };
	InstrRet(Value* retV): InstrBase(), value_(retV) { SET_TYPE_ID(InstrRetIR); }

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
	Value* get_value() { return value_; }

private:
	Value* value_ { nullptr };
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

class InstrBExpr : public InstrSymDef {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::InstrBExprIR };

	InstrBExpr(BinaryOp op, Symbol* ret, Value* opnd1, Value* opnd2):
		InstrSymDef(ret),
		op_(op), opnd1_(opnd1), opnd2_(opnd2) {
		SET_TYPE_ID(InstrBExprIR);
	}

public:
	void dump(std::ostream& os) const override {
		os << "\t";
		def_->dump(os);
		os << " = ";
		dump_op(os);
		os << " ";
		opnd1_->dump(os);
		os << ", ";
		opnd2_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	
public:
	Value* get_opnd1() { return opnd1_; }
	Value* get_opnd2() { return opnd2_; }
	auto get_op() { return op_; }
	void dump_op(std::ostream& os) const;

private:
	BinaryOp	op_		{ BinaryOp::BAD };	// operator
	Value*	opnd1_	{ nullptr };		// operand1
	Value*	opnd2_	{ nullptr };		// operand2
};


class InstrAlloc : public InstrSymDef {
	/*
		%ret = alloc <type>
	*/
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::InstrAllocIR };
	InstrAlloc(Symbol* ret, Type* ty):
		InstrSymDef(ret), allocTy_(ty)
		{ SET_TYPE_ID(InstrAllocIR); }

public:
	void dump(std::ostream& os) const override {
		os << '\t';
		def_->dump(os);
		os << " = alloc ";
		allocTy_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	Type* get_alloc_ty() { return allocTy_; }

private:
	Type*	allocTy_ { nullptr };
};


class InstrStore : public InstrBase {
	/*
		store %src, %des
	*/
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::InstrStoreIR };
	InstrStore(Value* src, Symbol* des):
		InstrBase(),
		src_(src), des_(des)
		{ SET_TYPE_ID(InstrStoreIR); }

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
	Value* get_src() { return src_; }
	Symbol* get_des() { return des_; }

private:
	Value*	src_	{ nullptr };
	Symbol*	des_	{ nullptr };
};


class InstrLoad : public InstrSymDef {
	/*
		%def = load %src
	*/
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::InstrLoadIR };
	InstrLoad(Symbol* def, Symbol* src):
		InstrSymDef(def), src_(src)
		{ SET_TYPE_ID(InstrLoadIR); }

public:
	void dump(std::ostream& os) const override {
		os << '\t';
		def_->dump(os);
		os << " = load ";
		src_->dump(os);
		os << std::endl;
	}
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	Value* get_src() { return src_; }

private:
	Symbol*	src_	{ nullptr };
};

}