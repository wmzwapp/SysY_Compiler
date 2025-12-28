#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "ValueIR.hh"

#include <cassert>
#include <ostream>


std::string FunctionIR::get_func_name() { return sym_->repr(); }

SymbolIR* FunctionIR::get_tmp_var(TypeIR* ty) {
    auto tmpVarName = std::string("t") + std::to_string(++tmpVarCount_);
    auto* var = mmpool_.make<SymbolIR>(tmpVarName, ty, true);
    symTab_[tmpVarName] = var;
    return var;
}

void FunctionIR::dump(std::ostream& os) const {
    os << "fun ";
    assert(ty_->isFunc());
    ty_->dump(os);
    os << " {" << std::endl;

    for (auto* bb : bbs_) {
        bb->dump(os);
    }

    os << "}" << std::endl;
}
