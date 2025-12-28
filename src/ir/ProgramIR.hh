#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"


class ProgramIR : public IRBase {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::ProgramIR };
	ProgramIR(): IRBase(mmpool_.make<TypeUnitIR>()) { SET_TYPE_ID(ProgramIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }
	
public:
	void add_func(FunctionIR* func) { funcs_.push_back(func); }
	auto& get_funcs() { return funcs_; }

private:
	std::vector<FunctionIR*> funcs_;
};

inline ProgramIR __IR_TOP__;