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
	// void add_BB(Block* bb) { bbs_.push_back(bb); }
	auto& get_BBs() { return bbs_; }
	Block* create_BB();
	Block* create_entry_BB() { entryBB_ = create_BB(); return entryBB_; }
	Block* create_exit_BB() { exitBB_ = create_BB(); return exitBB_; }
	Block* get_exit_BB() { return exitBB_; }
	Block* get_entry_BB() { return entryBB_; }

	Symbol* get_tmp_var(Type* ty);

	void add_var(std::string sym, Symbol* var) { symTab_[sym] = var; }
	Symbol* get_var(std::string sym) { return symTab_.at(sym); }

	std::string get_func_name();

	Symbol* get_return_var();
	Type* get_return_type();

private:
	Symbol* sym_ { nullptr };
	std::vector<Block*> bbs_;
	Symbol* returnSlot_ { nullptr };
	Block* entryBB_ { nullptr };
	Block* exitBB_ { nullptr };
	unsigned long tmpVarCount_ { 0 };
	std::unordered_map<std::string, Symbol*> symTab_;
};

}
