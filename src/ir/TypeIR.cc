#include "TypeIR.hh"
#include "ir/FunctionIR.hh"

#include <cassert>
#include <ostream>


void TypeFuncIR::dump(std::ostream& os) const {
    assert(isa<FunctionIR*>(prototype_));
    static_cast<FunctionIR*>(prototype_)->getSym()->dump(os);
    os << "(): ";
    retType_->dump(os);
}