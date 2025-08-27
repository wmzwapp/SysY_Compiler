#include "ProgramIR.hh"
#include "asm/cfg.hh"
#include <ostream>


void ProgramIR::dump(std::ostream& os) const {
    for (auto* v : gValues_) {
        v->dump(os);
    }

    for (auto* func : funcs_) {
        func->dump(os);
    }
}

void ProgramIR::gen_asm(ConfigASM* asmer) {
    for (auto* v : gValues_) {
        // TODO
    }

    for (auto* func : funcs_) {
        func->gen_asm(asmer);
    }
}