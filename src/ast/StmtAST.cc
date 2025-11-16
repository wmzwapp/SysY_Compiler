#include "StmtAST.hh"
#include "BaseAST.hh"
#include "VarAST.hh"
#include "ir/BlockIR.hh"
#include "ir/FunctionIR.hh"
#include "ir/StmtIR.hh"
#include "ir/TypeIR.hh"
#include "ir/ValueIR.hh"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>


/* class DeclAST */
DeclAST::~DeclAST() {
    if (is_const_decl()) {
        delete get_const_decl();
    } else if (is_var_decl()) {
        delete get_var_decl();
    }
}

void DeclAST::Dump() const {
    if (is_const_decl()) {
        get_const_decl()->Dump();
    } else if (is_var_decl()) {
        get_var_decl()->Dump();
    }
}

void DeclAST::gen_ir(GenIRCfg* cfg) {
    if (is_const_decl()) {
        // do nothing
    } else if (is_var_decl()) {
        get_var_decl()->gen_ir(cfg);
    }
}

/* class DeclAST end */






/* class ConstDeclAST */

ConstDeclAST::~ConstDeclAST() {
    for (auto* v : defs_) {
        delete v;
    }
}

void ConstDeclAST::Dump() const {
    for (auto* v : defs_) {
        v->Dump();
    }
}

// void ConstDeclAST::gen_ir(GenIRCfg* cfg) {
//     for (auto* defStmt : defs_) {
//         defStmt->gen_ir(cfg);
//     }
// }

/* class ConstDeclAST end */





/* class ConstDefAST */
ConstDefAST::~ConstDefAST() {
    delete constInitVal_;
}

void ConstDefAST::Dump() const {
    std::cout << "const ";
    type_.repr();
    std::cout << " ";
    ident_.repr();
    std::cout << " = ";
    constInitVal_->Dump();
    std::cout << ";";
}

/* class ConstDefAST end */





/* class ConstInitValAST */

ConstInitValAST::~ConstInitValAST() {
    delete constExp_;
}

void ConstInitValAST::Dump() const {
    constExp_->Dump();
}

tyI32 ConstInitValAST::eval() const {
    return constExp_->try_eval();
}

/* class ConstInitValAST end */





/* class VarDeclAST */

void VarDeclAST::Dump() const {
    for (auto* def : defs_) {
        def->Dump();
    }
}

void VarDeclAST::gen_ir(GenIRCfg* cfg) {
    for (auto* def : defs_) {
        def->gen_ir(cfg);
    }
}

/* class VarDeclAST end */





/* class VarDefAST */

void VarDefAST::Dump() const {
    type_.repr();
    std::cout << " ";
    ident_.repr();
    if (initVal_ != nullptr) {
        std::cout << " = ";
        initVal_->Dump();
    }
    std::cout << ";";
}

void VarDefAST::gen_ir(GenIRCfg* cfg) {
    if (initVal_ != nullptr) {
        ValueIR* currentval { nullptr };
        initVal_->gen_ir(cfg);
        currentval = cfg->get_current_value();

        assert(currentval->get_ty()->isInt() && type_.get_ty_str() == "int");

        auto* symbol = SymbolIR::create_named_var(ident_.get_ident(), TypeAST::get_int_ptr_type_ir());
        auto* allocIr = new AllocIR(symbol, TypeAST::get_int_type_ir());

        gSymTable_.add_sym(ident_.get_ident(), symbol);

        cfg->get_current_blockIR()->appendStmt(allocIr);
        cfg->set_current_value(symbol);

        auto* storeIr = new StoreIR(currentval, symbol);
        cfg->get_current_blockIR()->appendStmt(storeIr);
    } else {
        auto* symbol = SymbolIR::create_named_var(ident_.get_ident(), TypeAST::get_int_ptr_type_ir());
        auto* allocIr = new AllocIR(symbol, TypeAST::get_int_type_ir());

        gSymTable_.add_sym(ident_.get_ident(), symbol);

        cfg->get_current_blockIR()->appendStmt(allocIr);
        cfg->set_current_value(symbol);
    }
}

/* class VarDefAST end */




/* class StmtAST */

void StmtAST::Dump() const {
    if (isRetExp()) {
        std::cout << "return ";
        getRetExp()->Dump();
    } else if (isAssignExp()) {
        const auto* assignExp = getAssignExp();
        assignExp->lval_->Dump();
        std::cout << " = ";
        assignExp->exp_->Dump();
    }
    std::cout << " ;";
}

