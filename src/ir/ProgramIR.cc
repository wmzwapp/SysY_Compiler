#include "ProgramIR.hh"
#include "FunctionIR.hh"
#include <ostream>

using namespace IR;

void Program::dump(std::ostream& os) const {
    for (auto* func : funcs_) {
        func->dump(os);
    }
}
