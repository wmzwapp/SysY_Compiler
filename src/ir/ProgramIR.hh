#pragma once

#include "BaseIR.hh"

namespace IR {

class Program : public BaseIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::ProgramIR };
	Program(): BaseIR() { SET_TYPE_ID(ProgramIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	
public:
	void add_func(Function* func) { funcs_.push_back(func); }
	auto& get_funcs() { return funcs_; }

private:
	std::vector<Function*> funcs_;
};

}	// namespace IR

inline IR::Program __IR_TOP__;