#pragma once

#include "common/utils.hh"
#include "ir/BlockIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/ProgramIR.hh"
#include "ir/ValueIR.hh"
#include <variant>


using tyI32 = int;

class GenIRCfg {
	ProgramIR*	curProgram_ { nullptr };
	FunctionIR*	curFunc_	{ nullptr };
	BlockIR*	curBlock_	{ nullptr };
	ValueIR*	curValue_	{ nullptr };

  public:
	ProgramIR* get_current_programIR() { return curProgram_; }
	void set_current_programIR(ProgramIR* ir) { curProgram_ = ir; }

	FunctionIR*	get_current_functionIR() { return curFunc_; }
	void set_current_functionIR(FunctionIR* ir) { curFunc_ = ir; }

	BlockIR* get_current_blockIR() { return curBlock_; }
	void set_current_blockIR(BlockIR* ir) { curBlock_ = ir; }

	ValueIR* get_current_value() { return curValue_; }
	void set_current_value(ValueIR* ir) { curValue_ = ir; }
};


class BaseAST {
	ObjType type_;
  public:
	virtual ~BaseAST() = default;

	void set_type_id(ObjType t) { type_ = t; }
	ObjType get_type_id() { return type_; }

  public:
	virtual void Dump() const = 0;
	virtual void gen_ir(GenIRCfg* cfg) = 0;
};


struct symTableValue {
	bool is_const() const { return v_.index() == 0; }
	bool is_var() const { return v_.index() == 1; }

	tyI32 get_const() { return std::get<0>(v_); }
	void* get_var() { return std::get<1>(v_); }

	std::variant<tyI32, void*> v_;
};


class SymTabAST {
  public:
	void add_sym(std::string sym, tyI32 val) { table_[sym] = {val}; }
	void add_sym(std::string sym, void* alloc) { table_[sym] = {alloc}; }

	bool has_sym(std::string sym) { return table_.find(sym) != table_.end(); }

	symTableValue& get_sym(std::string sym) { return table_.at(sym); }

	tyI32 get_const_val(std::string sym) { return table_.at(sym).get_const(); }
	void* get_var_alloc(std::string sym) { return table_.at(sym).get_var(); }

	static SymTabAST* get_tab() {
		static SymTabAST inst;
		return &inst;
	};

  private:
	SymTabAST() {}

  private:
	std::unordered_map<std::string, symTableValue> table_;
};

inline SymTabAST &gSymTable_ = *SymTabAST::get_tab(); 