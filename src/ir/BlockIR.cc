#include "BlockIR.hh"
#include "asm/cfg.hh"


void BlockIR::dump(std::ostream& os) const {
    os << '%' << sym_ << ':' << std::endl;

    for (auto* stmt : stmts_) {
        stmt->dump(os);
    }
}

void BlockIR::gen_asm(FuncASM* func) {
    if (!isEntryBlock()) {
        func->create_bb();
    }
    auto* bbASM = func->get_current_bb();

    for (auto* stmt : stmts_) {
        auto* instr = stmt->gen_asm(bbASM);
    }
}