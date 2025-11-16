#include "BlockIR.hh"
#include "asm/cfg.hh"
#include "ir/BaseIR.hh"


void BlockIR::dump(std::ostream& os) const {
    os << '%' << sym_ << ':' << std::endl;

    for (auto* stmt : stmts_) {
        stmt->dump(os);
    }
}

void BlockIR::gen_asm(GenASMCfg* cfg) {
    auto* func = cfg->currentFunc_;
    if (!isEntryBlock()) {
        func->create_bb();
    }
    auto* bbASM = func->get_current_bb();
    cfg->currentBB_ = bbASM;

    for (auto* stmt : stmts_) {
        stmt->gen_asm(cfg);
    }
}