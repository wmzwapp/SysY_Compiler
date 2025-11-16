#include "BlockAST.hh"
#include "ast/BaseAST.hh"
#include "ir/BlockIR.hh"

#include <iostream>


/* class BlockAST */
BlockAST::~BlockAST() {
    for (auto* v : items_) {
        delete v;
    }
}


void BlockAST::Dump() const {
    std::cout << "<Block> {\n";
    for (auto* item : items_) {
        item->Dump();
        std::cout << std::endl;
    }
    std::cout << " }";
}


void BlockAST::gen_ir(GenIRCfg* cfg) {
    //
    auto* bb = new BlockIR("entry");
    cfg->get_current_functionIR()->appendBB(bb);
    cfg->set_current_blockIR(bb);

    for (auto* item : items_) {
        item->gen_ir(cfg);
    }
}

/* class BlockAST end */


/* class BlockItemAST */

BlockItemAST::BlockItemAST(BaseAST* ast) {
    SET_TYPE_ID(BlockItemAST);
    if (isa<DeclAST*>(ast)) {
        item_ = (DeclAST*)ast;
    } else if (isa<StmtAST*>(ast)) {
        item_ = (StmtAST*)ast;
    }
}

BlockItemAST::~BlockItemAST() {
    if (is_decl()) {
        delete get_decl_ast();
    } else if (is_stmt()) {
        delete get_stmt_ast();
    }
}

void BlockItemAST::Dump() const {
    std::cout << "<BlockItem> { ";
    if (is_decl()) {
        std::cout << "<Decl> { ";
        std::get<0>(item_)->Dump();
        std::cout << " }";
    } else if (is_stmt()) {
        std::cout << "<Stmt> { ";
        std::get<1>(item_)->Dump();
        std::cout << " }";
    }
    std::cout << " }";
}

void BlockItemAST::gen_ir(GenIRCfg* cfg) {
    if (is_decl()) {
        // do nothing
        std::get<0>(item_)->gen_ir(cfg);
    } else if (is_stmt()) {
        std::get<1>(item_)->gen_ir(cfg);
    }
}

/* class BlockItemAST end */