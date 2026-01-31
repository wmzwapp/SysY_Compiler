#include "GenIR.hh"
#include "FuncDefAST.hh"
#include "BlockAST.hh"
#include "StmtAST.hh"
#include "common/mmp.hh"
#include "common/utils.hh"
#include "ir/ProgramIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/BlockIR.hh"
#include "ir/TypeIR.hh"
#include "ir/ValueIR.hh"
#include "ir/InstrIR.hh"
#include <cassert>


void GenIRVisitor::visit(FuncDefAST* func, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);

    auto* type = mmpool_.make<IR::TypeFunc>();
    auto* retType = func->get_ret_type()->get_ty_IR();
    type->set_ret_type(retType);

    auto* sym = mmpool_.make<IR::Symbol>(func->get_ident()->repr(), type);
    auto* funcIR = mmpool_.make<IR::Function>(sym);

    type->set_prototype(funcIR);

    gctx->get_current_programIR()->add_func(funcIR);
    gctx->set_current_functionIR(funcIR);

    auto* bb = funcIR->create_entry_BB();
    funcIR->create_exit_BB();
    gctx->set_current_blockIR(bb);

    func->get_block()->accept(this, gctx);
}

void GenIRVisitor::visit(BlockItemAST* item, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);

    if (item->is_decl()) {
        item->get_decl_ast()->accept(this, gctx);
    } else if (item->is_stmt()) {
        item->get_stmt_ast()->accept(this, gctx);
    }
}

void GenIRVisitor::visit(DeclAST* decl, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();
    auto* curB = gctx->get_current_blockIR();
    if (curB->is_terminated()) {
        return;
    }

    for (auto* def : decl->get_defs()) {
        if (def->isConst_) {
            continue;
        }
        if (def->initExp_ != nullptr) {
            def->initExp_->accept(this, gctx);
            curB = gctx->get_current_blockIR();
            auto* initval = gctx->get_current_value();
            auto* ty = def->type_.get_ty_IR();
            auto* ptrTy = mmpool_.make<IR::TypePtr>(ty);
            auto* symbol = mmpool_.make<IR::Symbol>(def->var_->repr(), ptrTy);

            curB->create_instr<IR::InstrAlloc>(symbol, ty);            
            curB->create_instr<IR::InstrStore>(initval, symbol);

            gctx->set_current_value(symbol);
            def->var_->set_IR_var(symbol);
            funcIR->add_var(def->var_->repr(), symbol);
        } else {
            auto* ty = def->type_.get_ty_IR();
            auto* ptrTy = mmpool_.make<IR::TypePtr>(ty);
            auto* symbol = mmpool_.make<IR::Symbol>(def->var_->repr(), ptrTy);

            curB->create_instr<IR::InstrAlloc>(symbol, ty);

            def->var_->set_IR_var(symbol);
            gctx->set_current_value(symbol);
            funcIR->add_var(def->var_->repr(), symbol);
        }
    }
}

void GenIRVisitor::visit(StmtAST* stmt, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();
    auto* curB = gctx->get_current_blockIR();
    if (curB->is_terminated()) {
        return;
    }

    if (stmt->isRetExp()) {
        process_return_stmt(stmt, gctx);
    } else if (stmt->isExp()) {
        stmt->getExp()->accept(this, gctx);
    } else if (stmt->isAssignExp()) {
        auto* assignExp = stmt->getAssignExp();
        assignExp->exp_->accept(this, gctx);
        auto* retVal = gctx->get_current_value();
        curB = gctx->get_current_blockIR();

        auto* lval = assignExp->lval_;
        auto* var = funcIR->get_var(lval->repr());

        curB->create_instr<IR::InstrStore>(retVal, var);
    } else if (stmt->isBlock()) {
        stmt->getBlock()->accept(this, gctx);
    } else if (stmt->isIFExp()) {
        auto* ifexp = stmt->getIFExp();
        if (ifexp->ifStmt_ != nullptr || ifexp->elseStmt_ != nullptr) {
            process_ifelse_stmt(stmt, gctx);
        }
    } else if (stmt->isWhileExp()) {
        auto* exp = stmt->getWhileExp();
        process_while_stmt(exp, gctx);
    } else if (stmt->isBreakExp()) {
        process_break_continue_stmt(true, gctx);
    } else if (stmt->isContinueExp()) {
        process_break_continue_stmt(false, gctx);
    }
}

void GenIRVisitor::process_return_stmt(StmtAST* stmt, GenIRCtx* ctx) {
    auto* funcIR = ctx->get_current_functionIR();
    if (stmt->getRetExp() == nullptr) {
        auto* retval = mmpool_.make<IR::ValueUndef>(funcIR->get_return_type());
        ctx->get_current_blockIR()->create_instr<IR::InstrStore>(retval, funcIR->get_return_var());
    } else {
        stmt->getRetExp()->accept(this, ctx);
        auto* retval = ctx->get_current_value();
        ctx->get_current_blockIR()->create_instr<IR::InstrStore>(retval, funcIR->get_return_var());
    }
    ctx->get_current_blockIR()->create_instr<IR::InstrJump>(funcIR->get_exit_BB());
}

