#pragma once

#include "BaseIR.hh"
#include "BlockIR.hh"
#include "ValueIR.hh"
// #include "../asm/cfg.hh"

#include <cstdint>
#include <string>
#include <vector>


class FunctionIR : public IRBase {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::FunctionIR };

	FunctionIR(SymbolIR* funcSym, TypeFuncIR* ty): IRBase(ty), sym_(funcSym) { SET_TYPE_ID(FunctionIR); }

  public:
	void dump(std::ostream& os) const override;

  public:
	void appendBB(BlockIR* bb) { bbs_.push_back(bb); }

	SymbolIR* getATmpSymbol(TypeIR* ty) {
		auto tmpSym = std::string("t") + std::to_string(tmpVars.size());
		auto* sym = SymbolIR::create_named_var(tmpSym, ty, true);
		tmpVars.push_back(sym);
		return sym;
	}

	SymbolIR* getSym() { return sym_; }
	std::string getFuncName() { return sym_->getSym(); }

	void gen_asm(GenASMCfg* cfg) override;
	void gen_asm_prologue(BasicBlockASM* bb);
	void gen_asm_epilogue(BasicBlockASM* bb);

  private:
	std::vector<BlockIR*> bbs_;
	SymbolIR* sym_ { nullptr };
	uint32_t stackSize_ { 0 };

	std::vector<SymbolIR*> tmpVars;
};
