#include "FunctionIR.hh"
#include "asm/cfg.hh"

#include <cassert>
#include <ostream>


void FunctionIR::dump(std::ostream& os) const {
    os << "fun ";
    auto* ty = sym_->getType();
    assert(ty->isFunc());
    ty->dump(os);
    os << " {" << std::endl;

    for (auto* bb : bbs_) {
        bb->dump(os);
    }

    os << "}" << std::endl;
}

void FunctionIR::gen_asm(ConfigASM* asmer) {
    auto* funcASM = asmer->create_func(getFuncName());

    for (auto* block : bbs_) {
        block->gen_asm(funcASM);
    }
}