void link_bb(IR::Block* prevB, IRBlock auto*... nextBs) {
    (..., (prevB->add_next_block(nextBs), nextBs->add_pre_block(prevB)));
}

void GenIRVisitor::process_ifelse_stmt(StmtAST* stmt, GenIRCtx* ctx) {
    auto* funcIR = ctx->get_current_functionIR();
    auto* ifExp = stmt->getIFExp();
    ifExp->condExp_->accept(this, ctx);
    auto* curB = ctx->get_current_blockIR();
    auto* value = ctx->get_current_value();
    assert(isa<IR::ValueInt*>(value) || isa<IR::Symbol*>(value));
    auto* thenB = funcIR->create_BB();
    auto* elseB = ifExp->hasElse() ? funcIR->create_BB() : nullptr;
    auto* mergeB = funcIR->create_BB();
    if (ifExp->hasElse()) {
        curB->create_instr<IR::InstrBr>(value, thenB, elseB);
        link_bb(curB, thenB, elseB);
    } else {
        curB->create_instr<IR::InstrBr>(value, thenB, mergeB);
        link_bb(curB, thenB, mergeB);
    }
    // true branch
    ctx->set_current_blockIR(thenB);
    ifExp->ifStmt_->accept(this, ctx);
    curB = ctx->get_current_blockIR();
    if (!curB->is_terminated()) {
        curB->create_instr<IR::InstrJump>(mergeB);
        link_bb(curB, mergeB);
    } else {
        link_bb(curB, funcIR->get_exit_BB());
    }
    if (ifExp->hasElse()) {
        // false branch
        ctx->set_current_blockIR(elseB);
        ifExp->elseStmt_->accept(this, ctx);
        curB = ctx->get_current_blockIR();
        if (!curB->is_terminated()) {
            curB->create_instr<IR::InstrJump>(mergeB);
            link_bb(curB, mergeB);
        } else {
            link_bb(curB, funcIR->get_exit_BB());
        }
    }
    ctx->set_current_blockIR(mergeB);
}

void GenIRVisitor::process_short_circuit_eval(OperExpAST* expr, GenIRCtx* ctx) {
    /*
        ret = lhs || rhs
        =>
        int ret = 1
        if (lhs == 0) ret = rhs != 0;

        ret = lhs && rhs
        =>
        int ret = 0
        if (lhs == 1) ret = rhs != 0;
    */
    auto op = expr->op_.get_op_type();
    assert(op == OpAST::LOR || op == OpAST::LAND);

    expr->opnd1_->accept(this, ctx);
    auto* opnd1V = ctx->get_current_value();
    auto* funcIR = ctx->get_current_functionIR();
    auto* curB = ctx->get_current_blockIR();

    // int ret = 1/0;
    auto* intTy = mmpool_.make<IR::TypeInt>();
    auto* retVar = funcIR->get_tmp_var(mmpool_.make<IR::TypePtr>(intTy));
    curB->create_instr<IR::InstrAlloc>(retVar, intTy);
    auto* val = mmpool_.make<IR::ValueInt>(op == OpAST::LOR ? 1 : 0);
    curB->create_instr<IR::InstrStore>(val, retVar);

    // if (lhs == 0/1)
    val = mmpool_.make<IR::ValueInt>(op == OpAST::LOR ? 0 : 1);
    auto* condVar = funcIR->get_tmp_var(intTy);
    curB->create_instr<IR::InstrBExpr>(IR::BinaryOp::EQ, condVar, opnd1V, val);
    auto* thenB = funcIR->create_BB();
    auto* mergeB = funcIR->create_BB();
    curB->create_instr<IR::InstrBr>(condVar, thenB, mergeB);
    link_bb(curB, thenB, mergeB);

    // eval rhs
    ctx->set_current_blockIR(thenB);
    expr->opnd2_->accept(this, ctx);
    curB = ctx->get_current_blockIR();
    auto* opnd2V = ctx->get_current_value();
    if (curB->is_terminated()) {
        assert(false);
    }

    // ret = rhs != 0;
    auto* tmpVar = funcIR->get_tmp_var(intTy);
    curB->create_instr<IR::InstrBExpr>(IR::BinaryOp::NE, tmpVar, opnd2V, mmpool_.make<IR::ValueInt>(0));
    curB->create_instr<IR::InstrLoad>(retVar, tmpVar);
    curB->create_instr<IR::InstrJump>(mergeB);
    link_bb(curB, mergeB);

    ctx->set_current_value(retVar);
    ctx->set_current_blockIR(mergeB);
}

