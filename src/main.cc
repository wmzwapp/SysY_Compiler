#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

#include "ast/BaseAST.hh"
#include "ast/CompUnitAST.hh"
#include "ast/ScopeCheck.hh"
#include "ast/ConstProp.hh"
#include "ast/GenIR.hh"

#include "ir/BaseIR.hh"
#include "ir/ProgramIR.hh"
#include "ir/GenASM.hh"

#include "asm/cfg.hh"

extern FILE *yyin;
extern int yyparse(BaseAST *&ast);
extern "C" int yydebug;

int main (int argc, const char *argv[]) {
    assert(argc == 4 || argc == 3);
    auto mode = argv[1];
    auto input = argv[2];
    bool debugMode { argc == 4 && (strcmp(argv[3], "-d") == 0) };
    bool dumpFile { argc == 4 && (strcmp(argv[3], "-f") == 0) };
    const char* outAST = "out.ast";
    const char* outKoopa = "out.koopa";
    const char* outASM = "out.S";

    bool genAST { false }, genIR { false }, genASM { false };
    if (strcmp(mode, "ast") == 0) {
        genAST = true;
    } else if (strcmp(mode, "koopa") == 0) {
        genAST = true;
        genIR = true;
    } else if (strcmp(mode, "asm") == 0) {
        genAST = true;
        genIR = true;
        genASM = true;
    }

    yyin = fopen(input, "r");
    assert(yyin);

    BaseAST* ast { nullptr };
    yydebug = 0;
    auto ret = yyparse(ast);
    if (ret) {
        return 1;
    }

    // AST
    if (genAST) {
        ScopeCheckerVisitor scv;
        ScopeCheckCtx scctx;
        scv.visit(&__AST_TOP__, &scctx);
        
        ConstPropVisitor cpv;
        ConstPropCtx cpctx;
        cpv.visit(&__AST_TOP__, &cpctx);

        if (dumpFile) {
            auto fp = std::ofstream(outAST);
            __AST_TOP__.Dump(fp);
        } else if (debugMode) {
            __AST_TOP__.Dump(std::cout);
        }
    }

    // IR
    if (genIR) {
        GenIRVisitor girv;
        GenIRCtx girctx;
        girctx.set_current_programIR(&__IR_TOP__);
        girv.visit(&__AST_TOP__, &girctx);

        if (dumpFile) {
            auto fp = std::ofstream(outKoopa);
            __IR_TOP__.dump(fp);
        } else if (debugMode) {
            __IR_TOP__.dump(std::cout);
        }
    }

    // ASM
    if (genASM) {
        GenASMVisitor gasmv;
        GACTX gasmctx;
        gasmctx.set_asm_top(&__ASMER__);
        gasmv.visit(&__IR_TOP__, &gasmctx);

        if (dumpFile) {
            auto fp = std::ofstream(outASM);
            __ASMER__.dump(fp);
        } else if (debugMode) {
            __ASMER__.dump(std::cout);
        }
    }

    return 0;
}