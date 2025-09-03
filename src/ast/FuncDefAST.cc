#include "FuncDefAST.hh"

#include "BaseAST.hh"

#include <cassert>
#include <iostream>


void FuncDefAST::Dump() const {
    std::cout << "<FuncDef> { ";
    funcType_->Dump();
    std::cout << ", " << ident_ << ", ";
    block_->Dump();
    std::cout << " }";
}


void FuncDefAST::gen_ir(GenIRCfg* cfg) {
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

    cfg->get_current_programIR()->appendFunctionIR(funcIR);
    cfg->set_current_functionIR(funcIR);

    // blocks
    if (block_ != nullptr) {
        block_->gen_ir(cfg);
    }
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