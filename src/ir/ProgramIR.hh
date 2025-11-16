#pragma once

#include "BaseIR.hh"
#include "FunctionIR.hh"
#include "TypeIR.hh"


class ProgramIR : public IRBase {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::ProgramIR };

	ProgramIR(): IRBase(new TypeUnitIR()) { SET_TYPE_ID(ProgramIR); }

	void dump(std::ostream& os) const override;
	
  public:
	void gen_asm(GenASMCfg* cfg) override;
	void appendFunctionIR(FunctionIR* func) { funcs_.push_back(func); }

  private:
	// std::vector<ValueIR*>       	gValues_;
	std::vector<FunctionIR*>    	funcs_;
};

inline ProgramIR __IR_TOP__;