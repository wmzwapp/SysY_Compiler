#include "BlockAST.hh"
#include "ast/BaseAST.hh"
#include "ir/BlockIR.hh"

#include <iostream>


void BlockAST::Dump() const {
    std::cout << "<Block> { ";
    stmt_->Dump();
    std::cout << " }";
}


void BlockAST::gen_ir(GenIRCfg* cfg) {
    //
    auto* bb = new BlockIR("entry");
    cfg->get_current_functionIR()->appendBB(bb);
    cfg->set_current_blockIR(bb);

    stmt_->gen_ir(cfg);
}