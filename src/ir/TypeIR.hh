#pragma once

#include <cstddef>
#include <ostream>
#include <vector>


class IRBase;

class TypeIR {
public:
	virtual bool isInt() const { return false; }
	virtual bool isArray() const { return false; }
	virtual bool isPointer() const { return false; }
	virtual bool isFunc() const { return false; }
	virtual bool isUnit() const { return false; }

public:
	virtual void dump(std::ostream& os) const = 0;
	virtual unsigned get_layout_size() const = 0;
	virtual ~TypeIR() {}
};


class TypeUnitIR: public TypeIR {
public:
	bool isUnit() const override { return true; }
	void dump(std::ostream& os) const override { /* do nothing */ }
	unsigned get_layout_size() const override { return 0; }
};


class TypeIntIR: public TypeIR {
public:
	bool isInt() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32"; }
	unsigned get_layout_size() const override { return sizeof(int); }
};


class TypeFuncIR: public TypeIR {
public:
	bool isFunc() const override { return true; }
	void dump(std::ostream& os) const override;
	unsigned get_layout_size() const override { return 0; }			// ???

public:
	void set_prototype(IRBase* funcIR) { prototype_ = funcIR; }
	void set_ret_type(TypeIR* retType) { retType_ = retType; }

private:
	IRBase* prototype_ { nullptr };
	TypeIR* retType_ { nullptr };
	std::vector<TypeIR*> argTypes_;
};


class TypePtrIR : public TypeIR {
public:
	TypePtrIR(TypeIR* ir) : source_(ir) {}

	bool isPointer() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32*"; }
	unsigned get_layout_size() const override { return sizeof(ptrdiff_t); }

public:
	TypeIR* get_source() { return source_; }  

private:
	TypeIR*		source_ { nullptr };
};
