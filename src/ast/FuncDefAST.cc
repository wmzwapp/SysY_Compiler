#include "FuncDefAST.hh"
#include "BaseAST.hh"
#include "ast/VarAST.hh"

#include <cassert>
#include <iostream>


void FuncDefAST::Dump() const {
    std::cout << "<FuncDef> { ";
    funcType_.repr();
    std::cout << ", ";
    ident_.repr();
    std::cout << ", ";
    block_->Dump();
    std::cout << " }";
}


void FuncDefAST::gen_ir(GenIRCfg* cfg) {
    
    // type
    auto* type = TypeAST::get_func_type_ir();
    auto* retType = TypeAST::get_int_type_ir();
    type->setRetType(retType);
    
    // symbol & function IR
    auto* sym = SymbolIR::create_named_var(ident_.get_ident(), TypeAST::get_unit_type_ir());
    auto* funcIR = new FunctionIR(sym, type);

    type->setPrototype(funcIR);

    cfg->get_current_programIR()->appendFunctionIR(funcIR);
    cfg->set_current_functionIR(funcIR);

    // blocks
    if (block_ != nullptr) {
        block_->gen_ir(cfg);
    }
}
