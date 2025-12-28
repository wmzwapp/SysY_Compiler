#include "BlockIR.hh"
#include "StmtIR.hh"


void BlockIR::dump(std::ostream& os) const {
    os << '%' << sym_ << ':' << std::endl;

    for (auto* stmt : stmts_) {
        stmt->dump(os);
    }
}
