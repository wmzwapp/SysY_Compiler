#include <cassert>
#include <cstdio>
#include <iostream>

#include "ast.hh"
#include "ir.hh"
#include "../ext/include/koopa.h"

extern FILE *yyin;
extern int yyparse(BaseAST *&ast);

// void initRawProgram(koopa_raw_program_t &p) {
//     p.funcs.len = 0;
//     p.funcs.kind = KOOPA_RSIK_FUNCTION;
//     p.funcs.buffer = nullptr;
//     p.values.len = 0;
//     p.values.kind = KOOPA_RSIK_VALUE;
//     p.values.buffer = nullptr;
// }

int main (int argc, const char *argv[]) {
    assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    auto output = argv[4];

    yyin = fopen(input, "r");
    assert(yyin);

    BaseAST* ast { nullptr };
    auto ret = yyparse(ast);
    assert(!ret);

    auto* cu = dynamic_cast<CompUnitAST*>(ast);
    assert(cu);

    koopa_raw_program_t rawProgram {};
    koopa_program_t program { nullptr };
    // initRawProgram(rawProgram);

    cu->GenRawProgram(rawProgram);
    auto errorcode = koopa_generate_raw_to_koopa(&rawProgram, &program);

    // auto* topIR = cu->GenIR();
    // auto errorcode = koopa_parse_from_string(topIR->toStr().c_str(), &program);
    // auto rawBuilder = koopa_new_raw_program_builder();
    // auto rawProgram1 = koopa_build_raw_program(rawBuilder, program);
    koopa_dump_llvm_to_stdout(program);
    // koopa_delete_program(program);
    return 0;
}