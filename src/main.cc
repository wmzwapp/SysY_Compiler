#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "ast/CompUnitAST.hh"
#include "../ext/include/koopa.h"
#include "ir/ProgramIR.hh"
#include "asm/cfg.hh"

extern FILE *yyin;
extern int yyparse(BaseAST *&ast);
extern "C" int yydebug;

int main (int argc, const char *argv[]) {
    assert(argc == 4 || argc == 3);
    auto mode = argv[1];
    auto input = argv[2];
    bool dumpFile { argc == 4 && (strcmp(argv[3], "-f") == 0) };
    const char* outKoopa = "out.koopa";
    const char* outASM = "out.S";

    yyin = fopen(input, "r");
    assert(yyin);

    BaseAST* ast { nullptr };
    yydebug = 0;
    auto ret = yyparse(ast);
    assert(!ret);

    // AST
    auto* cu = dynamic_cast<CompUnitAST*>(ast);
    // cu->Dump();
    assert(cu);

    // IR
    cu->GenIR(&__IR_TOP__);

    if (strcmp(mode, "koopa") == 0) {
        if (dumpFile) {
            auto fp = std::ofstream(outKoopa);
            __IR_TOP__.dump(fp);
        } else {
            __IR_TOP__.dump(std::cout);
        }
    } else if (strcmp(mode, "asm") == 0) {
        // ASM
        __IR_TOP__.gen_asm(&__ASMER__);
        if (dumpFile) {
            auto fp = std::ofstream(outASM);
            __ASMER__.dump(fp);
        } else {
            __ASMER__.dump(std::cout);
        }
    }

    return 0;
}