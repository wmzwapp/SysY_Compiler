#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"
#include "asm/var.hh"
#include <unordered_map>

class ValueIntIR;
class SymbolIR;

inline std::unordered_map<std::string, SymbolIR*> _gIRVarsMap_;

class ValueIR : public IRBase {
public:
	ValueIR(TypeIR* ty) : IRBase(ty) {}
	void gen_asm(GenASMCfg* cfg) override { /* do nothing */ }

};


class ValueIntIR : public ValueIR {
  public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::ValueIntIR };

	static ValueIntIR* create_num_var(int v) { return new ValueIntIR(v); }
	// static ValueIntIR* get_num_var(int v);

  private:
	ValueIntIR(int v) :
		ValueIR(new TypeIntIR()), value_(v)
		{ SET_TYPE_ID(ValueIntIR); }

  public:
	void dump(std::ostream& os) const override { os << value_; }

  public:
	int value_ { 0 };
};


class SymbolIR : public ValueIR {
  public:
	static inline constexpr ObjType TYPE_ID_ { ObjType::SymbolIR };


	static SymbolIR* create_named_var(std::string sym, TypeIR* ty, bool isTemp = false);
	static SymbolIR* get_named_var(std::string sym);

  private:
	SymbolIR(std::string sym, TypeIR* ty, bool isTemp = false):
		ValueIR(ty),
		isTemp_(isTemp), sym_(sym)
		{ SET_TYPE_ID(SymbolIR); }

  public:
	void dump(std::ostream& os) const {
		os << (isTemp_ ? '%' : '@') << sym_;
	}
	std::string getSym() { return sym_; }

  public:
	void set_reg_var(VarASM* var) { regVar_ = var; }
	VarASM* get_reg_var() { return regVar_; }

	void set_offset(unsigned offset) { stackOffset_ = offset; }
	unsigned get_offset() const { return stackOffset_; }

  private:
	bool isTemp_ { false };
	std::string sym_;
	VarASM* regVar_ { nullptr };
	unsigned	stackOffset_ { (unsigned)-1 };
};
