#pragma once

#include <cstddef>
#include <ostream>
#include <vector>


namespace IR {

class BaseIR;

class Type {
public:
	virtual bool isInt() const { return false; }
	virtual bool isArray() const { return false; }
	virtual bool isPointer() const { return false; }
	virtual bool isFunc() const { return false; }
	virtual bool isUnit() const { return false; }

public:
	virtual void dump(std::ostream& os) const = 0;
	virtual unsigned get_layout_size() const = 0;
	virtual ~Type() {}
};


class TypeUnit: public Type {
public:
	bool isUnit() const override { return true; }
	void dump(std::ostream& os) const override { /* do nothing */ }
	unsigned get_layout_size() const override { return 0; }
};


class TypeInt: public Type {
public:
	bool isInt() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32"; }
	unsigned get_layout_size() const override { return sizeof(int); }
};


class TypeFunc: public Type {
public:
	bool isFunc() const override { return true; }
	void dump(std::ostream& os) const override;
	unsigned get_layout_size() const override { return 0; }			// ???

public:
	void set_prototype(BaseIR* funcIR) { prototype_ = funcIR; }
	void set_ret_type(Type* retType) { retType_ = retType; }

private:
	BaseIR* prototype_ { nullptr };
	Type* retType_ { nullptr };
	std::vector<Type*> argTypes_;
};


class TypePtr : public Type {
public:
	TypePtr(Type* ir) : source_(ir) {}

	bool isPointer() const override { return true; }
	void dump(std::ostream& os) const override { os << "*"; source_->dump(os); }
	unsigned get_layout_size() const override { return sizeof(ptrdiff_t); }

public:
	Type* get_source() { return source_; }  

private:
	Type*		source_ { nullptr };
};

}
