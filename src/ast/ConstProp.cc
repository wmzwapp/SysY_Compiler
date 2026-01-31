#include "ConstProp.hh"
#include "BlockAST.hh"
#include "StmtAST.hh"
#include "VarAST.hh"


bool ConstPropCtx::sym_is_const(std::string sym) {
    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
        auto& symTab = (*iter)->get_sym_tab();
        if (symTab.has_sym(sym)) return symTab.is_const(sym);
    }
    assert(false);
}

int ConstPropCtx::get_sym_const(std::string sym) {
    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
        auto& symTab = (*iter)->get_sym_tab();
        if (symTab.has_sym(sym)) return symTab.get_const_val(sym);
    }
    assert(false);
}


void ConstPropCtx::push_scope(BlockAST* block) {
    scopes_.push_back(block);
    curSymTab_ = &scopes_.back()->get_sym_tab();
}

void ConstPropCtx::pop_scope() {
    scopes_.pop_back();
    if (scopes_.empty()) {
        curSymTab_ = nullptr;
    } else {
        curSymTab_ = &scopes_.back()->get_sym_tab();
    }
}

int ConstPropCtx::cal_unary_exp(OpAST op, NumberAST* opnd) {
    auto val = opnd->get_val();
    switch (op.get_op_type()) {
        case OpAST::PLUS: {
            return val;
        }
        case OpAST::MINUS: {
            return -val;
        }
        case OpAST::LNOT: {
            return !val;
        }
        default:
            throw ASTCheckFailed("Unexpected unary operator '%s', please check parser.", OpAST::repr_op_ast(op).c_str());
    }
    return 0;
}

int ConstPropCtx::cal_binary_exp(OpAST op, NumberAST* opnd1, NumberAST* opnd2) {
    auto val1 = opnd1->get_val();
    auto val2 = opnd2->get_val();
    switch (op.get_op_type()) {
        case OpAST::PLUS: {
            return val1 + val2;
        }
        case OpAST::MINUS: {
            return val1 - val2;
        }
        case OpAST::MUL: {
            return val1 * val2;
        }
        case OpAST::DIV: {
            return val1 / val2;
        }
        case OpAST::MOD: {
            return val1 % val2;
        }
        case OpAST::LT: {
            return val1 < val2;
        }
        case OpAST::GT: {
            return val1 > val2;
        }
        case OpAST::LE: {
            return val1 <= val2;
        }
        case OpAST::GE: {
            return val1 >= val2;
        }
        case OpAST::EQ: {
            return val1 == val2;
        }
        case OpAST::NEQ: {
            return val1 != val2;
        }
        case OpAST::LAND: {
            return val1 && val2;
        }
        case OpAST::LOR: {
            return val1 || val2;
        }
        default:
            throw ASTCheckFailed("Unexpected binary operator '%s', please check parser.", OpAST::repr_op_ast(op).c_str());
    }
    return 0;
}

void ConstPropVisitor::visit(BlockAST* block, VCtx* ctx) {
    auto* cpCtx = static_cast<ConstPropCtx*>(ctx);
    cpCtx->push_scope(block);

    for (auto* item : block->get_items()) {
        item->accept(this, cpCtx);
    }

    cpCtx->pop_scope();
}

void ConstPropVisitor::visit(DeclAST* decl, VCtx* ctx) {
    auto* cpCtx = static_cast<ConstPropCtx*>(ctx);
    for (auto* def : decl->get_defs()) {
        if (def->isConst_) {
            def->initExp_->accept(this, cpCtx);
            auto* retNode = cpCtx->get_ret_node();
            if (auto* retExp = dynamic_cast<ExpAST*>(retNode); retExp == nullptr || !retExp->is_num()) {
                throw ASTCheckFailed("Not a const expression '%s'.", def->initExp_->repr().c_str());
            }
            cpCtx->add_sym(def->var_->repr(), static_cast<ExpAST*>(retNode)->get_num()->get_val());
            if (def->initExp_ != retNode) {
                def->initExp_ = static_cast<ExpAST*>(retNode);
            }
        } else if (def->initExp_ != nullptr) {
            def->initExp_->accept(this, cpCtx);
            auto* retNode = cpCtx->get_ret_node();
            assert(isa<ExpAST*>(retNode));
            if (retNode != def->initExp_) {
                def->initExp_ = static_cast<ExpAST*>(retNode);
            }
        }
    }
    cpCtx->set_ret_node(nullptr);
}

