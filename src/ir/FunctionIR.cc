#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "ValueIR.hh"

#include <cassert>
#include <ostream>

using namespace IR;

std::string Function::get_func_name() { return sym_->repr(); }

Symbol* Function::get_tmp_var(Type* ty) {
    auto tmpVarName = std::string("t") + std::to_string(++tmpVarCount_);
    auto* var = mmpool_.make<Symbol>(tmpVarName, ty, true);
    symTab_[tmpVarName] = var;
    return var;
}

void Function::dump(std::ostream& os) const {
    os << "fun ";
    sym_->get_ty()->dump(os);
    os << " {" << std::endl;

    for (auto* bb : bbs_) {
        bb->dump(os);
    }

    os << "}" << std::endl;
}
