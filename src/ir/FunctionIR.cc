#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "InstrIR.hh"
#include "ValueIR.hh"

#include <cassert>
#include <ostream>

using namespace IR;

std::string Function::get_func_name() { return sym_->repr(); }

Block* Function::create_BB() {
    auto* newBBSym = mmpool_.make<Symbol>(std::string("b") + std::to_string(bbs_.size()), mmpool_.make<TypeUnit>(), true);
    auto* newBB = mmpool_.make<Block>(newBBSym);
    bbs_.push_back(newBB);
    return newBB;
}


Symbol* Function::get_return_var() {
    if (returnSlot_) {
         return returnSlot_;
    }
    auto* retTy = get_return_type();
    returnSlot_ = mmpool_.make<Symbol>("_retSlot_", mmpool_.make<TypePtr>(retTy));
    if (entryBB_->get_entry_instr() == nullptr) {
        entryBB_->create_instr<InstrAlloc>(returnSlot_, retTy);
    } else {
        auto* mark = entryBB_->get_current_instr();
        auto* instr = entryBB_->create_instr<InstrAlloc>(returnSlot_, retTy);
        instr->insert_me_before(entryBB_->get_entry_instr());
        entryBB_->set_entry_instr(instr);
        entryBB_->set_current_instr(mark);
    }
    auto* retVar = get_tmp_var(retTy);
    exitBB_->create_instr<InstrLoad>(retVar, returnSlot_);
    exitBB_->create_instr<InstrRet>(retVar);
    return returnSlot_;
}

Type* Function::get_return_type() {
    assert(sym_->get_ty()->isFunc());
    return ((TypeFunc*)sym_->get_ty())->get_ret_type();
}

Symbol* Function::get_tmp_var(Type* ty) {
    auto tmpVarName = std::string("t") + std::to_string(tmpVarCount_++);
    auto* var = mmpool_.make<Symbol>(tmpVarName, ty, true);
    symTab_[tmpVarName] = var;
    return var;
}

void Function::dump(std::ostream& os) const {
    os << "fun ";
    sym_->get_ty()->dump(os);
    os << " {" << std::endl;

    for (auto* bb : bbs_) {
        bb->dump(os);
    }

    os << "}" << std::endl;
}
