#include "ProgramIR.hh"
#include <ostream>


void ProgramIR::dump(std::ostream& os) const {
    for (auto* func : funcs_) {
        func->dump(os);
    }
}

void ProgramIR::gen_asm(GenASMCfg* cfg) {
    for (auto* func : funcs_) {
        func->gen_asm(cfg);
    }
}