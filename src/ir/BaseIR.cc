#include "BaseIR.hh"
#include "ProgramIR.hh"
#include "FunctionIR.hh"
#include "BlockIR.hh"
#include "InstrIR.hh"
#include "ValueIR.hh"


using namespace IR;

void IRVisitor::visit(Program* unit, IVCtx* ctx) {
    for (auto* func : unit->get_funcs()) {
        func->accept(this, ctx);
    }
}

void IRVisitor::visit(Function* func, IVCtx* ctx) {
    for (auto* bb : func->get_BBs()) {
        bb->accept(this, ctx);
    }
}

void IRVisitor::visit(Block* block, IVCtx* ctx) {
    for (auto* stmt : block->get_instrs()) {
        stmt->accept(this, ctx);
    }
    // block->get_end_stmt()->accept(this, ctx);
}

void IRVisitor::visit(InstrRet* stmt, IVCtx* ctx) {
    stmt->get_value()->accept(this, ctx);
}

void IRVisitor::visit(InstrBExpr* stmt, IVCtx* ctx) {
    stmt->get_opnd1()->accept(this, ctx);
    stmt->get_opnd2()->accept(this, ctx);
    stmt->get_def()->accept(this, ctx);
}

void IRVisitor::visit(InstrAlloc* stmt, IVCtx* ctx) {
    stmt->get_def()->accept(this, ctx);
}

void IRVisitor::visit(InstrLoad* stmt, IVCtx* ctx) {
    stmt->get_src()->accept(this, ctx);
    stmt->get_def()->accept(this, ctx);
}

void IRVisitor::visit(InstrStore* stmt, IVCtx* ctx) {
    stmt->get_src()->accept(this, ctx);
    stmt->get_des()->accept(this, ctx);
}

void IRVisitor::visit(InstrBr* instr, IVCtx* ctx) {
    instr->get_value()->accept(this, ctx);
    instr->get_true_branch()->accept(this, ctx);
    instr->get_false_branch()->accept(this, ctx);
}

void IRVisitor::visit(InstrJump* instr, IVCtx* ctx) {
    instr->get_branch()->accept(this, ctx);
}

void IRVisitor::visit(Symbol* sym, IVCtx* ctx) {

}

void IRVisitor::visit(ValueInt* num, IVCtx* ctx) {

}

void IRVisitor::visit(ValueUndef* undef, IVCtx* ctx) {
    
}

