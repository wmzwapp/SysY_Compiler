#pragma once
// #include "../ext/include/koopa.h"

#include <vector>
#include <string>

class ProgramIR;
class FunctionIR;
class BasicBlockIR;
class InstrBaseIR;
class ValueIR;
class ValueNumIR;

class IRBase {
public:
	virtual ~IRBase() = default;

public:
	virtual void dump() = 0;
	virtual std::string toStr() = 0;
};


class ProgramIR : public IRBase {
public:
	void dump() override;
	std::string toStr() override;

	void appendFunctionIR(FunctionIR* func) { funcs_.push_back(func); }

private:
	std::vector<ValueIR*>       	gValues_;
	std::vector<FunctionIR*>    	funcs_;
};

inline ProgramIR __IR_TOP__;

class FunctionIR : public IRBase {
public:
	FunctionIR(std::string name): name_(name) {}

public:
	void dump() override;
	std::string toStr() override;

public:
	void setRet(ValueNumIR* ret) { ret_ = ret; }
	void appendBB(BasicBlockIR* bb) { bbs_.push_back(bb); }
	void appendArg(ValueIR* arg) { args_.push_back(arg); }


private:
    std::vector<BasicBlockIR*>    	bbs_;
    std::string                 	name_;
	ValueNumIR*						ret_ { nullptr };
	std::vector<ValueIR*>			args_;
};


class BasicBlockIR : public IRBase {
public:
	BasicBlockIR(std::string name): name_(name) {}

public:
	void dump() override;
	std::string toStr() override;

public:
	void appendInstr(InstrBaseIR* instr) { instrs_.push_back(instr); }

private:
    std::vector<InstrBaseIR*>		instrs_;
    std::string                 	name_;
};

class ValueIR : public IRBase {
public:
    // virtual ~ValueIR () = default;

private:
    // koopa_raw_value_kind_t      v_;
};


enum class InstrOp {
	O_ret,

	O_end
};


class InstrBaseIR : public ValueIR {
public:
	virtual ~InstrBaseIR() = default;

	InstrOp	getOp()					{ return op_; }
	void	setOp(InstrOp op)		{ op_ = op; }

private:
	InstrOp						op_;
};


template <InstrOp op>
class InstrIR : public InstrBaseIR {
public:

};


template<>
class InstrIR<InstrOp::O_ret> : public InstrBaseIR {
public:
	InstrIR(ValueIR* ret): ret_(ret) {}

public:
	void dump() override;
	std::string toStr() override;

private:
	ValueIR*					ret_ { nullptr };
};
using InstrRetIR = InstrIR<InstrOp::O_ret>;


class ValueNumIR : public ValueIR {
public:
	void 			dump() override {}
	std::string 	toStr() override { return std::to_string(getValue()); }

public:
	int				getValue()			{ return value_; }
	void			setValue(int v)		{ value_ = v; }

	std::string		getValueTyStr()		{ return "i32"; }

public:
	int				value_ { 0 };
};

