#include "StmtIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include "asm/var.hh"
#include "common/utils.hh"
#include "ir/BaseIR.hh"
#include "ir/ValueIR.hh"
#include <cassert>

static VarASM* get_a_reg_var(BasicBlockASM* bb, ValueIR* v, int& idx) {
    if (auto* opnd = dynamic_cast<ValueIntIR*>(v)) {
        if (opnd->value_ == 0) {
            return BasicBlockASM::get_reg_var_x0();
        }
        auto* var = BasicBlockASM::get_temp_var(idx++);
        auto* instr = new Instr2RI(InstrOp::LI, var, opnd->value_);
        bb->push_back_instr(instr);
        return var;
    } else if (auto* opnd = dynamic_cast<SymbolIR*>(v)) {
        auto* tmp = bb->get_temp_var(idx++);
        auto* sp = bb->get_reg_var_sp();
        auto offset = opnd->get_offset();
        bb->create_instr<Instr3RIR>(InstrOp::LW, tmp, offset, sp);
        return tmp;
    } else {
        assert(false && "unexpected branch!");
    }
    return nullptr;
}

void StmtRetIR::gen_asm(GenASMCfg* cfg) {
    auto* bb = cfg->currentBB_;
    /*
        ret %0
            ==>
        mv  a0, t0
        ret
    */
    auto* a0 = BasicBlockASM::get_reg_var_a0();
    if (auto* v = dynamic_cast<ValueIntIR*>(value_)) {
        bb->create_instr<Instr2RI>(InstrOp::LI, a0, v->value_);
    } else if (auto* v = dynamic_cast<SymbolIR*>(value_)) {
        auto* sp = bb->get_reg_var_sp();
        auto offset = v->get_offset();
        bb->create_instr<Instr3RIR>(InstrOp::LW, a0, offset, sp);
    } else {
        assert(false && "unexpected branch!");
    }
    // bb->create_instr<Instr0>(InstrOp::RET);
}


void StmtBinaryExprIR::gen_asm(GenASMCfg* cfg) {
    auto* bb = cfg->currentBB_;
    switch (op_) {
        case BinaryOp::EQ:
            gen_asm_eq(bb);
            break;
        case BinaryOp::NE:
            gen_asm_ne(bb);
            break;
        case BinaryOp::SUB:
            gen_asm_sub(bb);
            break;
        case BinaryOp::ADD:
            gen_asm_add(bb);
            break;
        case BinaryOp::MUL:
            gen_asm_mul(bb);
            break;
        case BinaryOp::DIV:
            gen_asm_div(bb);
            break;
        case BinaryOp::MOD:
            gen_asm_mod(bb);
            break;
        case BinaryOp::LT:
            gen_asm_lt(bb);
            break;
        case BinaryOp::GT:
            gen_asm_gt(bb);
            break;
        case BinaryOp::LE:
            gen_asm_le(bb);
            break;
        case BinaryOp::GE:
            gen_asm_ge(bb);
            break;
        case BinaryOp::AND:
            gen_asm_and(bb);
            break;
        case BinaryOp::OR:
            gen_asm_or(bb);
            break;
        default:
            break;
    }
    cfg->stackOffset_ += 4;
}

Instruction* StmtBinaryExprIR::gen_asm_eq(BasicBlockASM* bb) {
    /*
        result_ = eq opnd1_ opnd2_
        =>
        %0 = xor @opnd1_ @opnd2_
        @result_ = seqz %0
    */
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (auto* var2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_ne(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (auto* var2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, ret);
        bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_add(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* reg2 = get_a_reg_var(bb, opnd2_, tmpIdx);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::ADDI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRI>(InstrOp::ADDI, tmp, reg1, var2->value_);
        }
    } else if (isa<SymbolIR*>(opnd2_)) {
        // auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::ADD, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var(1);
            bb->create_instr<Instr3RRR>(InstrOp::ADD, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);
    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_sub(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* reg2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (reg1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SUB, reg1, reg1, reg2);
    } else {
        auto* tmp = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::SUB, tmp, reg1, reg2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_mul(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::MUL, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::MUL, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_div(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::DIV, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::DIV, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_mod(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::REM, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::REM, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_lt(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_gt(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
    // set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_le(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_ge(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* var1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    auto* var2 = get_a_reg_var(bb, opnd2_, tmpIdx);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        bb->create_instr<Instr3RIR>(InstrOp::SW, ret, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_and(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    if (reg1->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = bb->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(bb, opnd2_, tmpIdx);
    if (reg2->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = bb->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::AND, reg2, reg1, reg2);
        bb->create_instr<Instr3RIR>(InstrOp::SW, reg2, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(reg2);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_or(BasicBlockASM* bb) {
    int tmpIdx { 0 };
    auto* reg1 = get_a_reg_var(bb, opnd1_, tmpIdx);
    if (reg1->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = bb->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(bb, opnd2_, tmpIdx);
    if (reg2->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var(1);
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = bb->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::OR, reg2, reg1, reg2);
        bb->create_instr<Instr3RIR>(InstrOp::SW, reg2, result_->get_offset(), BasicBlockASM::get_reg_var_sp());
        // set_ret_reg_var(reg2);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}


void StoreIR::gen_asm(GenASMCfg* cfg) {
    assert(get_ty()->isUnit());
    auto* bb = cfg->currentBB_;

    int tmpIdx { 0 };
    auto* var = get_a_reg_var(bb, src_, tmpIdx);
    assert(isa<SymbolIR*>(des_));
    auto* desVar = static_cast<SymbolIR*>(des_);
    auto* sp = bb->get_reg_var_sp();
    auto offset =  desVar->get_offset();
    bb->create_instr<Instr3RIR>(InstrOp::SW, var, offset, sp);
}


void LoadIR::gen_asm(GenASMCfg* cfg) {
    assert(get_ty()->isInt());
    auto* bb = cfg->currentBB_;

    int tmpIdx { 0 };
    auto* var = get_a_reg_var(bb, src_, tmpIdx);
    assert(isa<SymbolIR*>(des_));
    auto* desVar = static_cast<SymbolIR*>(des_);
    auto* sp = bb->get_reg_var_sp();
    auto offset =  desVar->get_offset();
    bb->create_instr<Instr3RIR>(InstrOp::SW, var, offset, sp);
}

