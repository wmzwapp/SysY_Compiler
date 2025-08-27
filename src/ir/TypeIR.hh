#pragma once

#include "BaseIR.hh"
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>



class TypeIR;
inline std::unordered_map<std::string, TypeIR*> _gTypeM_; 

class TypeIR {
  public:
	virtual bool isInt() const { return false; }
	virtual bool isArray() const { return false; }
	virtual bool isPointer() const { return false; }
	virtual bool isFunc() const { return false; }

public:
	virtual void dump(std::ostream& os) const = 0;
};


class TypeIntIR: public TypeIR {
  public:
	bool isInt() const override { return true; }
	void dump(std::ostream& os) const override { os << "i32"; }
};


class TypeFuncIR: public TypeIR {
public:
	bool isFunc() const override { return true; }
	void dump(std::ostream& os) const override;

	void setPrototype(IRBase* funcIR) { prototype_ = funcIR; }
	void setRetType(TypeIR* retType) { retType_ = retType; }

	bool hasRet() const { return retType_ != nullptr; }
	TypeIR* getRetType() { return retType_; }

private:
	IRBase* prototype_ { nullptr };
	TypeIR* retType_ { nullptr };
	std::vector<TypeIR*> argTypes_;
};


inline TypeIntIR* getIntType() {
	if (_gTypeM_.find("i32") == _gTypeM_.end()) {
		_gTypeM_["i32"] = new TypeIntIR();
	}
	return (TypeIntIR*)_gTypeM_["i32"];
}

