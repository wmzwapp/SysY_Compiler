#pragma once

#include "asm/var.hh"
#include "instr.hh"

#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

class FuncASM;
class BasicBlockASM;

class TOPASM {
  public:
	void dump(std::ostream& os);

	FuncASM* create_func(std::string funcName);

  private:
	std::vector<FuncASM*> funcs_;
};

static TOPASM __ASMER__;

class FuncASM {
  public:
	FuncASM(std::string sym);

  public:
	void dump(std::ostream& os);

	BasicBlockASM* create_bb();

	BasicBlockASM* get_current_bb() { return curbb_; }

	std::string get_entry_label();

  private:
	BasicBlockASM* curbb_ { nullptr };
	std::vector<BasicBlockASM*> bbs_;
};


inline std::unordered_map<std::string, VarASM*> _reservedRegVarMap_ {
	{ "x0", new VarASM("x0") },
	{ "a0", new VarASM("a0") },
	{"sp", new VarASM("sp")},
};


class BasicBlockASM {
  public:
	BasicBlockASM(std::string sym) : sym_(sym) {}

  public:
	void dump(std::ostream& os);

	Instruction* get_current_instr() { return currentInstr_; }
	std::vector<Instruction*>& get_instrs() { return instrs_; }
	void push_back_instr(Instruction* instr) {
		if (currentInstr_ != nullptr) {
			currentInstr_->setNext(instr);
			instr->setPrev(currentInstr_);
		} else {
			instr->setPrev(nullptr);
		}
		instr->setNext(nullptr);
		instrs_.push_back(instr);
		currentInstr_ = instr;
	}

	template<typename T, typename... Args>
	T* create_instr(Args&&... args) {
		auto* instr = new T(std::forward<Args>(args)...);
		push_back_instr(instr);
		return instr;
	}

	static VarASM* get_temp_var(int idx = -1) {
		static uint32_t count_;
		std::string sym;
		if (idx >= 0) {
			sym = std::string("t") + std::to_string(idx);
		} else {
			sym = std::string("t") + std::to_string(count_++);
		}
		if (_reservedRegVarMap_.find(sym) == _reservedRegVarMap_.end()) {
			auto* tmp = new VarASM(sym, true);
			_reservedRegVarMap_[sym] = tmp;
			return tmp;
		} else {
			return _reservedRegVarMap_[sym];
		}
	}

	static VarASM* get_reg_var_x0() { return _reservedRegVarMap_.at("x0"); }
	static VarASM* get_reg_var_a0() { return _reservedRegVarMap_.at("a0"); }
	static VarASM* get_reg_var_sp() { return _reservedRegVarMap_.at("sp"); }

	std::string get_label() { return sym_; }

  private:
	std::string sym_;
	Instruction* currentInstr_ { nullptr };
	std::vector<Instruction*> instrs_;
};

