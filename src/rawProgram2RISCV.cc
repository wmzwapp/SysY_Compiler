#include "rawProgram2RISCV.hh"
#include "koopa.h"
#include <cstddef>

using P2R = rawProgram2RISCV;

std::string P2R::generate_RISCV_str(const koopa_raw_program_t &program) {
    output_ << "\t.text\n";
    Visit(program);
    return output_.str();
}

void P2R::Visit(const koopa_raw_program_t &program) {
    Visit(program.values);
    Visit(program.funcs);
}

void P2R::Visit(const koopa_raw_slice_t &slice) {
    for (size_t i = 0; i < slice.len; ++i) {
        auto* ptr = slice.buffer[i];
        switch (slice.kind) {
            case KOOPA_RSIK_FUNCTION:
                Visit(reinterpret_cast<koopa_raw_function_t>(ptr));
                break;
            case KOOPA_RSIK_BASIC_BLOCK:
                Visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
                break;
            case KOOPA_RSIK_VALUE:
                Visit(reinterpret_cast<koopa_raw_value_t>(ptr));
                break;
            default:
                break;
        }
    }
}

void P2R::Visit(const koopa_raw_function_t &func) {
    output_ << "\t.global " << func->name << '\n';
    output_ << func->name << ":\n";                 // O_o?
    Visit(func->bbs);
}

void P2R::Visit(const koopa_raw_basic_block_t &bb) {
    Visit(bb->insts);
}

void P2R::Visit(const koopa_raw_value_t &value) {
    const auto &kind = value->kind;
    switch (kind.tag) {
        case KOOPA_RVT_RETURN:
            Visit(kind.data.ret);
            break;
        case KOOPA_RVT_INTEGER:
            Visit(kind.data.integer);
            break;
        default:
            break;
    }
}

void P2R::Visit(const koopa_raw_return_t &v) {
    output_ << "\tli a0, ";
    Visit(v.value);
    output_ << '\n';
    output_ << "\tret\n";
}

void P2R::Visit(const koopa_raw_integer_t &v) {
    output_ << v.value;
}