#include "GenASM.hh"
#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "InstrIR.hh"
#include "ValueIR.hh"
#include "asm/cfg.hh"
#include "asm/instr.hh"
#include <unordered_set>
#include <cassert>


using namespace IR;

int align_16_bytes(int v) {
    return (v + 15) & ~15;
}

void GACTX::gen_asm_func_prologue(Function* func) {
    std::unordered_set<std::string> vars;
    uint32_t stackSz { 0 };
    for (auto* block : func->get_BBs()) {
        for (auto* instr : block->get_instrs()) {
            if (!instr->isSymbolDef()) {
                continue;
            }
            auto* instrSymDef = static_cast<InstrSymDef*>(instr);
            auto* defVar = instrSymDef->get_def();
            assert(defVar != nullptr);
            auto* ty = defVar->get_ty();
            if (isa<InstrAlloc*>(instr)) {
                ty = static_cast<InstrAlloc*>(instr)->get_alloc_ty();
            }
            if (vars.find(defVar->repr()) == vars.end()) {
                defVar->set_offset(stackSz);
                stackSz += ty->get_layout_size();
                vars.insert(defVar->repr());
            }
        }
    }
    stackSz = align_16_bytes(stackSz);
    currentFunc_->set_stack_size(stackSz);
    auto* sp = get_reg_var_sp();
    if (stackSz > 0 && stackSz > (1 << (12 -1))) {
        auto* t0 = get_tmp_var();
        currentBB_->create_instr<Instr2RI>(InstrOp::LI, t0, -stackSz);
        currentBB_->create_instr<Instr3RRR>(InstrOp::ADD, sp, sp, t0);
    } else {
        currentBB_->create_instr<Instr3RRI>(InstrOp::ADDI, sp, sp, -stackSz);
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_func_epilogue() {
    auto stackSz = currentFunc_->get_stack_size();
    if (stackSz > 0) {
        auto* sp = get_reg_var_sp();
        currentBB_->create_instr<Instr3RRI>(InstrOp::ADDI, sp, sp, stackSz);
    }
}

VarASM* GACTX::get_a_reg_var(Value* v) {
    if (auto* opnd = dynamic_cast<ValueInt*>(v)) {
        if (opnd->value_ == 0) {
            return get_reg_var_x0();
        }
        auto* var = get_tmp_var();
        currentBB_->create_instr<Instr2RI>(InstrOp::LI, var, opnd->value_);
        return var;
    } else if (auto* opnd = dynamic_cast<Symbol*>(v)) {
        auto* tmp = get_tmp_var();
        auto* sp = get_reg_var_sp();
        auto offset = opnd->get_offset();
        currentBB_->create_instr<Instr3RIR>(InstrOp::LW, tmp, offset, sp);
        return tmp;
    } else {
        assert(false && "unexpected branch!");
        return nullptr;
    }
}

GenASMVisitorContest::GenASMVisitorContest() {
    x0 = mmpool_.make<VarASM>("x0");
    a0 = mmpool_.make<VarASM>("a0");
    sp = mmpool_.make<VarASM>("sp");
    tmpVars_.push_back(mmpool_.make<VarASM>("t0", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t1", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t2", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t3", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t4", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t5", true));
    tmpVars_.push_back(mmpool_.make<VarASM>("t6", true));
}

void GenASMVisitor::visit(Function* func, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* funcASM = mmpool_.make<FuncASM>();
    auto* bbASM = funcASM->create_or_get_BB(func->get_func_name());
    gctx->get_asm_top()->add_func(funcASM);
    gctx->set_current_func(funcASM);
    gctx->set_current_BB(bbASM);
    gctx->set_current_stask_offset(0);

    gctx->gen_asm_func_prologue(func);
    for (auto* block : func->get_BBs()) {
        block->accept(this, ctx);
    }
}

void GenASMVisitor::visit(Block* block, IVCtx* ctx) {
    if (block->get_instrs().empty()) {
        // TODO: should remove empty block IR
        return;
    }

    auto* gctx = static_cast<GACTX*>(ctx);
    auto* func = gctx->get_current_func();
    auto* bb = func->create_or_get_BB(block->get_symbol()->repr());
    gctx->set_current_BB(bb);
    // for (auto* stmt : block->get_instrs()) {
    //     stmt->accept(this, ctx);
    // }
    auto* instr = block->get_entry_instr();
    while (instr) {
        instr->accept(this, ctx);
        instr = instr->get_next_instr();
    }
}

void GenASMVisitor::visit(InstrRet* stmt, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* bb = gctx->get_current_BB();
    /*
        ret %0
            ==>
        mv  a0, t0
        ret
    */
    auto* a0 = gctx->get_reg_var_a0();
    if (auto* v = dynamic_cast<ValueInt*>(stmt->get_value())) {
        bb->create_instr<Instr2RI>(InstrOp::LI, a0, v->value_);
    } else if (auto* v = dynamic_cast<Symbol*>(stmt->get_value())) {
        auto* sp = gctx->get_reg_var_sp();
        auto offset = v->get_offset();
        bb->create_instr<Instr3RIR>(InstrOp::LW, a0, offset, sp);
    } else {
        assert(false && "unexpected branch!");
    }
    gctx->gen_asm_func_epilogue();
    gctx->get_current_BB()->create_instr<Instr0>(InstrOp::RET);
}

void GenASMVisitor::visit(InstrBExpr* stmt, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    switch (stmt->get_op()) {
        case BinaryOp::EQ:
            gctx->gen_asm_binary_stmt_eq(stmt);
            break;
        case BinaryOp::NE:
            gctx->gen_asm_binary_stmt_ne(stmt);
            break;
        case BinaryOp::SUB:
            gctx->gen_asm_binary_stmt_sub(stmt);
            break;
        case BinaryOp::ADD:
            gctx->gen_asm_binary_stmt_add(stmt);
            break;
        case BinaryOp::MUL:
            gctx->gen_asm_binary_stmt_mul(stmt);
            break;
        case BinaryOp::DIV:
            gctx->gen_asm_binary_stmt_div(stmt);
            break;
        case BinaryOp::MOD:
            gctx->gen_asm_binary_stmt_mod(stmt);
            break;
        case BinaryOp::LT:
            gctx->gen_asm_binary_stmt_lt(stmt);
            break;
        case BinaryOp::GT:
            gctx->gen_asm_binary_stmt_gt(stmt);
            break;
        case BinaryOp::LE:
            gctx->gen_asm_binary_stmt_le(stmt);
            break;
        case BinaryOp::GE:
            gctx->gen_asm_binary_stmt_ge(stmt);
            break;
        case BinaryOp::AND:
            gctx->gen_asm_binary_stmt_and(stmt);
            break;
        case BinaryOp::OR:
            gctx->gen_asm_binary_stmt_or(stmt);
            break;
        default:
            break;
    }
}

void GACTX::gen_asm_binary_stmt_eq(InstrBExpr* stmt) {
    /*
        result_ = eq opnd1_ opnd2_
        =>
        %0 = xor @opnd1_ @opnd2_
        @result_ = seqz %0
    */
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    auto* reg2 = get_a_reg_var(opnd2);
    if (auto* var2 = dynamic_cast<ValueInt*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (isa<Symbol*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = currentBB_->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_ne(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    auto* reg2 = get_a_reg_var(opnd2);
    if (auto* var2 = dynamic_cast<ValueInt*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRI>(InstrOp::XORI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRI>(InstrOp::XORI, tmp, reg1, var2->value_);
        }
    } else if (isa<Symbol*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRR>(InstrOp::XOR, reg1, reg1, reg2);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRR>(InstrOp::XOR, tmp, reg1, reg2);
        }
    }
    auto* ret = currentBB_->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, ret, ret);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_add(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    auto* reg2 = get_a_reg_var(opnd2);
    if (auto* var2 = dynamic_cast<ValueInt*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRI>(InstrOp::ADDI, reg1, reg1, var2->value_);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRI>(InstrOp::ADDI, tmp, reg1, var2->value_);
        }
    } else if (isa<Symbol*>(opnd2)) {
        if (reg1->is_temp()) {
            currentBB_->create_instr<Instr3RRR>(InstrOp::ADD, reg1, reg1, reg2);
        } else {
            auto* tmp = get_tmp_var();
            currentBB_->create_instr<Instr3RRR>(InstrOp::ADD, tmp, reg1, reg2);
        }
    }
    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_sub(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    auto* reg2 = get_a_reg_var(opnd2);
    if (reg1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::SUB, reg1, reg1, reg2);
    } else {
        auto* tmp = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::SUB, tmp, reg1, reg2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_mul(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::MUL, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::MUL, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_div(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::DIV, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::DIV, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_mod(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::REM, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::REM, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_lt(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_gt(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();
    currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_le(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::SGT, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::SGT, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_ge(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* var1 = get_a_reg_var(opnd1);
    auto* var2 = get_a_reg_var(opnd2);
    if (var1->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::SLT, var1, var1, var2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr3RRR>(InstrOp::SLT, ret, var1, var2);
    }

    auto* ret = currentBB_->get_current_instr()->get_ret();

    if (ret->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SEQZ, ret, ret);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, ret, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_and(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    if (reg1->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = currentBB_->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(opnd2);
    if (reg2->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = currentBB_->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::AND, reg2, reg1, reg2);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, reg2, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GACTX::gen_asm_binary_stmt_or(InstrBExpr* stmt) {
    auto* opnd1 = stmt->get_opnd1();
    auto* opnd2 = stmt->get_opnd2();
    auto* result = stmt->get_def();

    auto* reg1 = get_a_reg_var(opnd1);
    if (reg1->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, reg1, reg1);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg1);
    }
    reg1 = currentBB_->get_current_instr()->get_ret();

    auto* reg2 = get_a_reg_var(opnd2);
    if (reg2->is_temp()) {
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, reg2, reg2);
    } else {
        auto* ret = get_tmp_var();
        currentBB_->create_instr<Instr2RR>(InstrOp::SNEZ, ret, reg2);
    }
    reg2 = currentBB_->get_current_instr()->get_ret();

    if (reg2->is_temp()) {
        currentBB_->create_instr<Instr3RRR>(InstrOp::OR, reg2, reg1, reg2);
        currentBB_->create_instr<Instr3RIR>(InstrOp::SW, reg2, result->get_offset(), get_reg_var_sp());
    } else {
        // unreachable branch
    }
    return_all_tmp_var();
}

void GenASMVisitor::visit(InstrStore* stmt, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* bb = gctx->get_current_BB();

    auto* var = gctx->get_a_reg_var(stmt->get_src());
    assert(isa<Symbol*>(stmt->get_des()));
    auto* desVar = static_cast<Symbol*>(stmt->get_des());
    auto* sp = gctx->get_reg_var_sp();
    auto offset =  desVar->get_offset();
    bb->create_instr<Instr3RIR>(InstrOp::SW, var, offset, sp);
    gctx->return_all_tmp_var();
}

void GenASMVisitor::visit(InstrLoad* stmt, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* bb = gctx->get_current_BB();

    auto* var = gctx->get_a_reg_var(stmt->get_src());
    assert(isa<Symbol*>(stmt->get_def()));
    auto* desVar = static_cast<Symbol*>(stmt->get_def());
    auto* sp = gctx->get_reg_var_sp();
    auto offset =  desVar->get_offset();
    bb->create_instr<Instr3RIR>(InstrOp::SW, var, offset, sp);
    gctx->return_all_tmp_var();
}

void GenASMVisitor::visit(InstrBr* instr, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* bb = gctx->get_current_BB();

    auto* var = gctx->get_a_reg_var(instr->get_value());
    auto trueLable = instr->get_true_branch()->get_symbol()->repr();
    bb->create_instr<Instr2RS>(InstrOp::BNEZ, var, trueLable);
    auto falseLabel = instr->get_false_branch()->get_symbol()->repr();
    bb->create_instr<Instr1S>(InstrOp::J, falseLabel);
    gctx->return_all_tmp_var();
}

void GenASMVisitor::visit(InstrJump* instr, IVCtx* ctx) {
    auto* gctx = static_cast<GACTX*>(ctx);
    auto* bb = gctx->get_current_BB();

    auto label = instr->get_branch()->get_symbol()->repr();
    bb->create_instr<Instr1S>(InstrOp::J, label);
}

