#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"

#include <string>
#include <vector>
#include "unordered_map"


class FunctionIR : public IRBase {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::FunctionIR };
	FunctionIR(SymbolIR* funcSym, TypeFuncIR* ty): IRBase(ty), sym_(funcSym) { SET_TYPE_ID(FunctionIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	void add_BB(BlockIR* bb) { bbs_.push_back(bb); }
	auto& get_BBs() { return bbs_; }

	SymbolIR* get_tmp_var(TypeIR* ty);

	void add_var(std::string sym, SymbolIR* var) { symTab_[sym] = var; }
	SymbolIR* get_var(std::string sym) { return symTab_.at(sym); }

	std::string get_func_name();

private:
	SymbolIR* sym_ { nullptr };
	std::vector<BlockIR*> bbs_;
	unsigned long tmpVarCount_ { 0 };
	std::unordered_map<std::string, SymbolIR*> symTab_;
};
