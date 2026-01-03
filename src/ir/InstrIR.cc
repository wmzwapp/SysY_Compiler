#include "InstrIR.hh"

using namespace IR;

void InstrBExpr::dump_op(std::ostream& os) const {
    switch (op_) {
		case BinaryOp::ADD: {
			os << "add";
			break;
		}
		case BinaryOp::AND: {
			os << "and";
			break;
		}
		case BinaryOp::DIV: {
			os << "div";
			break;
		}
		case BinaryOp::EQ: {
			os << "eq";
			break;
		}
		case BinaryOp::GE: {
			os << "ge";
			break;
		}
		case BinaryOp::GT: {
			os << "gt";
			break;
		}
		case BinaryOp::LE: {
			os << "le";
			break;
		}
		case BinaryOp::LT: {
			os << "lt";
			break;
		}
		case BinaryOp::MOD: {
			os << "mod";
			break;
		}
		case BinaryOp::MUL: {
			os << "mul";
			break;
		}
		case BinaryOp::NE: {
			os << "ne";
			break;
		}
		case BinaryOp::OR: {
			os << "or";
			break;
		}
		case BinaryOp::SAR: {
			os << "sar";
			break;
		}
		case BinaryOp::SHL: {
			os << "shl";
			break;
		}
		case BinaryOp::SHR: {
			os << "shr";
			break;
		}
		case BinaryOp::SUB: {
			os << "sub";
			break;
		}
		case BinaryOp::XOR: {
			os << "xor";
			break;
		}
		default: {
			os << "badop";
			break;
		}
 	}
}