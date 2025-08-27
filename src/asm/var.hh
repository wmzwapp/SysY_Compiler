#pragma once

#include <ostream>
#include <string>


class VarASM {
public:
    VarASM(std::string sym, bool isTemp = false ) : name_(sym), isTemp_(isTemp) {}

    friend std::ostream& operator<<(std::ostream& os, const VarASM& obj) { os << obj.name_; return os; }

private:
    std::string name_;
    bool isTemp_ { false };
};