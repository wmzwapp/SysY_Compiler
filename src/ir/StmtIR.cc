#include "StmtIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include "asm/var.hh"
#include "ir/ValueIR.hh"

static VarASM* get_a_reg_var(BasicBlockASM* bb, ValueIR* v) {
    if (auto* opnd = dynamic_cast<ValueIntIR*>(v)) {
        if (opnd->value_ == 0) {
            return BasicBlockASM::get_reg_var_x0();
        }
        auto* var = BasicBlockASM::get_temp_var();
        auto* instr = new Instr2RI(InstrOp::LI, var, opnd->value_);
        bb->push_back_instr(instr);
        return var;
    } else if (auto* opnd = dynamic_cast<SymbolIR*>(v)) {
        return opnd->get_reg_var();
    }
    return nullptr;
}

Instruction* StmtRetIR::gen_asm(BasicBlockASM* bb) {
    /*
        ret %0
            ==>
        mv  a0, t0
        ret
    */
    auto* var = get_a_reg_var(bb, value_);
    auto* a0 = BasicBlockASM::get_reg_var_a0();
    bb->create_instr<Instr2RR>(InstrOp::MV, a0, var);
    bb->create_instr<Instr0>(InstrOp::RET);

    return bb->get_current_instr();
}


Instruction* StmtBinaryExprIR::gen_asm(BasicBlockASM* bb) {
    switch (op_) {
        case BinaryOp::EQ:
            return gen_asm_eq(bb);
        case BinaryOp::NE:
            return gen_asm_ne(bb);
        case BinaryOp::SUB:
            return gen_asm_sub(bb);
        case BinaryOp::ADD:
            return gen_asm_add(bb);
        case BinaryOp::MUL:
            return gen_asm_mul(bb);
        case BinaryOp::DIV:
            return gen_asm_div(bb);
        case BinaryOp::MOD:
            return gen_asm_mod(bb);
        case BinaryOp::LT:
            return gen_asm_lt(bb);
        case BinaryOp::GT:
            return gen_asm_gt(bb);
        case BinaryOp::LE:
            return gen_asm_le(bb);
        case BinaryOp::GE:
            return gen_asm_ge(bb);
        case BinaryOp::AND:
            return gen_asm_and(bb);
        case BinaryOp::OR:
            return gen_asm_or(bb);
        default:
            break;
    }
    return nullptr;
}

Instruction* StmtBinaryExprIR::gen_asm_eq(BasicBlockASM* bb) {
       auto* reg1 = get_a_reg_var(bb, opnd1_);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (auto* var2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_ne(BasicBlockASM* bb) {
    auto* reg1 = get_a_reg_var(bb, opnd1_);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (auto* var2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, ret);
        set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_add(BasicBlockASM* bb) {
    auto* reg1 = get_a_reg_var(bb, opnd1_);
    if (auto* var2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRI>(InstrOp::ADDI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRI>(InstrOp::ADDI, tmp, reg1, var2->value_);
        }
    } else if (auto* var2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        auto* reg2 = var2->get_reg_var();
        if (reg1->is_temp()) {
            bb->create_instr<Instr3RRR>(InstrOp::ADD, reg1, reg1, reg2);
        } else {
            auto* tmp = BasicBlockASM::get_temp_var();
            bb->create_instr<Instr3RRR>(InstrOp::ADD, tmp, reg1, reg2);
        }
    }
    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);
    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_sub(BasicBlockASM* bb) {
    auto* reg1 = get_a_reg_var(bb, opnd1_);
    auto* reg2 = get_a_reg_var(bb, opnd2_);

    if (reg1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SUB, reg1, reg1, reg2);
    } else {
        auto* tmp = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::SUB, tmp, reg1, reg2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_mul(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::MUL, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::MUL, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_div(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::DIV, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::DIV, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_mod(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::REM, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::REM, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_lt(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_gt(BasicBlockASM* bb) {
        auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();
    set_ret_reg_var(ret);

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_le(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_ge(BasicBlockASM* bb) {
    auto* var1 = get_a_reg_var(bb, opnd1_);
    auto* var2 = get_a_reg_var(bb, opnd2_);

    if (var1->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = bb->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        set_ret_reg_var(ret);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_and(BasicBlockASM* bb) {
    auto* reg1 = get_a_reg_var(bb, opnd1_);
    if (reg1->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = bb->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(bb, opnd2_);
    if (reg2->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = bb->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::AND, reg2, reg1, reg2);
        set_ret_reg_var(reg2);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

Instruction* StmtBinaryExprIR::gen_asm_or(BasicBlockASM* bb) {
    auto* reg1 = get_a_reg_var(bb, opnd1_);
    if (reg1->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = bb->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(bb, opnd2_);
    if (reg2->is_temp()) {
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = BasicBlockASM::get_temp_var();
        bb->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = bb->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        bb->create_instr<Instr3RRR>(InstrOp::OR, reg2, reg1, reg2);
        set_ret_reg_var(reg2);
    } else {
        // unreachable branch
    }

    return bb->get_current_instr();
}

