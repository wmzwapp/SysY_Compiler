#include "BlockAST.hh"
#include "ir/BlockIR.hh"

#include <iostream>


void BlockAST::Dump() const {
    std::cout << "<Block> { ";
    stmt_->Dump();
    std::cout << " }";
}


void BlockAST::GenIR(FunctionIR* func) {
    //
    auto* bb = new BlockIR("entry");
    stmt_->GenIR(func, bb);
    func->appendBB(bb);
}