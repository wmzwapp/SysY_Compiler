#pragma once

#include <ostream>
#include <string>


class VarASM {
public:
    VarASM(std::string sym, bool isTemp = false ) : name_(sym), isTemp_(isTemp) {}

    friend std::ostream& operator<<(std::ostream& os, const VarASM& obj) { os << obj.name_; return os; }

    bool is_temp() const { return isTemp_; }

    // void set_offset(unsigned offset) { stkOffset_ = offset; }
    // unsigned get_offset() const { return stkOffset_; }

private:
    std::string name_;
    bool isTemp_ { false };
    // unsigned stkOffset_ { (unsigned)-1 };
};