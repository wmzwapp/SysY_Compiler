#pragma once

#include "../common/utils.hh"

#include <ostream>
#include <sstream>

class IRBase {
	ObjType typeId_;

  public:
	virtual ~IRBase() = default;

  public:
	void set_type_id(ObjType type) { typeId_ = type; }
	ObjType get_type_id() const { return typeId_; }

	// IRBase() {
	// 	SET_TYPE_ID(IRBase);
	// }

  public:
	virtual void dump(std::ostream& os) const = 0;
	std::string to_string() {
		std::stringstream ss;
		dump(ss);
		return ss.str();	
	};
};

