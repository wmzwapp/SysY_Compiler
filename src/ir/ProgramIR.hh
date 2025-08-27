#pragma once

#include "BaseIR.hh"
#include "FunctionIR.hh"
#include "ValueIR.hh"
#include "../asm/cfg.hh"


class ProgramIR : public IRBase {
  public:
	inline static constexpr IRObjType TYPE_ID_ { IRObjType::ProgramIR };

	ProgramIR() { IR_SET_TYPE(ProgramIR); }
	
	void dump(std::ostream& os) const override;
	
  public:
	void gen_asm(ConfigASM* asmer);
	void appendFunctionIR(FunctionIR* func) { funcs_.push_back(func); }

  private:
	std::vector<ValueIR*>       	gValues_;
	std::vector<FunctionIR*>    	funcs_;
};

inline ProgramIR __IR_TOP__;