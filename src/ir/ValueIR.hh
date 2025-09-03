#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"
#include "asm/var.hh"
#include <cstdint>


class ValueIR : public IRBase {
  public:
	TypeIR* getType() const { return type_; }
	void setType(TypeIR* ty) { type_ = ty; }

  public:
	TypeIR* type_ { nullptr };
};


class ValueIntIR : public ValueIR {
  public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::ValueIntIR };

	ValueIntIR(TypeIntIR* ty, int v) : value_(v) {
		SET_TYPE_ID(ValueIntIR);
		type_ = ty;
	}

  public:
	void dump(std::ostream& os) const override { os << value_; }

  public:
	int value_ { 0 };
};


class SymbolIR : public ValueIR {
  public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::SymbolIR };

	SymbolIR(std::string sym) : isTemp_(false), sym_(sym) { SET_TYPE_ID(SymbolIR); }
	SymbolIR(uint64_t idx): isTemp_(true), sym_(std::to_string(idx)) { SET_TYPE_ID(SymbolIR); }

  public:
	void dump(std::ostream& os) const {
		os << (isTemp_ ? '%' : '@') << sym_;
	}
	std::string getSym() { return sym_; }

	void set_reg_var(VarASM* var) { regVar_ = var; }
	VarASM* get_reg_var() { return regVar_; }

  private:
	bool isTemp_ { false };
	std::string sym_;
	VarASM* regVar_ { nullptr };
};
