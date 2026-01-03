#pragma once

#include "BaseIR.hh"
#include <vector>

namespace IR {

class InstrBase;

class Block : public BaseIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::BlockIR };
	Block(std::string sym): BaseIR(), sym_(std::move(sym)) { SET_TYPE_ID(BlockIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	void add_instr(InstrBase* instr) { Instrs_.push_back(instr); }
	void add_end_instr(InstrBase* stmt) { endInstr_ = stmt; }

	std::vector<InstrBase*>& get_instrs() { return Instrs_; }
	InstrBase* get_end_instr() { return endInstr_; }

private:
	std::vector<InstrBase*> Instrs_;
	InstrBase* endInstr_ { nullptr };
	std::string sym_;
};

}