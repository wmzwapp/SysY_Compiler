#include "CompUnitAST.hh"
#include "FuncDefAST.hh"
#include "ast/BaseAST.hh"


void CompUnitAST::Dump(std::ostream& os) const {
    os << "<CompUnit> { ";
    funcDef_->Dump(os);
    os << " }" << std::endl;
}

