#include "TypeIR.hh"
#include "FunctionIR.hh"

#include <cassert>


void TypeFuncIR::dump(std::ostream& os) const {
    assert(isa<FunctionIR*>(prototype_));
    os << "@" << static_cast<FunctionIR*>(prototype_)->get_func_name() << "():";
    retType_->dump(os);
}