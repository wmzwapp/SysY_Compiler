#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"


namespace IR {

class Value : public BaseIR {
public:
	Value(Type* ty) : ty_(ty) {}

	Type* get_ty() { return ty_; }
	void set_ty(Type* ty) { ty_ = ty; }

private:
	Type* ty_ { nullptr };
};


class ValueInt : public Value {
public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::ValueIntIR };
	ValueInt(int v)
		: Value(mmpool_.make<TypeInt>()), value_(v)
		{ SET_TYPE_ID(ValueIntIR); }

public:
	void dump(std::ostream& os) const override { os << value_; }
	std::string repr() const override { return std::to_string(value_); }
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	
public:
	int value_ { 0 };
};


class Symbol : public Value {
public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::SymbolIR };
	Symbol(std::string sym, Type* ty, bool isTemp = false)
		: Value(ty), isTemp_(isTemp), sym_(sym)
		{ SET_TYPE_ID(SymbolIR); }

public:
	void dump(std::ostream& os) const override { os << (isTemp_ ? '%' : '@') << sym_; }
	std::string repr() const override { return sym_; }
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	void set_offset(unsigned offset) { stackOffset_ = offset; }
	unsigned get_offset() const { return stackOffset_; }

private:
	bool isTemp_ { false };
	std::string sym_;
	unsigned stackOffset_ { (unsigned)-1 };
};

class ValueUndef : public Value {
public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::ValueUndefIR };
	ValueUndef(Type* ty) : Value(ty) { SET_TYPE_ID(ValueUndefIR); }

public:
	void dump(std::ostream& os) const override { os << "undef"; }
	std::string repr() const override { return "undef"; }
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
};

}
