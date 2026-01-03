#include "TypeIR.hh"
#include "FunctionIR.hh"

#include <cassert>

using namespace IR;

void TypeFunc::dump(std::ostream& os) const {
    assert(isa<Function*>(prototype_));
    os << "@" << static_cast<Function*>(prototype_)->get_func_name() << "():";
    retType_->dump(os);
}