#include "StmtAST.hh"
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

void StmtAST::GenIR(FunctionIR* func, BlockIR* bbIR) {
    auto* value = retExp_->GenIR(func, bbIR);
    auto* retIR = new StmtRetIR(value);
    bbIR->appendStmt(retIR);
    bbIR->setEndStmt(retIR);
}

// class StmtAST end

// class ExpAST

ValueIR* ExpAST::GenIR(FunctionIR* func, BlockIR* bb) {
    return addExp_->gen_ir(func, bb);
}

void ExpAST::Dump() const {
    std::cout << "<expr> {";
    addExp_->Dump();
    std::cout << " }";
}

// class ExpAST end

// class PrimaryExpAST

ValueIR* PrimaryExpAST::GenIR(FunctionIR* func, BlockIR* bb) {
    if (exp_ != nullptr) {
        return exp_->GenIR(func, bb);
    } else if (num_ != nullptr) {
        return num_->GenIR();
    }
    return nullptr;
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

ValueIR* UnaryExpAST::GenIR(FunctionIR* func, BlockIR* bb) {
    if (unaryExp_ != nullptr) {
        return GenUnaryExp(func, bb);
    } else if (pExp_ != nullptr) {
        return GenPrimaryExp(func, bb);
    }
    return nullptr;
}

ValueIR* UnaryExpAST::GenUnaryExp(FunctionIR* func, BlockIR* bb) {
    auto* value = unaryExp_->GenIR(func, bb);
    if (op_ == OpAST::LNOT) {
        auto* ret = func->getATmpSymbol();
        auto* value0 = new ValueIntIR(getIntType(), 0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::EQ, ret, value, value0);
        bb->appendStmt(stmt);
        return ret;
    } else if (op_ == OpAST::MINUS) {
        auto* ret = func->getATmpSymbol();
        auto* value0 = new ValueIntIR(getIntType(), 0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::SUB, ret, value0, value);
        bb->appendStmt(stmt);
        return ret;
    }
    return value;
}

ValueIR* UnaryExpAST::GenPrimaryExp(FunctionIR* func, BlockIR* bb) {
    return pExp_->GenIR(func, bb);
}

void UnaryExpAST::Dump() const {
    if (pExp_ != nullptr) {
        DumpPrimaryExp();
    } else if (unaryExp_ != nullptr) {
        DumpUnaryExp();
    }
}

void UnaryExpAST::DumpPrimaryExp() const {
    pExp_->Dump();
}

void UnaryExpAST::DumpUnaryExp() const {
    dump_op_ast(op_);
    unaryExp_->Dump();
}

// class UnaryExpAST end


// class AddExpAST begin

void AddExpAST::Dump() const {
    if (addExp_ != nullptr) {
        addExp_->Dump();
        dump_op_ast(op_);
        mulExp_->Dump();
    } else if (mulExp_ != nullptr) {
        mulExp_->Dump();
    }
}

ValueIR* AddExpAST::gen_ir(FunctionIR* func, BlockIR* block) {
    if (addExp_ != nullptr) {
        auto* op1 = addExp_->gen_ir(func, block);
        auto* op2 = mulExp_->gen_ir(func, block);

        auto OpIR { BinaryOp::BAD };
        if (op_ == OpAST::PLUS) OpIR = BinaryOp::ADD;
        else if (op_ == OpAST::MINUS) OpIR = BinaryOp::SUB;

        auto* ret = func->getATmpSymbol();
        auto* stmt = new StmtBinaryExprIR(OpIR, ret, op1, op2);
        block->appendStmt(stmt);

        return ret;
    } else if (mulExp_ != nullptr) {
        return mulExp_->gen_ir(func, block);
    }
    return nullptr;
}

// class AddExpAST end


// class MulExpAST begin

void MulExpAST::Dump() const {
    if (mulExp_ != nullptr) {
        mulExp_->Dump();
        dump_op_ast(op_);
        unaryExp_->Dump();
    } else if (unaryExp_ != nullptr) {
        unaryExp_->Dump();
    }
}

ValueIR* MulExpAST::gen_ir(FunctionIR* func, BlockIR* block) {
    if (mulExp_ != nullptr) {
        auto* op1 = mulExp_->gen_ir(func, block);
        auto* op2 = unaryExp_->GenIR(func, block);

        auto OpIR { BinaryOp::BAD };
        if (op_ == OpAST::MUL) OpIR = BinaryOp::MUL;
        else if (op_ == OpAST::DIV) OpIR = BinaryOp::DIV;
        else if (op_ == OpAST::MOD) OpIR = BinaryOp::MOD;

        auto* ret = func->getATmpSymbol();
        auto* stmt = new StmtBinaryExprIR(OpIR, ret, op1, op2);
        block->appendStmt(stmt);

        return ret;
    } else if (unaryExp_ != nullptr) {
        return unaryExp_->GenIR(func, block);
    }
    return nullptr;
}

// class MulExpAST end