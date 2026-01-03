#include "BlockAST.hh"
#include "StmtAST.hh"
#include "BaseAST.hh"
#include "VarAST.hh"

#include <cassert>
#include <iostream>
#include <string>


/* class DeclAST */

void DeclAST::Dump(std::ostream& os) const {
    for (auto* def : defs_) {
        if (def->isConst_) {
            os << "const ";
        }
        def->type_.Dump(os);
        os << " ";
        def->var_->Dump(os);
        if (def->initExp_ != nullptr) {
            os << " = ";
            def->initExp_->Dump(os);
        }
        os << ";";
        os << '\t';
    }
}

/* class DeclAST end */


/* class StmtAST */

void StmtAST::Dump(std::ostream& os) const {
    if (isRetExp()) {
        os << "return ";
        if (getRetExp() != nullptr) {
            getRetExp()->Dump(os);
        }
    } else if (isAssignExp()) {
        const auto* assignExp = getAssignExp();
        assignExp->lval_->Dump(os);
        os << " = ";
        assignExp->exp_->Dump(os);
    } else if (isBlock()) {
        getBlock()->Dump(os);
    } else if (isExp()) {
        getExp()->Dump(os);
    } else if (isIFExp()) {
        auto* ifExp = getIFExp();
        os << "if (";
        ifExp->condExp_->Dump(os);
        os << ") ";
        ifExp->ifStmt_->Dump(os);
        if (ifExp->elseStmt_ != nullptr) {
            os << " else ";
            ifExp->elseStmt_->Dump(os);
        }
    }
    os << " ;";
}

/* class StmtAST end */


/* class ExpAST */

void ExpAST::Dump(std::ostream& os) const {
    if (is_exp()) {
        os << "(";
        auto* exp = get_exp();
        if (exp->opnd1_ != nullptr) {
            exp->opnd1_->Dump(os);
            os << " ";
        }
        OpAST::dump_op_ast(os, exp->op_);
        os << " ";
        exp->opnd2_->Dump(os);
        os << ")";
    } else if (is_lval()) {
        get_lval()->Dump(os);
    } else {
        get_num()->Dump(os);
    }
}

/* class ExpAST end */



/* struct OpAST */

void OpAST::set_op_ast(const char *op, OpAST& opAST) {
    switch (*op) {
        case '-':   opAST.op_ = MINUS; break;
        case '+':   opAST.op_ = PLUS; break;
        case '!': {
			if (strcmp(op, "!") == 0) {
				opAST.op_ = LNOT;
			} else if (strcmp(op, "!=") == 0) {
				opAST.op_ = NEQ;
			}
			break;
		}
        case '*':   opAST.op_ = MUL; break;
        case '/':   opAST.op_ = DIV; break;
        case '%':   opAST.op_ = MOD; break;
		case '<': {
			if(strcmp(op, "<") == 0) {
				opAST.op_ = LT;
			} else if (strcmp(op, "<=") == 0) {
				opAST.op_ = LE;
			}
			break;
		}
		case '>': {
			if (strcmp(op, ">") == 0) {
				opAST.op_ = GT;
			} else if (strcmp(op, ">=") == 0) {
				opAST.op_ = GE;
			}
            break;
		}
		case '=': {
			if (strcmp(op, "==") == 0) {
				opAST.op_ = EQ;
			}
			break;
		}
		case '&': {
			if (strcmp(op, "&&") == 0) {
				opAST.op_ = LAND;
			}
			break;
		}
		case '|': {
			if (strcmp(op, "||") == 0) {
				opAST.op_ = LOR;
			}
			break;
		}
        default:    break;
    }
}

void OpAST::dump_op_ast(std::ostream& os, OpAST opAST) {
    switch (opAST.op_) {
        case OpAST::MINUS:  os << "-"; break;
        case OpAST::PLUS:   os << "+"; break;
        case OpAST::LNOT:   os << "!"; break;
        case OpAST::MUL:    os << "*"; break;
        case OpAST::DIV:    os << "/"; break;
        case OpAST::MOD:    os << "%"; break;
		case OpAST::LT:		os << "<"; break;
		case OpAST::LE:		os << "<="; break;
		case OpAST::GT:		os << ">"; break;
		case OpAST::GE:		os << ">="; break;
		case OpAST::EQ:		os << "=="; break;
		case OpAST::NEQ:	os << "!="; break;
		case OpAST::LAND:	os << "&&"; break;
		case OpAST::LOR:	os << "||"; break;
		case OpAST::OP_BAD: os << "<BAD OP>"; break;
        default:    break;
    }
}

std::string OpAST::repr_op_ast(OpAST opAST) {
    switch (opAST.op_) {
        case OpAST::MINUS:	return "-";
        case OpAST::PLUS:	return "+";
        case OpAST::LNOT:	return "!";
        case OpAST::MUL:	return "*";
        case OpAST::DIV:	return "/";
        case OpAST::MOD:	return "%";
		case OpAST::LT:		return "<";
		case OpAST::LE:		return "<=";
		case OpAST::GT:		return ">";
		case OpAST::GE:		return ">=";
		case OpAST::EQ:		return "==";
		case OpAST::NEQ:	return "!=";
		case OpAST::LAND:	return "&&";
		case OpAST::LOR:	return "||";
		case OpAST::OP_BAD: return "<BAD OP>";
        default:    return "";
    }
}

/* Struct OpAST end */