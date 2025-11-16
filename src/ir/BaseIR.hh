#pragma once

#include "../common/utils.hh"

#include <cstdint>
#include <ostream>
#include <sstream>
#include "TypeIR.hh"

struct GenASMCfg;
class TOPASM;
class FuncASM;
class BasicBlockASM;

class IRBase {
	ObjType typeId_;
	
  public:
	TypeIR*	ty_	{ nullptr };
	IRBase(TypeIR* ty) : ty_(ty) { SET_TYPE_ID(IRBase); }
	virtual ~IRBase() { delete ty_; }

  public:
	void set_type_id(ObjType type) { typeId_ = type; }
	ObjType get_type_id() const { return typeId_; }

	TypeIR*	get_ty() { return ty_; }
	void set_ty(TypeIR* ty) { ty_ = ty; }

  public:
	virtual void dump(std::ostream& os) const = 0;
	std::string to_string() {
		std::stringstream ss;
		dump(ss);
		return ss.str();	
	};

	virtual void gen_asm(GenASMCfg* cfg) = 0;
};

struct GenASMCfg {
	TOPASM*			top_;
	FuncASM*		currentFunc_;
	BasicBlockASM*	currentBB_;
	uint32_t		stackOffset_;
};

