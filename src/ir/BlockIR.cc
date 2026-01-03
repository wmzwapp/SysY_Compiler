#include "BlockIR.hh"
#include "InstrIR.hh"


using namespace IR;

void Block::dump(std::ostream& os) const {
    os << '%' << sym_ << ':' << std::endl;

    for (auto* instr : Instrs_) {
        instr->dump(os);
    }
}
