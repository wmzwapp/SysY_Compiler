#include "BlockIR.hh"
#include "ValueIR.hh"
#include "InstrIR.hh"


using namespace IR;

void Block::dump(std::ostream& os) const {
    sym_->dump(os);
    os << ':' << std::endl;

    // for (auto* instr : Instrs_) {
    //     instr->dump(os);
    // }
    auto* instr = entryInstr_;
    while (instr) {
        instr->dump(os);
        instr = instr->get_next_instr();
    }
    os << std::endl;
}
