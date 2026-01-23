#include "InstrIR.hh"
#include "ValueIR.hh"
#include "BlockIR.hh"

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

void InstrRet::dump(std::ostream& os) const {
	os << "\tret ";
	if (value_ != nullptr) {
		value_->dump(os);
	}
	os << std::endl;
}

void InstrBExpr::dump(std::ostream& os) const {
	os << "\t";
	def_->dump(os);
	os << " = ";
	dump_op(os);
	os << " ";
	opnd1_->dump(os);
	os << ", ";
	opnd2_->dump(os);
	os << std::endl;
}

void InstrAlloc::dump(std::ostream& os) const {
	os << '\t';
	def_->dump(os);
	os << " = alloc ";
	allocTy_->dump(os);
	os << std::endl;
}

void InstrStore::dump(std::ostream& os) const {
	os << "\tstore ";
	src_->dump(os);
	os << ", ";
	des_->dump(os);
	os << std::endl;
}

void InstrLoad::dump(std::ostream& os) const {
	os << '\t';
	def_->dump(os);
	os << " = load ";
	src_->dump(os);
	os << std::endl;
}

void InstrBr::dump(std::ostream& os) const {
	os << "\tbr ";
	value_->dump(os);
	os << ", ";
	trueB_->get_symbol()->dump(os);
	os << ", ";
	falseB_->get_symbol()->dump(os);
	os << std::endl;
}

void InstrJump::dump(std::ostream& os) const {
	os << "\tjump ";
	branch_->get_symbol()->dump(os);
	os << std::endl;
}
