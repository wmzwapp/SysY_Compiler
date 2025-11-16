#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>



class IRBase;
class TypeIR;
class TypeIntIR;
class TypePtrIR;
inline std::unordered_map<std::string, TypeIR*> _gTypeM_; 

class TypeIR {
  public:
	virtual bool isInt() const { return false; }
	virtual bool isArray() const { return false; }
	virtual bool isPointer() const { return false; }
	virtual bool isFunc() const { return false; }
	virtual bool isUnit() const { return false; }

public:
	virtual void dump(std::ostream& os) const = 0;
	virtual ~TypeIR() {}
	virtual TypeIR* clone() const = 0;
	virtual unsigned get_layout_size() const = 0;
};


class TypeUnitIR: public TypeIR {
  public:
	bool isUnit() const override { return true; }
	void dump(std::ostream& os) const override { /* do nothing */ }
	TypeIR* clone() const override { return new TypeUnitIR(*this); }
	unsigned get_layout_size() const override { return 0; }
};


class TypeIntIR: public TypeIR {
  public:
	bool isInt() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32"; }
	TypeIR* clone() const override { return new TypeIntIR(*this); }
	unsigned get_layout_size() const override { return sizeof(int); }
};


class TypeFuncIR: public TypeIR {
public:
	TypeFuncIR() = default;
	~TypeFuncIR() {
		delete retType_;
		for (auto* argType : argTypes_) {
			delete argType;
		}
	}
	TypeFuncIR(const TypeFuncIR& other) {
		retType_ = other.retType_->clone();
		for (auto* argType : other.argTypes_) {
			argTypes_.push_back(argType->clone());
		}
	}
public:
	bool isFunc() const override { return true; }
	void dump(std::ostream& os) const override;
	TypeIR* clone() const override { return new TypeFuncIR(*this); }
	unsigned get_layout_size() const override { return 0; }			// ???

	void setPrototype(IRBase* funcIR) { prototype_ = funcIR; }
	void setRetType(TypeIR* retType) { retType_ = retType; }

	bool hasRet() const { return retType_ != nullptr; }
	TypeIR* getRetType() { return retType_; }

private:
	IRBase* prototype_ { nullptr };
	TypeIR* retType_ { nullptr };
	std::vector<TypeIR*> argTypes_;
};


class TypePtrIR : public TypeIR {
public:
	TypePtrIR(TypeIR* ir) : source_(ir) {}
	~TypePtrIR() { delete source_; }
	TypePtrIR(const TypePtrIR& other): source_(other.source_->clone()) {}

	bool isPointer() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32*"; }
	TypeIR* clone() const override { return new TypePtrIR(*this); }
	unsigned get_layout_size() const override { return sizeof(ptrdiff_t); }

	TypeIR* get_source() { return source_; }  
private:
	TypeIR*		source_ { nullptr };
};
