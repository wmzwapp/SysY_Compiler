#include "BlockAST.hh"
#include "BaseAST.hh"
#include "StmtAST.hh"

#include <iostream>


/* class BlockAST */
void BlockAST::Dump(std::ostream& os) const {
    os << "<Block> {\n";
    for (auto* item : items_) {
        item->Dump(os);
        os << std::endl;
    }
    os << " }";
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

void BlockItemAST::Dump(std::ostream& os) const {
    os << "<BlockItem> { ";
    if (is_decl()) {
        os << "<Decl> { ";
        std::get<0>(item_)->Dump(os);
        os << " }";
    } else if (is_stmt()) {
        os << "<Stmt> { ";
        std::get<1>(item_)->Dump(os);
        os << " }";
    }
    os << " }";
}

/* class BlockItemAST end */