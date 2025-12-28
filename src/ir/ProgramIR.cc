#include "ProgramIR.hh"
#include "FunctionIR.hh"
#include <ostream>


void ProgramIR::dump(std::ostream& os) const {
    for (auto* func : funcs_) {
        func->dump(os);
    }
}
