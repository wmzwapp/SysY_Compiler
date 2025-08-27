#pragma once

#include <cstdint>
#include <ostream>
#include <sstream>

enum class IRObjType : uint32_t {
	IRBase,

	// value
	ValueIntIR,
	SymbolIR,

	// entity
	ProgramIR,
	FunctionIR,
	BlockIR,
	StmtRetIR,
	StmtBinaryExprIR,
};

#define IR_SET_TYPE(T) setTypeId(IRObjType::T)

class IRBase {
	IRObjType typeId_;

  public:
	virtual ~IRBase() = default;

  public:
	void setTypeId(IRObjType type) { typeId_ = type; }
	IRObjType getTypeId() const { return typeId_; }

	IRBase() {
		IR_SET_TYPE(IRBase);
	}

  public:
	virtual void dump(std::ostream& os) const = 0;
	std::string to_string() {
		std::stringstream ss;
		dump(ss);
		return ss.str();	
	};
};

template<typename T>
inline bool isa(const IRBase* obj) {
	return obj->getTypeId() == T::TYPE_ID_;
}


