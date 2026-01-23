#include "cfg.hh"
#include "instr.hh"

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

void FuncASM::dump(std::ostream& os) {
    for (auto* bb : bbs_) {
        bb->dump(os);
    }
}

BasicBlockASM* FuncASM::create_or_get_BB(std::string label) {
    if (bbsMap_.find(label) != bbsMap_.end()) {
        return bbsMap_[label];
    } else {
        auto* bb = mmpool_.make<BasicBlockASM>(label);
        bbsMap_[label] = bb;
        bbs_.push_back(bb);
        return bb;
    }
}

void BasicBlockASM::push_back_instr(Instruction* instr) {
    if (currentInstr_ != nullptr) {
        currentInstr_->setNext(instr);
        instr->setPrev(currentInstr_);
    } else {
        instr->setPrev(nullptr);
    }
    instr->setNext(nullptr);
    instrs_.push_back(instr);
    currentInstr_ = instr;
}

void BasicBlockASM::dump(std::ostream& os) {
    os << label_ << ":\n";
    for (auto* instr : instrs_) {
        os << '\t';
        instr->dump(os);
        os << std::endl;
    }
    // os << std::endl;
}