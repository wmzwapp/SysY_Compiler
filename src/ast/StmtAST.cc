#include "StmtAST.hh"
#include "ast/BaseAST.hh"
#include "ir/BlockIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/StmtIR.hh"
#include "ir/ValueIR.hh"

#include <iostream>

// class StmtAST

void StmtAST::Dump() const {
    std::cout << "<stmt> { return ";
    retExp_->Dump();
    std::cout << "; }";
}

void StmtAST::gen_ir(GenIRCfg* cfg) {
    retExp_->gen_ir(cfg);
    auto* value = cfg->get_return_value();
    auto* retIR = new StmtRetIR(value);
    auto* bbIR = cfg->get_current_blockIR();
    bbIR->appendStmt(retIR);
    bbIR->setEndStmt(retIR);
}

// class StmtAST end

// class ExpAST

void ExpAST::gen_ir(GenIRCfg* cfg) {
    return BinaryExp_->gen_ir(cfg);
}

void ExpAST::Dump() const {
    std::cout << "<expr> {";
    BinaryExp_->Dump();
    std::cout << " }";
}

// class ExpAST end

// class PrimaryExpAST

void PrimaryExpAST::gen_ir(GenIRCfg* cfg) {
    if (exp_ != nullptr) {
        exp_->gen_ir(cfg);
    } else if (num_ != nullptr) {
        num_->gen_ir(cfg);
    }
}

void PrimaryExpAST::Dump() const {
    std::cout << "<primaryexp> {";
    if (exp_ != nullptr) {
        std::cout << "(";
        // DumpExp();
        exp_->Dump();
        std::cout << ")";
    } else if (num_ != nullptr) {
        // DumpNumer();
        std::cout << num_->getValue();
    }
    std::cout << " }";
}

// class PrimaryExpAST end

// class UnaryExpAST
void UnaryExpAST::gen_ir(GenIRCfg* cfg) {
    if (sub_is_unary()) {
        gen_unary_exp_ir(cfg);
    } else if (sub_is_primary()) {
        gen_primary_exp_ir(cfg);
    }
}

void UnaryExpAST::gen_unary_exp_ir(GenIRCfg* cfg) {
    auto* unaryExp = get_unary_exp();
    auto op = get_unary_op();

    unaryExp->gen_ir(cfg);
    auto* value = cfg->get_return_value();
    auto* func = cfg->get_current_functionIR();
    auto* bb = cfg->get_current_blockIR();

    if (op == OpAST::LNOT) {
        auto* ret = func->getATmpSymbol();
        auto* value0 = new ValueIntIR(getIntType(), 0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::EQ, ret, value, value0);
        bb->appendStmt(stmt);
        cfg->set_return_value(ret);
    } else if (op == OpAST::MINUS) {
        auto* ret = func->getATmpSymbol();
        auto* value0 = new ValueIntIR(getIntType(), 0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::SUB, ret, value0, value);
        bb->appendStmt(stmt);
        cfg->set_return_value(ret);
    }
}

void UnaryExpAST::gen_primary_exp_ir(GenIRCfg* cfg) {
    get_primary_exp()->gen_ir(cfg);
}

void UnaryExpAST::Dump() const {
    if (sub_is_primary()) {
        get_primary_exp()->Dump();
    } else if (sub_is_unary()) {
        get_unary_exp()->Dump();
    }
}
// class UnaryExpAST end


// class BinaryExpAST begin
void BinaryExpAST::Dump() const {
    if (is_binary_exp()) {
        get_binary_opnd1()->Dump();
        dump_op_ast(get_binary_op());
        get_binary_opnd2()->Dump();
    } else if (is_other_exp()) {
        get_other_exp()->Dump();
    }
}

void BinaryExpAST::gen_ir(GenIRCfg* cfg) {
    if (is_binary_exp()) {
        get_binary_opnd1()->gen_ir(cfg);
        auto* op1 = cfg->get_return_value();
        get_binary_opnd2()->gen_ir(cfg);
        auto* op2 = cfg->get_return_value();

        auto OpIR { BinaryOp::BAD };
        auto op = get_binary_op();
        if (op == OpAST::PLUS) OpIR = BinaryOp::ADD;
        else if (op == OpAST::MINUS) OpIR = BinaryOp::SUB;
        else if (op == OpAST::MUL) OpIR = BinaryOp::MUL;
        else if (op == OpAST::DIV) OpIR = BinaryOp::DIV;
        else if (op == OpAST::MOD) OpIR = BinaryOp::MOD;
        else if (op == OpAST::LT) OpIR = BinaryOp::LT;
        else if (op == OpAST::LE) OpIR = BinaryOp::LE;
        else if (op == OpAST::GT) OpIR = BinaryOp::GT;
        else if (op == OpAST::GE) OpIR = BinaryOp::GE;
        else if (op == OpAST::EQ) OpIR = BinaryOp::EQ;
        else if (op == OpAST::NEQ) OpIR = BinaryOp::NE;
        else if (op == OpAST::LAND) OpIR = BinaryOp::AND;
        else if (op == OpAST::LOR) OpIR = BinaryOp::OR;

        auto* func = cfg->get_current_functionIR();
        auto* block = cfg->get_current_blockIR();
        auto* ret = func->getATmpSymbol();
        auto* stmt = new StmtBinaryExprIR(OpIR, ret, op1, op2);
        block->appendStmt(stmt);
        cfg->set_return_value(ret);
    } else if (is_other_exp()) {
        get_other_exp()->gen_ir(cfg);
    }
}

// class BinaryExpAST end
