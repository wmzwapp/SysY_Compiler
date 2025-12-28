#pragma once

#include "BaseIR.hh"
#include "TypeIR.hh"

#include <vector>

class StmtIR;

class BlockIR : public IRBase {
public:
	inline static constexpr ObjType TYPE_ID_ { ObjType::BlockIR };
	BlockIR(std::string sym): IRBase(mmpool_.make<TypeUnitIR>()), sym_(std::move(sym)) { SET_TYPE_ID(BlockIR); }

public:
	void dump(std::ostream& os) const override;
	void accept(IRVisitor* visitor, IVCtx* ctx) override { visitor->visit(this, ctx); }

public:
	void add_stmt(StmtIR* stmt) { stmts_.push_back(stmt); }
	void add_end_stmt(StmtIR* stmt) { endStmt_ = stmt; }

	std::vector<StmtIR*>& get_stmts() { return stmts_; }
	StmtIR* get_end_stmt() { return endStmt_; }

private:
	std::vector<StmtIR*> stmts_;
	StmtIR* endStmt_ { nullptr };
	std::string sym_;
};