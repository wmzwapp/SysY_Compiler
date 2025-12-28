#pragma once

#include "common/mmp.hh"

#include <ostream>
#include <string>
#include <vector>

class FuncASM;
class BasicBlockASM;
class Instruction;

class TOPASM {
public:
	void dump(std::ostream& os);
	void add_func(FuncASM* func) { funcs_.push_back(func); }

private:
	std::vector<FuncASM*> funcs_;
};

inline TOPASM __ASMER__;

class FuncASM {
public:
	void dump(std::ostream& os);

	auto& get_BBs() { return bbs_; }
	void add_BB(BasicBlockASM* bb) { bbs_.push_back(bb); }

	void set_stack_size(unsigned int sz) { stackSize_ = sz; }
	auto get_stack_size() { return stackSize_; }

	std::string get_entry_label();

private:
	unsigned int stackSize_ { 0 };
	std::vector<BasicBlockASM*> bbs_;
};


class BasicBlockASM {
public:
	BasicBlockASM(std::string label) : label_(label) {}

public:
	void dump(std::ostream& os);

	Instruction* get_current_instr() { return currentInstr_; }
	std::vector<Instruction*>& get_instrs() { return instrs_; }

	void push_back_instr(Instruction* instr);

	template<typename T, typename... Args>
	T* create_instr(Args&&... args) {
		auto* instr = mmpool_.make<T>(std::forward<Args>(args)...);
		push_back_instr(instr);
		return instr;
	}

	std::string get_label() { return label_; }

  private:
	std::string label_;
	Instruction* currentInstr_ { nullptr };
	std::vector<Instruction*> instrs_;
};

