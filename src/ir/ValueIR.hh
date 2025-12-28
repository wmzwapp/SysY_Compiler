#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"


class VarASM;

class ValueIR : public IRBase {
public:
	ValueIR(TypeIR* ty) : IRBase(ty) {}
};


class ValueIntIR : public ValueIR {
public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::ValueIntIR };
	ValueIntIR(int v) : ValueIR(mmpool_.make<TypeIntIR>()), value_(v) { SET_TYPE_ID(ValueIntIR); }

public:
	void dump(std::ostream& os) const override { os << value_; }
	std::string repr() const override { return std::to_string(value_); }
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	
public:
	int value_ { 0 };
};


class SymbolIR : public ValueIR {
public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::SymbolIR };
	SymbolIR(std::string sym, TypeIR* ty, bool isTemp = false):
		ValueIR(ty),
		isTemp_(isTemp), sym_(sym)
		{ SET_TYPE_ID(SymbolIR); }

public:
	void dump(std::ostream& os) const override {
		os << (isTemp_ ? '%' : '@') << sym_;
	}
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
