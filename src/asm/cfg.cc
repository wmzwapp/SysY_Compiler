#include "cfg.hh"
#include <ostream>

void TOPASM::dump(std::ostream& os) {
    os << ".text\n";
    for (auto* func : funcs_) {
        os << ".globl " << func->get_entry_label() << std::endl;
    }
    for (auto* func : funcs_) {
        func->dump(os);
    }
}


std::string FuncASM::get_entry_label() {
    return bbs_.front()->get_label();
}


FuncASM* TOPASM::create_func(std::string funcName) {
    auto* func = new FuncASM(funcName);
    funcs_.push_back(func);
    return func;
}


FuncASM::FuncASM(std::string sym) {
    auto* entryBB = new BasicBlockASM(sym);
    bbs_.push_back(entryBB);
    curbb_ = entryBB;
}

void FuncASM::dump(std::ostream& os) {
    for (auto* bb : bbs_) {
        bb->dump(os);
    }
}


BasicBlockASM* FuncASM::create_bb() {
    auto* newBB = new BasicBlockASM(std::to_string(bbs_.size()));
    bbs_.push_back(newBB);
    curbb_ = newBB;
    return newBB;
}


void BasicBlockASM::dump(std::ostream& os) {
    os << sym_ << ":\n";
    for (auto* instr : instrs_) {
        os << '\t';
        instr->dump(os);
        os << std::endl;
    }
    os << std::endl;
}