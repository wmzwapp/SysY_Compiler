#include "BaseAST.hh"
#include "CompUnitAST.hh"
#include "BlockAST.hh"
#include "FuncDefAST.hh"
#include "ast/StmtAST.hh"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>


/* class  AstVisitor begin */
void AstVisitor::visit(CompUnitAST* unit, VCtx* ctx) {
    unit->get_func_def()->accept(this, ctx);
}

void AstVisitor::visit(FuncDefAST* func, VCtx* ctx) {
    func->get_block()->accept(this, ctx);
}

void AstVisitor::visit(BlockAST* block, VCtx* ctx) {
    for (auto* item : block->get_items()) {
        item->accept(this, ctx);
    }
}

void AstVisitor::visit(BlockItemAST* item, VCtx* ctx) {
    if (item->is_decl()) item->get_decl_ast()->accept(this, ctx);
    else if (item->is_stmt()) item->get_stmt_ast()->accept(this, ctx);
}

void AstVisitor::visit(DeclAST* decl, VCtx* ctx) {
    for (auto* def : decl->get_defs()) {
        if (def->initExp_ != nullptr) def->initExp_->accept(this, ctx);
        def->var_->accept(this, ctx);
    }
}

void AstVisitor::visit(StmtAST* stmt, VCtx* ctx) {
    if (stmt->isBlock()) stmt->getBlock()->accept(this, ctx);
    else if (stmt->isExp()) stmt->getExp()->accept(this, ctx);
    else if (stmt->isAssignExp()) {
        stmt->getAssignExp()->exp_->accept(this, ctx);
        stmt->getAssignExp()->lval_->accept(this, ctx);
    } else if (stmt->isRetExp()) {
        stmt->getRetExp()->accept(this, ctx);
    }
}

void AstVisitor::visit(ExpAST* expr, VCtx* ctx) {
    if (expr->is_exp()) {
        if (expr->get_exp()->opnd1_ != nullptr) {
            expr->get_exp()->opnd1_->accept(this, ctx);
        }
        expr->get_exp()->opnd2_->accept(this, ctx);
    } else if (expr->is_lval()) {
        expr->get_lval()->accept(this, ctx);
    } else if (expr->is_num()) {
        expr->get_num()->accept(this, ctx);
    }
}

void AstVisitor::visit(VarAST* var, VCtx* ctx) { }

void AstVisitor::visit(NumberAST* num, VCtx* ctx) { }
/* class AstVisitor end */




/* class ASTCheckFailed begin */

ASTCheckFailed::ASTCheckFailed(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int len = std::vsnprintf(nullptr, 0, fmt, ap);
    va_end(ap);

    if (len < 0) {
        msg_ = "ASTCheckFailed construct error.";
        return;
    }

    std::vector<char> buf(len + 1);
    va_start(ap, fmt);
    std::vsnprintf(buf.data(), buf.size(), fmt, ap);
    va_end(ap);

    msg_.assign(buf.data(), len);
}

/* class ASTCheckFailed end */