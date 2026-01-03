#pragma once

#include "common/utils.hh"
#include "common/mmp.hh"

#include <ostream>
#include <sstream>

extern MArena mmpool_;

namespace IR {

class Program;
class Function;
class Block;
class InstrRet;
class InstrBExpr;
class InstrAlloc;
class InstrLoad;
class InstrStore;
class Value;
class Symbol;
class ValueInt;

class IRVisitorContext {
	// empty
};

using IVCtx = IRVisitorContext;

class IRVisitor {
public:
	virtual void visit(Program* unit, IVCtx* ctx);
	virtual void visit(Function* func, IVCtx* ctx);
	virtual void visit(Block* block, IVCtx* ctx);
	virtual void visit(InstrRet* stmt, IVCtx* ctx);
	virtual void visit(InstrBExpr* stmt, IVCtx* ctx);
	virtual void visit(InstrAlloc* stmt, IVCtx* ctx);
	virtual void visit(InstrLoad* stmt, IVCtx* ctx);
	virtual void visit(InstrStore* stmt, IVCtx* ctx);
	virtual void visit(Symbol* sym, IVCtx* ctx);
	virtual void visit(ValueInt* num, IVCtx* ctx);
};
class BaseIR {
	ObjType typeId_;
public:
	BaseIR() { SET_TYPE_ID(BaseIR); }
	virtual ~BaseIR() = default;

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
};

}	// namespace IR
