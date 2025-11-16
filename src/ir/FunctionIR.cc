#include "FunctionIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include "ir/BaseIR.hh"
#include "ir/TypeIR.hh"

#include <cassert>
#include <ostream>
#include <unordered_set>


inline int align_16_bytes(int v) {
    return (v + 15) & ~15;
}

void FunctionIR::dump(std::ostream& os) const {
    os << "fun ";
    assert(ty_->isFunc());
    ty_->dump(os);
    os << " {" << std::endl;

    for (auto* bb : bbs_) {
        bb->dump(os);
    }

    os << "}" << std::endl;
}

void FunctionIR::gen_asm(GenASMCfg* cfg) {
    auto* funcASM = cfg->top_->create_func(getFuncName());
    cfg->currentFunc_ = funcASM;
    cfg->stackOffset_ = 0;

    gen_asm_prologue(funcASM->get_current_bb());
    for (auto* block : bbs_) {
        block->gen_asm(cfg);
    }
    gen_asm_epilogue(funcASM->get_current_bb());
    cfg->currentBB_->create_instr<Instr0>(InstrOp::RET);
}

void FunctionIR::gen_asm_prologue(BasicBlockASM* bb) {
    // size_t stackSize_ { 0 };
    std::unordered_set<std::string> vars;
    for (auto* block : bbs_) {
        for (auto* stmt : block->get_stmts()) {
            auto* ty = stmt->get_ty();
            if (ty->isFunc() || ty->isUnit()) {
                continue;
            }
            if (ty->isInt() || ty->isFunc()) {
                auto* defVar = stmt->get_def_var();
                assert(defVar != nullptr);
                if (vars.find(defVar->getSym()) == vars.end()) {
                    defVar->set_offset(stackSize_);
                    stackSize_ += ty->get_layout_size();
                    vars.insert(defVar->getSym());
                }
            }
        }
    }
    stackSize_ = align_16_bytes(stackSize_);
    auto* sp = BasicBlockASM::get_reg_var_sp();
    if (stackSize_ > 0 && stackSize_ > (1 << (12 -1))) {
        auto* t0 = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr2RI>(InstrOp::LI, t0, -stackSize_);
        bb->create_instr<Instr3RRR>(InstrOp::ADD, sp, sp, t0);
    } else {
        bb->create_instr<Instr3RRI>(InstrOp::ADDI, sp, sp, -stackSize_);
    }
}

void FunctionIR::gen_asm_epilogue(BasicBlockASM* bb) {
    if (stackSize_ > 0) {
        auto* sp = BasicBlockASM::get_reg_var_sp();
        bb->create_instr<Instr3RRI>(InstrOp::ADDI, sp, sp, stackSize_);
    }
}