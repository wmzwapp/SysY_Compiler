#pragma once

#include "BaseIR.hh"
#include <ostream>


namespace IR {

class Type;

class InstrBase : public BaseIR {
public:
	InstrBase() : BaseIR() {
		static int count_;
		debugId_ = count_ ++;
	}

public:
	virtual bool isReturn() const { return false; }
	virtual bool isSymbolDef() const { return false; }
	virtual bool isBranch() const { return false; }
	virtual bool isJump() const { return false; }
	virtual bool isStore() const { return false; }
	virtual bool isFuncCall() const { return false; }

	bool is_end_instr() const { return isBranch() || isJump() || isReturn(); }

public:
	InstrBase* get_pre_instr() { return pre_; }	
	void set_pre_instr(InstrBase* instr) { pre_ = instr; }
	void insert_me_before(InstrBase* instr) {
		if (get_pre_instr()) {
			get_pre_instr()->set_next_instr(get_next_instr());
		}
		auto* bInstr = instr->get_pre_instr();
		instr->set_pre_instr(this);
		set_next_instr(instr);
		set_pre_instr(bInstr);
	}
	void insert_me_behind(InstrBase* instr) {
		if (get_pre_instr()) {
			get_pre_instr()->set_next_instr(get_next_instr());
		}
		auto* bInstr = instr->get_next_instr();
		instr->set_next_instr(this);
		set_pre_instr(instr);
		set_next_instr(bInstr);
	}

	InstrBase* get_next_instr() { return next_; }
	void set_next_instr(InstrBase* instr) { next_ = instr; }

public:
	InstrBase* pre_ { nullptr };
	InstrBase* next_ { nullptr };
	int debugId_;
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
	void dump(std::ostream& os) const override;
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
	void dump(std::ostream& os) const override;
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
	void dump(std::ostream& os) const override;
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
	void dump(std::ostream& os) const override;
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
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	auto* get_src() { return src_; }

private:
	Symbol*	src_	{ nullptr };
};

class InstrBr : public InstrBase {
	/*
		br %value, %trueB, %falseB
	*/
public:
	STATIC_TYPE_ID_DECL(InstrBrIR);
	InstrBr(Value* v, Block* t, Block* f)
		: InstrBase(), value_(v), trueB_(t), falseB_(f)
		{ SET_TYPE_ID(InstrBrIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	bool isBranch() const override { return true; }

public:
	auto* get_value() { return value_; }
	auto* get_true_branch() { return trueB_; }
	auto* get_false_branch() { return falseB_; }

private:
	Value* value_ { nullptr };
	Block* trueB_ { nullptr };
	Block* falseB_ { nullptr };
};

class InstrJump : public InstrBase {
	/*
		jump %branch
	*/
public:
	STATIC_TYPE_ID_DECL(InstrJumpIR);
	InstrJump(Block* b) : InstrBase(), branch_(b) { SET_TYPE_ID(InstrJumpIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	bool isJump() const override { return true; }

public:
	auto* get_branch() { return branch_; }

private:
	Block*	branch_ { nullptr };
};

}