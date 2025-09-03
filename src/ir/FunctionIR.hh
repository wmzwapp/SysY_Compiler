#pragma once

#include "BaseIR.hh"
#include "BlockIR.hh"
#include "ValueIR.hh"
#include "../asm/cfg.hh"

#include <vector>


class FunctionIR : public IRBase {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::FunctionIR };

	FunctionIR(SymbolIR* funcSym): sym_(funcSym) { SET_TYPE_ID(FunctionIR); }

  public:
	void dump(std::ostream& os) const override;

  public:
	void appendBB(BlockIR* bb) { bbs_.push_back(bb); }

	SymbolIR* getATmpSymbol() {
		auto* sym = new SymbolIR(tmpVars.size());
		tmpVars.push_back(sym);
		return sym;
	}

	SymbolIR* getSym() { return sym_; }
	std::string getFuncName() { return sym_->getSym(); }

	void gen_asm(ConfigASM* asmer);

  private:
	std::vector<BlockIR*> bbs_;
	SymbolIR* sym_ { nullptr };
	ValueIR* retValue_ { nullptr };

	std::vector<SymbolIR*> tmpVars;
};