void ConstPropVisitor::visit(StmtAST* stmt, VCtx* ctx) {
    auto* cpCtx = static_cast<ConstPropCtx*>(ctx);
    if (stmt->isAssignExp()) {
        auto* assignExp = stmt->getAssignExp();
        // auto* lval = stmt->getAssignExp()->get_lval();
        auto* exp = assignExp->exp_;
        exp->accept(this, cpCtx);
        auto* retNode = cpCtx->get_ret_node();
        assert(isa<ExpAST*>(retNode));
        if (retNode != exp) {
            assignExp->exp_ = (ExpAST*)retNode;
        }
    } else if (stmt->isRetExp() && stmt->getRetExp() != nullptr) {
        auto* exp = stmt->getRetExp();
        exp->accept(this, cpCtx);
        auto* retNode = cpCtx->get_ret_node();
        assert(isa<ExpAST*>(retNode));
        if (retNode != exp) {
            stmt->setRetExp(retNode);
        }
    } else if (stmt->isBlock()) {
        stmt->getBlock()->accept(this, cpCtx);
    } else if (stmt->isExp()) {
        // TODO
    } else if (stmt->isIFExp()) {
        auto* ifExp = stmt->getIFExp();
        ifExp->condExp_->accept(this, ctx);
        if (ifExp->ifStmt_) {
            ifExp->ifStmt_->accept(this, ctx);
        }
        if (ifExp->hasElse()) {
            ifExp->elseStmt_->accept(this, ctx);
        }
    } else if (stmt->isWhileExp()) {
        auto* exp = stmt->getWhileExp();
        exp->condExp_->accept(this, ctx);
        if (exp->hasStmt()) {
            exp->stmt_->accept(this, ctx);
        }
    }
    cpCtx->set_ret_node(nullptr);
}

void ConstPropVisitor::visit(ExpAST* exp, VCtx* ctx) {
    auto* cpCtx = static_cast<ConstPropCtx*>(ctx);
    if (exp->is_exp()) {
        auto* operExp = exp->get_exp();
        if (operExp->is_unary_exp()) {
            auto op = operExp->op_;
            auto* opnd2 = operExp->opnd2_;

            opnd2->accept(this, cpCtx);

            auto* retNode = cpCtx->get_ret_node();
            assert(isa<ExpAST*>(retNode));
            auto* retExp = static_cast<ExpAST*>(retNode);
            if (retExp->is_num()) {
                auto val = cpCtx->cal_unary_exp(op, retExp->get_num());
                auto* newExp = mmpool_.make<ExpAST>();
                newExp->set_num(mmpool_.make<NumberAST>(val));
                cpCtx->set_ret_node(newExp);
            } else {
                if (retNode != opnd2) {
                    operExp->opnd2_ = retExp;
                }
                cpCtx->set_ret_node(exp);
            }
        } else {
            auto* opnd1 = exp->get_exp()->opnd1_;
            auto* opnd2 = exp->get_exp()->opnd2_;
            auto op = exp->get_exp()->op_;

            opnd1->accept(this, cpCtx);
            auto* retNode1 = cpCtx->get_ret_node();
            assert(isa<ExpAST*>(retNode1));
            auto* retExp1 = static_cast<ExpAST*>(retNode1);
            
            opnd2->accept(this, cpCtx);
            auto* retNode2 = cpCtx->get_ret_node();
            assert(isa<ExpAST*>(retNode2));
            auto* retExp2 = static_cast<ExpAST*>(retNode2);

            if (retExp1->is_num() && retExp2->is_num()) {
                auto val = cpCtx->cal_binary_exp(op, retExp1->get_num(), retExp2->get_num());
                auto* newExp = mmpool_.make<ExpAST>();
                newExp->set_num(mmpool_.make<NumberAST>(val));
                cpCtx->set_ret_node(newExp);
            } else {
                if (retNode1 != opnd1) {
                    operExp->opnd1_ = retExp1;
                }
                if (retNode2 != opnd2) {
                    operExp->opnd2_ = retExp2;
                }
                cpCtx->set_ret_node(exp);
            }
        }
    } else if (exp->is_lval()) {
        auto* lval = exp->get_lval();
        auto sym = lval->repr();
        if (cpCtx->sym_is_const(sym)) {
            auto* newExp = mmpool_.make<ExpAST>();
            auto* num = mmpool_.make<NumberAST>(cpCtx->get_sym_const(sym));
            newExp->set_num(num);
            cpCtx->set_ret_node(newExp);
        } else {
            cpCtx->set_ret_node(exp);
        }
    } else if (exp->is_num()) {
        cpCtx->set_ret_node(exp);
    }
}