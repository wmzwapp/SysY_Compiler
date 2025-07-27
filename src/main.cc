#include <cassert>
#include <cstring>
#include <iostream>

#include "ast.hh"
// #include "ir.hh"
#include "../ext/include/koopa.h"
#include "rawProgram2RISCV.hh"

extern FILE *yyin;
extern int yyparse(BaseAST *&ast);

int main (int argc, const char *argv[]) {
    // assert(argc == 5);
    auto mode = argv[1];
    auto input = argv[2];
    // auto output = argv[4];

    yyin = fopen(input, "r");
    assert(yyin);

    BaseAST* ast { nullptr };
    auto ret = yyparse(ast);
    assert(!ret);

    auto* cu = dynamic_cast<CompUnitAST*>(ast);
    assert(cu);

    koopa_raw_program_t rawProgram {};
    koopa_program_t program { nullptr };
    cu->GenRawProgram(rawProgram);

    if (strcmp(mode, "-koopa") == 0) {
        auto errorcode = koopa_generate_raw_to_koopa(&rawProgram, &program);
        assert(errorcode == KOOPA_EC_SUCCESS);
        koopa_dump_llvm_to_stdout(program);   
    } else if (strcmp(mode, "-riscv") == 0) {
        rawProgram2RISCV p2r;
        std::cout << p2r.generate_RISCV_str(rawProgram);
    }

    return 0;
}