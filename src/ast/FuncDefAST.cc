#include "FuncDefAST.hh"
#include "BlockAST.hh"

#include <iostream>


void FuncDefAST::Dump(std::ostream& os) const {
    os << "<FuncDef> { ";
    funcType_.Dump(os);
    os << ", ";
    ident_.Dump(os);
    os << ", ";
    block_->Dump(os);
    os << " }";
}