void GenIRVisitor::process_while_stmt(WhileExp* stmt, GenIRCtx* ctx) {
    auto* func = ctx->get_current_functionIR();
    auto* cb = ctx->get_current_blockIR();
    auto* entryB = func->create_BB();
    auto* bodyB = func->create_BB();
    auto* mergedB = func->create_BB();
    auto* preB1 = ctx->get_while_entry_block();
    auto* preB2 = ctx->get_while_merge_block();
    ctx->set_while_entry_block(entryB);
    ctx->set_while_merge_block(mergedB);

    cb->create_instr<IR::InstrJump>(entryB);
    link_bb(cb, entryB);

    ctx->set_current_blockIR(entryB);
    stmt->condExp_->accept(this, ctx);
    auto* v = ctx->get_current_value();
    ctx->get_current_blockIR()->create_instr<IR::InstrBr>(v, bodyB, mergedB);
    link_bb(ctx->get_current_blockIR(), bodyB, mergedB);

    ctx->set_current_blockIR(bodyB);
    if (stmt->hasStmt()) {
        stmt->stmt_->accept(this, ctx);
    }
    ctx->get_current_blockIR()->create_instr<IR::InstrJump>(entryB);
    link_bb(ctx->get_current_blockIR(), entryB);

    ctx->set_current_blockIR(mergedB);
    ctx->set_while_entry_block(preB1);
    ctx->set_while_merge_block(preB2);
}

void GenIRVisitor::process_break_continue_stmt(bool isBreak, GenIRCtx* ctx) {
    if (isBreak) {
        ctx->get_current_blockIR()->create_instr<IR::InstrJump>(ctx->get_while_merge_block());
    } else {
        ctx->get_current_blockIR()->create_instr<IR::InstrJump>(ctx->get_while_entry_block());
    }
}

void GenIRVisitor::visit(ExpAST* exp, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();

    if (exp->is_exp()) {
        auto* operExp = exp->get_exp();
        if (operExp->is_unary_exp()) {
            operExp->opnd2_->accept(this, gctx);
            auto* val = gctx->get_current_value();
            auto op = operExp->op_;

            if (op.get_op_type() == OpAST::LNOT) {
                auto* ret = funcIR->get_tmp_var(mmpool_.make<IR::TypeInt>());
                auto* value0 = mmpool_.make<IR::ValueInt>(0);
                gctx->get_current_blockIR()->create_instr<IR::InstrBExpr>(IR::BinaryOp::EQ, ret, val, value0);
                gctx->set_current_value(ret);
            } else if (op.get_op_type() == OpAST::MINUS) {
                auto* ret = funcIR->get_tmp_var(mmpool_.make<IR::TypeInt>());
                auto* value0 = mmpool_.make<IR::ValueInt>(0);
                gctx->get_current_blockIR()->create_instr<IR::InstrBExpr>(IR::BinaryOp::SUB, ret, value0, val);
                gctx->set_current_value(ret);
            }
        } else {
            auto op = operExp->op_.get_op_type();
            if (op == OpAST::LAND || op == OpAST::LOR) {
                process_short_circuit_eval(operExp, gctx);
            } else {
                operExp->opnd1_->accept(this, gctx);
                auto* val1 = gctx->get_current_value();
                operExp->opnd2_->accept(this, gctx);
                auto* val2 = gctx->get_current_value();
                auto OpIR { IR::BinaryOp::BAD };
                switch (op) {
                    case OpAST::PLUS:   OpIR = IR::BinaryOp::ADD;   break;
                    case OpAST::MINUS:  OpIR = IR::BinaryOp::SUB;   break;
                    case OpAST::MUL:    OpIR = IR::BinaryOp::MUL;   break;
                    case OpAST::DIV:    OpIR = IR::BinaryOp::DIV;   break;
                    case OpAST::MOD:    OpIR = IR::BinaryOp::MOD;   break;
                    case OpAST::LT:     OpIR = IR::BinaryOp::LT;    break;
                    case OpAST::LE:     OpIR = IR::BinaryOp::LE;    break;
                    case OpAST::GT:     OpIR = IR::BinaryOp::GT;    break;
                    case OpAST::GE:     OpIR = IR::BinaryOp::GE;    break;
                    case OpAST::EQ:     OpIR = IR::BinaryOp::EQ;    break;
                    case OpAST::NEQ:    OpIR = IR::BinaryOp::NE;    break;
                    default:
                    break;
                }
                auto* ret = funcIR->get_tmp_var(mmpool_.make<IR::TypeInt>());
                gctx->get_current_blockIR()->create_instr<IR::InstrBExpr>(OpIR, ret, val1, val2);
                gctx->set_current_value(ret);
            }
        }
    } else if (exp->is_lval()) {
        exp->get_lval()->accept(this, gctx);
    } else if (exp->is_num()) {
        exp->get_num()->accept(this, gctx);
    }
}

void GenIRVisitor::visit(VarAST* var, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();

    auto* defVar = var->get_IR_var();
    auto* ty = defVar->get_ty();
    if (ty->isPointer()) {
        ty = ((IR::TypePtr*)ty)->get_source();
    }
    auto* tmpVar = funcIR->get_tmp_var(ty);
    gctx->get_current_blockIR()->create_instr<IR::InstrLoad>(tmpVar, defVar);
    gctx->set_current_value(tmpVar);
}

void GenIRVisitor::visit(NumberAST* num, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* numVar = mmpool_.make<IR::ValueInt>(num->get_val());
    gctx->set_current_value(numVar);
}