void StmtAST::gen_ir(GenIRCfg* cfg) {
    if (isRetExp()) {
        getRetExp()->gen_ir(cfg);
        auto* value = cfg->get_current_value();
        auto* retIR = new StmtRetIR(value);
        auto* bbIR = cfg->get_current_blockIR();
        bbIR->appendStmt(retIR);
        bbIR->setEndStmt(retIR);
    } else if (isAssignExp()) {
        auto* assignExp = getAssignExp();
        assignExp->exp_->gen_ir(cfg);
        auto* retVal = cfg->get_current_value();
        assert(retVal->get_ty()->isInt());

        auto* lval = assignExp->lval_;
        auto* sym = SymbolIR::get_named_var(lval->repr());

        auto* loadIr = new StoreIR(retVal, sym);
        cfg->get_current_blockIR()->appendStmt(loadIr);
    }
}

/* class StmtAST end */




// class ExpAST

void ExpAST::gen_ir(GenIRCfg* cfg) {
    return binaryExp_->gen_ir(cfg);
}

void ExpAST::Dump() const {
    std::cout << "<expr> { ";
    binaryExp_->Dump();
    std::cout << " }";
}

tyI32 ExpAST::try_eval() const {
    return binaryExp_->try_eval();
}

std::string ExpAST::repr() const {
    return binaryExp_->repr();
}

// class ExpAST end





// class PrimaryExpAST

void PrimaryExpAST::gen_ir(GenIRCfg* cfg) {
    if (is_exp_sub()) {
        get_exp_sub()->gen_ir(cfg);
    } else if (is_num_sub()) {
        auto* ir = ValueIntIR::create_num_var(get_num_sub()->val_);
        cfg->set_current_value(ir);
    } else if (is_lval_sub()) {
        get_lval_sub()->gen_ir(cfg);
    }
}

void PrimaryExpAST::Dump() const {
    std::cout << "<primaryexp> { ";
    if (is_exp_sub()) {
        std::cout << "(";
        // DumpExp();
        get_exp_sub()->Dump();
        std::cout << ")";
    } else if (is_num_sub()) {
        // DumpNumer();
        get_num_sub()->repr();
    } else if (is_lval_sub()) {
        get_lval_sub()->Dump();
    }
    std::cout << " }";
}

std::string PrimaryExpAST::repr() const {
    if (is_exp_sub()) {
        return std::string("(") + get_exp_sub()->repr() + ")";
    } else if (is_num_sub()) {
        return std::to_string(get_num_sub()->val_);
    } else if (is_lval_sub()) {
        return get_lval_sub()->repr();
    }
    return "";
}

tyI32 PrimaryExpAST::try_eval() const {
    if (is_exp_sub()) {
        return get_exp_sub()->try_eval();
    } else if (is_num_sub()) {
        return get_num_sub()->val_;
    } else if (is_lval_sub()) {
        auto sym = get_lval_sub()->repr();
        if (gSymTable_.has_sym(sym)) {
            auto v = gSymTable_.get_sym(sym);
            if (v.is_const()) {
                return gSymTable_.get_const_val(sym);
            } else {
                std::ostringstream ss;
                ss << "Not a const expression: " << sym << ".";
                throw ASTExpEvalFailed(ss.str());
            }
        } else {
            std::ostringstream ss;
            ss << "Cannot evaluate LVal: " << sym << ".";
            throw ASTExpEvalFailed(ss.str());
        }
    }
    assert("Unexpected branch reached.");
    return 0;
}

// class PrimaryExpAST end



/* class Lval */

void LValAST::gen_ir(GenIRCfg* cfg) {
    auto v = gSymTable_.get_sym(ident_.get_ident());
    if (v.is_const()) {
        auto constVal = v.get_const();
        auto* numvar = ValueIntIR::create_num_var(constVal);
        cfg->set_current_value(numvar);
    } else if (v.is_var()) {
        auto* var = (SymbolIR*)v.get_var();
        auto* func = cfg->get_current_functionIR();
        auto* ty = var->get_ty();
        if (ty->isPointer()) {
            ty = ((TypePtrIR*)ty)->get_source();
        }
        auto* tmpVar = func->getATmpSymbol(ty->clone());
        auto* loadIr = new LoadIR(tmpVar, var, ty->clone());
        cfg->set_current_value(tmpVar);
        cfg->get_current_blockIR()->appendStmt(loadIr);
    } else {
        assert(false && "Unexpected branch reached.");
    }
}

