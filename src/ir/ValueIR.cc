#include "ValueIR.hh"
#include "ir/TypeIR.hh"
#include <cassert>
#include <iostream>
#include <string>

// ValueIntIR* ValueIntIR::create_num_var(int v) {
//     auto key = std::to_string(v);
//     auto* var = new ValueIntIR(v);
//     if (_gIRVarsMap_.find(key) != _gIRVarsMap_.end()) {
//         std::cout << "ValueIntIR '" << key << "' overwrite!" << std::endl;
//     }
//     _gIRVarsMap_[key] = var;
//     return var;
// }

// ValueIntIR* ValueIntIR::get_num_var(int v) {
//     auto key = std::to_string(v);
//     auto iter = ;
//     if (_gIRVarsMap_.find(key) != _gIRVarsMap_.end()) {
//         auto* var = new ValueIntIR(v);
//         _gIRVarsMap_[key] = var;
//         return var;
//     } else {
//         return std::get<0>(iter->second);
//     }
// }




SymbolIR* SymbolIR::create_named_var(std::string sym, TypeIR* ty, bool isTemp) {
    auto* var = new SymbolIR(sym, ty, isTemp);
    if (_gIRVarsMap_.find(sym) != _gIRVarsMap_.end()) {
        std::cout << "SymbolIR '" << sym << "' overwrite!" << std::endl;
    }
    _gIRVarsMap_[sym] = var;
    return var;
}

SymbolIR* SymbolIR::get_named_var(std::string sym) {
    auto iter = _gIRVarsMap_.find(sym);
    if (iter == _gIRVarsMap_.end()) {
        std::cout << "Can't get SymbolIR '" << sym << "'!" << std::endl; 
        assert(false);
        return nullptr;
    } else {
        return iter->second;
    }
}
