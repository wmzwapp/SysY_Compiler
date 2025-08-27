#include "FuncDefAST.hh"
#include "ir/BaseIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/ValueIR.hh"

#include <cassert>
#include <iostream>


void FuncDefAST::Dump() const {
    std::cout << "<FuncDef> { ";
    funcType_->Dump();
    std::cout << ", " << ident_ << ", ";
    block_->Dump();
    std::cout << " }";
}


IRBase* FuncDefAST::GenIR() {
    // symbol & function IR
    auto* sym = new SymbolIR(ident_);
    auto* funcIR = new FunctionIR(sym);

    // type
    auto* type = new TypeFuncIR();
    sym->setType(type);
    if (funcType_ != nullptr) {
        auto* retType = funcType_->getType();
        type->setRetType(retType);
    }
    type->setPrototype(funcIR);

    // blocks
    if (block_ != nullptr) {
        block_->GenIR(funcIR);
    }

    return funcIR;
}


void FuncTypeAST::Dump() const {
    std::cout << "<FuncType> { ";
    std::cout << funcType_;
    std::cout << " }";
}


TypeIR* FuncTypeAST::getType() {
    if (funcType_ == "int") {
        return new TypeIntIR();
    }
    return nullptr;
}