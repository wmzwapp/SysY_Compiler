#pragma once

#include "BaseIR.hh"
#include "InstrIR.hh"
#include <vector>

namespace IR {

// class InstrBase;

class Block : public BaseIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::BlockIR };
	Block(Symbol* sym): BaseIR(), sym_(sym) { SET_TYPE_ID(BlockIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	// void add_instr(InstrBase* instr) { Instrs_.push_back(instr); }
	template<typename T, typename ... Args>
	T* create_instr(Args&&... args) {
		auto* instr = mmpool_.make<T>(std::forward<Args>(args)...);
		if (curInstr_ == nullptr) {
			entryInstr_ = instr;
		} else {
			curInstr_->set_next_instr(instr);
		}
		instr->set_pre_instr(curInstr_);
		Instrs_.push_back(instr);
		curInstr_ = instr;
		if (instr->is_end_instr()) {
			isTerminated_ = true;
		}
		return instr;
	}
	// void add_end_instr(InstrBase* stmt) { endInstr_ = stmt; }

	std::vector<InstrBase*>& get_instrs() { return Instrs_; }
	InstrBase* get_entry_instr() { return entryInstr_; }
	auto* get_current_instr() { return curInstr_; }
	void set_entry_instr(InstrBase* instr) { entryInstr_ = instr; }
	void set_current_instr(InstrBase* instr) { curInstr_ = instr; }
	// InstrBase* get_end_instr() { return endInstr_; }

	void add_next_block(Block* bb) { next_.push_back(bb); }
	auto& get_next_blocks() { return next_; }

	void add_pre_block(Block* bb) { pre_.push_back(bb); }
	auto& get_pre_blocks() { return pre_; }

	auto* get_symbol() { return sym_; }

	void set_terminated(bool v) { isTerminated_ = v; }
	bool is_terminated() const noexcept { return isTerminated_; }

private:
	std::vector<InstrBase*> Instrs_;
	InstrBase* entryInstr_ { nullptr };
	InstrBase* curInstr_ { nullptr };
	// InstrBase* endInstr_ { nullptr };
	std::vector<Block*> pre_;
	std::vector<Block*> next_;
	Symbol* sym_;
	bool isTerminated_ { false };
};

}