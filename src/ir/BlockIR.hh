#pragma once

#include "BaseIR.hh"
#include "StmtIR.hh"
#include "../asm/cfg.hh"
#include "common/utils.hh"


class BlockIR : public IRBase {
  public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::BlockIR };

	BlockIR(std::string sym): sym_(std::move(sym)) { SET_TYPE_ID(BlockIR); }

  public:
	void dump(std::ostream& os) const override;

  public:
	void appendStmt(StmtIR* stmt) { stmts_.push_back(stmt); }
	void setEndStmt(StmtIR* stmt) { endStmt_ = stmt; }

	bool isEntryBlock() { return sym_ == "entry"; }

	void gen_asm(FuncASM* func);
  private:
	std::vector<StmtIR*> stmts_;
	StmtIR* endStmt_ { nullptr };
	std::string sym_;
};