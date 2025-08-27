#include "CompUnitAST.hh"
#include "ir/ProgramIR.hh"
#include "ir/FunctionIR.hh"

#include "iostream"
#include <cassert>


void CompUnitAST::Dump() const {
	std::cout << "<CompUnit> { ";
    funcDef_->Dump();
    std::cout << " }";
}


void CompUnitAST::GenIR(ProgramIR* ir) {
    auto* funcIR = dynamic_cast<FunctionIR*>(funcDef_->GenIR());
    assert(funcIR != nullptr);
    ir->appendFunctionIR(funcIR);
}
