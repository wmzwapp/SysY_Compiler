#include "BaseIR.hh"
#include "ProgramIR.hh"
#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "StmtIR.hh"
#include "ValueIR.hh"


void IRVisitor::visit(ProgramIR* unit, IVCtx* ctx) {
    for (auto* func : unit->get_funcs()) {
        func->accept(this, ctx);
    }
}

void IRVisitor::visit(FunctionIR* func, IVCtx* ctx) {
    for (auto* bb : func->get_BBs()) {
        bb->accept(this, ctx);
    }
}

void IRVisitor::visit(BlockIR* block, IVCtx* ctx) {
    for (auto* stmt : block->get_stmts()) {
        stmt->accept(this, ctx);
    }
    block->get_end_stmt()->accept(this, ctx);
}

void IRVisitor::visit(StmtRetIR* stmt, IVCtx* ctx) {
    stmt->get_value()->accept(this, ctx);
}

void IRVisitor::visit(StmtBinaryExprIR* stmt, IVCtx* ctx) {
    stmt->get_opnd1()->accept(this, ctx);
    stmt->get_opnd2()->accept(this, ctx);
    stmt->get_def_var()->accept(this, ctx);
}

void IRVisitor::visit(AllocIR* stmt, IVCtx* ctx) {
    stmt->get_def_var()->accept(this, ctx);
}

void IRVisitor::visit(LoadIR* stmt, IVCtx* ctx) {
    stmt->get_src()->accept(this, ctx);
    stmt->get_des()->accept(this, ctx);
}

void IRVisitor::visit(StoreIR* stmt, IVCtx* ctx) {
    stmt->get_src()->accept(this, ctx);
    stmt->get_des()->accept(this, ctx);
}

void IRVisitor::visit(SymbolIR* sym, IVCtx* ctx) {

}

void IRVisitor::visit(ValueIntIR* num, IVCtx* ctx) {

}

