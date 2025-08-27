#include "StmtIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include "asm/var.hh"
#include "ir/BaseIR.hh"
#include "ir/ValueIR.hh"
#include <cstdint>


Instruction* StmtRetIR::gen_asm(BasicBlockASM* bb) {
    /*
        ret %0
            ==>
        mv  a0, t0
        ret
    */
    if (auto* ret = dynamic_cast<SymbolIR*>(value_)) {
        auto* a0 = BasicBlockASM::get_reg_var_a0();
        auto* retRegVar = ret->get_reg_var();
        auto* mvInstr = new Instr2RR(InstrOp::MV, a0, retRegVar);
        bb->push_back_instr(mvInstr);
    } else if (auto* ret = dynamic_cast<ValueIntIR*>(value_)) {
        // TODO
    }

    auto* retInstr = new Instr0(InstrOp::RET);
    bb->push_back_instr(retInstr);
    return retInstr;
}


Instruction* StmtBinaryExprIR::gen_asm(BasicBlockASM* bb) {
    switch (op_) {
        case BinaryOp::EQ:
            return gen_asm_eq(bb);
        case BinaryOp::SUB:
            return gen_asm_sub(bb);
        case BinaryOp::ADD:
            return gen_asm_add(bb);
        case BinaryOp::MUL:
            return gen_asm_mul(bb);
        default:
            break;
    }
    return nullptr;
}


Instruction* StmtBinaryExprIR::gen_asm_eq(BasicBlockASM* bb) {
    VarASM *reg1 { nullptr }, *reg2 { nullptr };
    if (auto* op1 = dynamic_cast<ValueIntIR*>(opnd1_)) {
        reg1 = BasicBlockASM::get_temp_var();
        auto* instr = new Instr2RI(InstrOp::LI, reg1, op1->value_);
        bb->push_back_instr(instr);
    }
    if (auto* op2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (op2->value_ == 0) {
            reg2 = BasicBlockASM::get_reg_var_x0();
        } else {
            // TODO
        }
    }

    bb->push_back_instr(new Instr3RRR(InstrOp::XOR, reg1, reg1, reg2));
    bb->push_back_instr(new Instr2RR(InstrOp::SEQZ, reg1, reg1));

    static_cast<SymbolIR*>(result_)->set_reg_var(reg1);     // ???

    return bb->get_current_instr();
}


Instruction* StmtBinaryExprIR::gen_asm_sub(BasicBlockASM* bb) {
    VarASM *reg1 { nullptr }, *reg2 { nullptr };
    if (auto* op1 = dynamic_cast<SymbolIR*>(opnd1_)) {
        reg1 = op1->get_reg_var();
    } else if (auto* op1 = dynamic_cast<ValueIntIR*>(opnd1_)) {
        if (op1->value_ == 0) {
            reg1 = BasicBlockASM::get_reg_var_x0();
        }
    }
    if (auto* op2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        reg2 = op2->get_reg_var();
    }

    auto* retRegVar = BasicBlockASM::get_temp_var();
    bb->push_back_instr(new Instr3RRR(InstrOp::SUB, retRegVar, reg1, reg2));

    static_cast<SymbolIR*>(result_)->set_reg_var(retRegVar);

    return bb->get_current_instr();
}


Instruction* StmtBinaryExprIR::gen_asm_mul(BasicBlockASM* bb) {
    VarASM *var1 { nullptr }, *var2 { nullptr };
    if (auto* op1 = dynamic_cast<SymbolIR*>(opnd1_)) {
        var1 = op1->get_reg_var();
    } else if (auto* op1 = dynamic_cast<ValueIntIR*>(opnd1_)) {
        var1 = i2r(bb, op1->value_);
    }
    if (auto* op2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        var2 = op2->get_reg_var();
    } else if (auto* op2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        var2 = i2r(bb, op2->value_);
    }

    auto* retRegVar = BasicBlockASM::get_temp_var();
    bb->push_back_instr(new Instr3RRR(InstrOp::MUL, retRegVar, var1, var2));

    static_cast<SymbolIR*>(result_)->set_reg_var(retRegVar);

    return bb->get_current_instr();
}


Instruction* StmtBinaryExprIR::gen_asm_add(BasicBlockASM* bb) {
    bool addi { false };
    if ((isa<SymbolIR>(opnd1_) && isa<ValueIntIR>(opnd2_)) ||
        (isa<SymbolIR>(opnd2_) && isa<ValueIntIR>(opnd1_))) {
        addi = true;
    }
    VarASM *var1 { nullptr }, *var2 { nullptr };
    uint32_t i { 0 };
    if (auto* op1 = dynamic_cast<SymbolIR*>(opnd1_)) {
        var1 = op1->get_reg_var();
    } else if (auto* op1 = dynamic_cast<ValueIntIR*>(opnd1_)) {
        if (!addi) {
            var1 = i2r(bb, op1->value_);
        } else {
            i = op1->value_;
        }
    }
    if (auto* op2 = dynamic_cast<SymbolIR*>(opnd2_)) {
        var2 = op2->get_reg_var();
    } else if (auto* op2 = dynamic_cast<ValueIntIR*>(opnd2_)) {
        if (!addi) {
            var2 = i2r(bb, op2->value_);
        } else {
            i = op2->value_;
        }
    }

    auto* retRegVar = BasicBlockASM::get_temp_var();
    if (!addi) {
        bb->push_back_instr(new Instr3RRR(InstrOp::ADD, retRegVar, var1, var2));
    } else {
        auto* var = var1 != nullptr ? var1 : var2;
        bb->push_back_instr(new Instr3RRI(InstrOp::ADDI, retRegVar, var, i));
    }

    static_cast<SymbolIR*>(result_)->set_reg_var(retRegVar);

    return bb->get_current_instr();
}

