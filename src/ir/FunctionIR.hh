#pragma once

#include "BaseIR.hh"

#include <string>
#include <vector>
#include "unordered_map"

namespace IR {

class Type;

class Function : public BaseIR {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::FunctionIR };
	Function(Symbol* funcSym): BaseIR(), sym_(funcSym) { SET_TYPE_ID(FunctionIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	void add_BB(Block* bb) { bbs_.push_back(bb); }
	auto& get_BBs() { return bbs_; }

	Symbol* get_tmp_var(Type* ty);

	void add_var(std::string sym, Symbol* var) { symTab_[sym] = var; }
	Symbol* get_var(std::string sym) { return symTab_.at(sym); }

	std::string get_func_name();

private:
	Symbol* sym_ { nullptr };
	std::vector<Block*> bbs_;
	unsigned long tmpVarCount_ { 0 };
	std::unordered_map<std::string, Symbol*> symTab_;
};

}
