#include "SemanticCheck.hh"
#include "FuncDefAST.hh"
#include "BlockAST.hh"
#include "StmtAST.hh"
#include "VarAST.hh"

bool ScopeCheckCtx::has_sym(std::string ident, bool glb) {
    if (!glb) return curSymTab_->has_sym(ident);

    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
        auto* block = *iter;
        auto& symTab = block->get_sym_tab();
        if (symTab.has_sym(ident)) return true;
    }
    return false;
}

VarAST* ScopeCheckCtx::get_sym_var(std::string sym, bool glb) {
    if (!glb) {
        if (curSymTab_->has_sym(sym)) {
            return curSymTab_->get_var(sym);
        } else {
            return nullptr;
        }
    } else {
        for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
            auto* block = *iter;
            auto& symTab = block->get_sym_tab();
            if (symTab.has_sym(sym)) {
                return symTab.get_var(sym);
            }
        }
    }
    return nullptr;
}

bool ScopeCheckCtx::sym_is_const(std::string ident) {
    for (auto iter = scopes_.rbegin(); iter != scopes_.rend(); ++iter) {
        auto* block = *iter;
        auto& symTab = block->get_sym_tab();
        if (symTab.has_sym(ident) && symTab.is_const(ident)) return true;
    }
    return false;
}

void ScopeCheckCtx::push_scope(BlockAST* block) {
    scopes_.push_back(block);
    curSymTab_ = &scopes_.back()->get_sym_tab();
}

void ScopeCheckCtx::pop_scope() {
    scopes_.pop_back();
    if (scopes_.empty()) {
        curSymTab_ = nullptr;
    } else {
        curSymTab_ = &scopes_.back()->get_sym_tab();
    }
}

void ScopeCheckCtx::handle_same_name_var_within_func() {
    for (auto& [sym, tabs] : namedVarRecord_) {
        if (tabs.size() == 1) {
            continue;
        }
        unsigned int count = 0;
        for (auto tab : tabs) {
            auto* var = tab->get_var(sym);
            auto newSym = sym + "_" + std::to_string(++count);
            var->set_ident(newSym);
            tab->add_sym(newSym, var);
            tab->remove_sym(sym);
        }
    }
}

void ScopeCheckerVisitor::visit(FuncDefAST* func, VCtx* ctx) {
    auto* scCtx = static_cast<ScopeCheckCtx*>(ctx);
    func->get_block()->accept(this, ctx);
    scCtx->handle_same_name_var_within_func();
    scCtx->namedVarRecord_.clear();
}

void ScopeCheckerVisitor::visit(BlockAST* block, VCtx* ctx) {
    auto* scCtx = static_cast<ScopeCheckCtx*>(ctx);
    scCtx->push_scope(block);

    for (auto* item : block->get_items()) {
        item->accept(this, ctx);
    }

    scCtx->pop_scope();
}

void ScopeCheckerVisitor::visit(DeclAST* decl, VCtx* ctx) {
    auto* scCtx = static_cast<ScopeCheckCtx*>(ctx);
    for (auto* def : decl->get_defs()) {
        auto ident = def->var_->repr();
        if (scCtx->has_sym(ident, false)) {
            throw ASTCheckFailed("[Scope Check] Re-defined symbol '%s'.", ident.c_str());
        }
        if (def->isConst_) {
            scCtx->add_sym(ident, INT32_MIN);
            def->initExp_->accept(this, scCtx);
        } else {
            scCtx->add_sym(ident, def->var_);
            if (def->initExp_ != nullptr) {
                def->initExp_->accept(this, scCtx);
            }
        }
    }
}

void ScopeCheckerVisitor::visit(StmtAST* stmt, VCtx* ctx) {
    auto* scCtx = static_cast<ScopeCheckCtx*>(ctx);
    if (stmt->isAssignExp()) {
        auto* asg = stmt->getAssignExp();
        auto lvalIdent = asg->lval_->repr();
        if (!scCtx->has_sym(lvalIdent)) {
            throw ASTCheckFailed("[Scope Check] Undefiend symbol '%s'.", lvalIdent.c_str());
        } else if (scCtx->sym_is_const(lvalIdent)) {
            throw ASTCheckFailed("[Scope Check] Use const var '%s' as LVal.", lvalIdent.c_str());
        } else {
            asg->lval_ = scCtx->get_sym_var(lvalIdent);
            asg->exp_->accept(this, scCtx);
        }
    } else if (stmt->isExp()) {
        stmt->getExp()->accept(this, scCtx);
    } else if (stmt->isRetExp() && stmt->getRetExp() != nullptr) {
        stmt->getRetExp()->accept(this, scCtx);
    } else if (stmt->isBlock()) {
        stmt->getBlock()->accept(this, scCtx);
    } else if (stmt->isIFExp()) {
        auto* ifExp = stmt->getIFExp();
        ifExp->condExp_->accept(this, scCtx);
        if (ifExp->ifStmt_) {
            ifExp->ifStmt_->accept(this, scCtx);
        }
        if (ifExp->hasElse()) {
            ifExp->elseStmt_->accept(this, scCtx);
        }
    } else if (stmt->isWhileExp()) {
        auto* exp = stmt->getWhileExp();
        auto v = scCtx->is_under_while();
        scCtx->set_is_under_while(true);
        exp->condExp_->accept(this, ctx);
        if (exp->hasStmt()) {
            exp->stmt_->accept(this, ctx);
        }
        scCtx->set_is_under_while(v);
    } else if (stmt->isBreakExp() || stmt->isContinueExp()) {
        if (!scCtx->is_under_while()) {
            throw ASTCheckFailed("[Scope check] unexpected expression '%s'.", stmt->repr().c_str());
        }
    }
}

void ScopeCheckerVisitor::visit(ExpAST* exp, VCtx* ctx) {
    auto* scCtx = static_cast<ScopeCheckCtx*>(ctx);
    if (exp->is_exp()) {
        auto* opnd1 = exp->get_exp()->opnd1_;
        if (opnd1 != nullptr) {
            opnd1->accept(this, scCtx);
        }
        exp->get_exp()->opnd2_->accept(this, scCtx);
    } else if (exp->is_lval()) {
        auto sym = exp->get_lval()->repr();
        if (!scCtx->has_sym(sym)) {
            throw ASTCheckFailed("[Scope Check] Undefined symbol '%s'.", sym.c_str());
        }
        if (!scCtx->sym_is_const(sym)) {
            exp->set_lval(scCtx->get_sym_var(sym));
        }
    }
}