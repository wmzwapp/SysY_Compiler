#include "GenIR.hh"
#include "FuncDefAST.hh"
#include "BlockAST.hh"
#include "StmtAST.hh"
#include "ir/ProgramIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/BlockIR.hh"
#include "ir/TypeIR.hh"
#include "ir/ValueIR.hh"
#include "ir/StmtIR.hh"


void GenIRVisitor::visit(FuncDefAST* func, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);

    auto* type = mmpool_.make<TypeFuncIR>();
    auto* retType = func->get_ret_type()->get_ty_IR();
    type->set_ret_type(retType);

    auto* sym = mmpool_.make<SymbolIR>(func->get_ident()->repr(), mmpool_.make<TypeUnitIR>());
    auto* funcIR = mmpool_.make<FunctionIR>(sym, type);

    type->set_prototype(funcIR);

    gctx->get_current_programIR()->add_func(funcIR);
    gctx->set_current_functionIR(funcIR);

    auto* entryBB = mmpool_.make<BlockIR>("enty");
    funcIR->add_BB(entryBB);
    gctx->set_current_blockIR(entryBB);

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
    auto* bbIR = gctx->get_current_blockIR();

    for (auto* def : decl->get_defs()) {
        if (def->isConst_) {
            continue;
        }
        if (def->initExp_ != nullptr) {
            def->initExp_->accept(this, gctx);
            auto* initval = gctx->get_current_value();
            
            auto* ptrTy = mmpool_.make<TypePtrIR>(def->type_.get_ty_IR());
            auto* symbol = mmpool_.make<SymbolIR>(def->var_->repr(), ptrTy);
            auto* allocIR = mmpool_.make<AllocIR>(symbol, def->type_.get_ty_IR());
            def->var_->set_IR_var(symbol);
            bbIR->add_stmt(allocIR);
            gctx->set_current_value(symbol);
            funcIR->add_var(def->var_->repr(), symbol);

            auto* storeIR = mmpool_.make<StoreIR>(initval, symbol);
            bbIR->add_stmt(storeIR);
        } else {
            auto* ptrTy = mmpool_.make<TypePtrIR>(def->type_.get_ty_IR());
            auto* symbol = mmpool_.make<SymbolIR>(def->var_->repr(), ptrTy);
            auto* allocIR = mmpool_.make<AllocIR>(symbol, def->type_.get_ty_IR());
            def->var_->set_IR_var(symbol);
            bbIR->add_stmt(allocIR);
            gctx->set_current_value(symbol);
            funcIR->add_var(def->var_->repr(), symbol);
        }
    }
}

void GenIRVisitor::visit(StmtAST* stmt, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();
    auto* bbIR = gctx->get_current_blockIR();

    if (stmt->isRetExp()) {
        ValueIR* retval { nullptr };
        if (stmt->getRetExp() != nullptr) {
            stmt->getRetExp()->accept(this, gctx);
            retval = gctx->get_current_value();
        }
        auto* retIR = mmpool_.make<StmtRetIR>(retval);
        bbIR->add_stmt(retIR);
        bbIR->add_end_stmt(retIR);
    } else if (stmt->isExp()) {
        stmt->getExp()->accept(this, gctx);
    } else if (stmt->isAssignExp()) {
        auto* assignExp = stmt->getAssignExp();
        assignExp->exp_->accept(this, gctx);
        auto* retVal = gctx->get_current_value();

        auto* lval = assignExp->lval_;
        auto* var = funcIR->get_var(lval->repr());

        auto* storeIR = mmpool_.make<StoreIR>(retVal, var);
        bbIR->add_stmt(storeIR);
    } else if (stmt->isBlock()) {
        stmt->getBlock()->accept(this, gctx);
    }
}

void GenIRVisitor::visit(ExpAST* exp, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* funcIR = gctx->get_current_functionIR();
    auto* bbIR = gctx->get_current_blockIR();

    if (exp->is_exp()) {
        auto* operExp = exp->get_exp();
        if (operExp->is_unary_exp()) {
            operExp->opnd2_->accept(this, gctx);
            auto* val = gctx->get_current_value();
            auto op = operExp->op_;

            if (op.get_op_type() == OpAST::LNOT) {
                auto* ret = funcIR->get_tmp_var(mmpool_.make<TypeIntIR>());
                auto* value0 = mmpool_.make<ValueIntIR>(0);
                auto* stmt = mmpool_.make<StmtBinaryExprIR>(BinaryOp::EQ, ret, val, value0);
                bbIR->add_stmt(stmt);
                gctx->set_current_value(ret);
            } else if (op.get_op_type() == OpAST::MINUS) {
                auto* ret = funcIR->get_tmp_var(mmpool_.make<TypeIntIR>());
                auto* value0 = mmpool_.make<ValueIntIR>(0);
                auto* stmt = mmpool_.make<StmtBinaryExprIR>(BinaryOp::SUB, ret, value0, val);
                bbIR->add_stmt(stmt);
                gctx->set_current_value(ret);
            }
        } else {
            operExp->opnd1_->accept(this, gctx);
            auto* val1 = gctx->get_current_value();
            operExp->opnd2_->accept(this, gctx);
            auto* val2 = gctx->get_current_value();
            auto OpIR { BinaryOp::BAD };
            switch (operExp->op_.get_op_type()) {
                case OpAST::PLUS:   OpIR = BinaryOp::ADD;   break;
                case OpAST::MINUS:  OpIR = BinaryOp::SUB;   break;
                case OpAST::MUL:    OpIR = BinaryOp::MUL;   break;
                case OpAST::DIV:    OpIR = BinaryOp::DIV;   break;
                case OpAST::MOD:    OpIR = BinaryOp::MOD;   break;
                case OpAST::LT:     OpIR = BinaryOp::LT;    break;
                case OpAST::LE:     OpIR = BinaryOp::LE;    break;
                case OpAST::GT:     OpIR = BinaryOp::GT;    break;
                case OpAST::GE:     OpIR = BinaryOp::GE;    break;
                case OpAST::EQ:     OpIR = BinaryOp::EQ;    break;
                case OpAST::NEQ:    OpIR = BinaryOp::NE;    break;
                case OpAST::LAND:   OpIR = BinaryOp::AND;   break;
                case OpAST::LOR:    OpIR = BinaryOp::OR;    break;
                default:
                    break;
            }
            auto* ret = funcIR->get_tmp_var(mmpool_.make<TypeIntIR>());
            auto* stmt = mmpool_.make<StmtBinaryExprIR>(OpIR, ret, val1, val2);
            bbIR->add_stmt(stmt);
            gctx->set_current_value(ret);
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
    auto* bbIR = gctx->get_current_blockIR();

    auto* defVar = var->get_IR_var();
    auto* ty = defVar->get_ty();
    if (ty->isPointer()) {
        ty = ((TypePtrIR*)ty)->get_source();
    }
    auto* tmpVar = funcIR->get_tmp_var(ty);
    auto* loadIR = mmpool_.make<LoadIR>(tmpVar, defVar, ty);
    gctx->set_current_value(tmpVar);
    bbIR->add_stmt(loadIR);
}

void GenIRVisitor::visit(NumberAST* num, VCtx* ctx) {
    auto* gctx = static_cast<GenIRCtx*>(ctx);
    auto* numVar = mmpool_.make<ValueIntIR>(num->get_val());
    gctx->set_current_value(numVar);
}
