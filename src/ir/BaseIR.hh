#pragma once

#include "common/utils.hh"
#include "common/mmp.hh"

#include <ostream>
#include <sstream>

class ProgramIR;
class FunctionIR;
class BlockIR;
class StmtRetIR;
class StmtBinaryExprIR;
class AllocIR;
class LoadIR;
class StoreIR;
class ValueIR;
class SymbolIR;
class ValueIntIR;
class TypeIR;

extern MArena mmpool_;

class IRVisitorContext {
	// empty
};

using IVCtx = IRVisitorContext;

class IRVisitor {
public:
	virtual void visit(ProgramIR* unit, IVCtx* ctx);
	virtual void visit(FunctionIR* func, IVCtx* ctx);
	virtual void visit(BlockIR* block, IVCtx* ctx);
	virtual void visit(StmtRetIR* stmt, IVCtx* ctx);
	virtual void visit(StmtBinaryExprIR* stmt, IVCtx* ctx);
	virtual void visit(AllocIR* stmt, IVCtx* ctx);
	virtual void visit(LoadIR* stmt, IVCtx* ctx);
	virtual void visit(StoreIR* stmt, IVCtx* ctx);
	virtual void visit(SymbolIR* sym, IVCtx* ctx);
	virtual void visit(ValueIntIR* num, IVCtx* ctx);
};
class IRBase {
	ObjType typeId_;

protected:
	TypeIR*	ty_	{ nullptr };

public:
	IRBase(TypeIR* ty) : ty_(ty) { SET_TYPE_ID(IRBase); }
	virtual ~IRBase() = default;

public:
	virtual void dump(std::ostream& os) const = 0;
	virtual std::string repr() const {
		std::stringstream ss;
		dump(ss);
		return ss.str();	
	};
	virtual void accept(IRVisitor* visitor, IVCtx* ctx) = 0;

public:
	void set_type_id(ObjType type) { typeId_ = type; }
	ObjType get_type_id() const { return typeId_; }

	TypeIR*	get_ty() { return ty_; }
	void set_ty(TypeIR* ty) { ty_ = ty; }
};
