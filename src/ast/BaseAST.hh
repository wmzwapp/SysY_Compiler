#pragma once

#include "common/utils.hh"
#include "ir/BlockIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/ProgramIR.hh"
#include "ir/ValueIR.hh"


class GenIRCfg {
	ProgramIR*	curProgram_ { nullptr };
	FunctionIR*	curFunc_	{ nullptr };
	BlockIR*	curBlock_	{ nullptr };

	ValueIR*	retValue_	{ nullptr };

public:
	ProgramIR* get_current_programIR() { return curProgram_; }
	void set_current_programIR(ProgramIR* ir) { curProgram_ = ir; }

	FunctionIR*	get_current_functionIR() { return curFunc_; }
	void set_current_functionIR(FunctionIR* ir) { curFunc_ = ir; }

	BlockIR* get_current_blockIR() { return curBlock_; }
	void set_current_blockIR(BlockIR* ir) { curBlock_ = ir; }

	ValueIR* get_return_value() { return retValue_; }
	void set_return_value(ValueIR* ir) { retValue_ = ir; }
};


class BaseAST {
	ObjType type_;
  public:
	virtual ~BaseAST() = default;

	void set_type_id(ObjType t) { type_ = t; }

  public:
	virtual void Dump() const = 0;
	virtual void gen_ir(GenIRCfg* cfg) = 0;
};