/* class Lval end */




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
    auto* value = cfg->get_current_value();
    auto* func = cfg->get_current_functionIR();
    auto* bb = cfg->get_current_blockIR();

    if (op == OpAST::LNOT) {
        auto* ret = func->getATmpSymbol(TypeAST::get_int_type_ir());
        auto* value0 = ValueIntIR::create_num_var(0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::EQ, ret, value, value0);
        bb->appendStmt(stmt);
        cfg->set_current_value(ret);
    } else if (op == OpAST::MINUS) {
        auto* ret = func->getATmpSymbol(TypeAST::get_int_type_ir());
        auto* value0 = ValueIntIR::create_num_var(0);
        auto* stmt = new StmtBinaryExprIR(BinaryOp::SUB, ret, value0, value);
        bb->appendStmt(stmt);
        cfg->set_current_value(ret);
    }
}

void UnaryExpAST::gen_primary_exp_ir(GenIRCfg* cfg) {
    get_primary_exp()->gen_ir(cfg);
}

void UnaryExpAST::Dump() const {
    if (sub_is_primary()) {
        get_primary_exp()->Dump();
    } else if (sub_is_unary()) {
        dump_op_ast(get_unary_op());
        get_unary_exp()->Dump();
    }
}

tyI32 UnaryExpAST::try_eval() const {
    if (sub_is_primary()) {
        return get_primary_exp()->try_eval();
    } else if (sub_is_unary()) {
        auto op = get_unary_op();
        if (op == OpAST::MINUS) {
            return - get_unary_exp()->try_eval();
        } else if (op == OpAST::LNOT) {
            return ! get_unary_exp()->try_eval();
        }
    }
    return 0;
}
// class UnaryExpAST end


// class BinaryExpAST begin
void BinaryExpAST::Dump() const {
    if (is_binary_exp()) {
        get_binary_opnd1()->Dump();
        std::cout << " ";
        dump_op_ast(get_binary_op());
        std::cout << " ";
        get_binary_opnd2()->Dump();
    } else if (is_other_exp()) {
        get_other_exp()->Dump();
    }
}

std::string BinaryExpAST::repr() const {
    if (is_binary_exp()) {
        return get_binary_opnd1()->repr() + repr_op_ast(get_binary_op()) + get_binary_opnd2()->repr();
    } else if (is_other_exp()) {
        return get_other_exp()->repr();
    }
    return "";
}

void BinaryExpAST::gen_ir(GenIRCfg* cfg) {
    if (is_binary_exp()) {
        get_binary_opnd1()->gen_ir(cfg);
        auto* op1 = cfg->get_current_value();
        get_binary_opnd2()->gen_ir(cfg);
        auto* op2 = cfg->get_current_value();

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
        auto* ret = func->getATmpSymbol(TypeAST::get_int_type_ir());
        auto* stmt = new StmtBinaryExprIR(OpIR, ret, op1, op2);
        block->appendStmt(stmt);
        cfg->set_current_value(ret);
    } else if (is_other_exp()) {
        get_other_exp()->gen_ir(cfg);
    }
}

tyI32 BinaryExpAST::try_eval() const {
    if (is_other_exp()) {
        return get_other_exp()->try_eval();
    } else if (is_binary_exp()) {
        tyI32 op1, op2;
        try {
            op1 = get_binary_opnd1()->try_eval();
        } catch(const ASTExpEvalFailed& e) {
            std::ostringstream ss;
            ss << "Failed to eval expr: " << get_binary_opnd1()->repr() << ". ";
            ss << e.what() << std::endl;
            throw ASTExpEvalFailed(ss.str());
        }
        try {
            op2 = get_binary_opnd2()->try_eval();
        } catch(const ASTExpEvalFailed& e) {
            std::ostringstream ss;
            ss << "Failed to eval expr: " << get_binary_opnd2()->repr() << '\n';
            ss << e.what() << std::endl;
            throw ASTExpEvalFailed(ss.str());
        }

        switch (get_binary_op()) {
            case OpAST::PLUS: {
                return op1 + op2;
            }
            case OpAST::MINUS: {
                return op1 - op2;
            }
            case OpAST::MUL: {
                return op1 * op2;
            }
            case OpAST::DIV: {
                // TODO: detect division by 0
                return op1 / op2;
            }
            case OpAST::MOD: {
                return op1 % op2;
            }
            case OpAST::LT: {
                return op1 < op2;
            }
            case OpAST::LE: {
                return op1 <= op2;
            }
            case OpAST::GT: {
                return op1 > op2;
            }
            case OpAST::GE: {
                return op1 >= op2;
            }
            case OpAST::EQ: {
                return op1 == op2;
            }
            case OpAST::NEQ: {
                return op1 != op2;
            }
            case OpAST::LAND: {
                return op1 && op2;
            }
            case OpAST::LOR: {
                return op1 || op2;
            }
            default: {
                break;
            }
        }
    }
    return 0;
}

// class BinaryExpAST end


/* class ConstExp begin */

/* class ConstExp end */
