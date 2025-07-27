#include "ast.hh"
#include "ir.hh"
#include "koopa.h"

#include <cstring>
#include <iostream>
#include <cassert>
#include <string>

void CompUnitAST::Dump() const {
    std::cout << "CompUnit { ";
    funcDef_->Dump();
    std::cout << " }";
}

IRBase* CompUnitAST::GenIR() {
    auto* funcIR = dynamic_cast<FunctionIR*>(funcDef_->GenIR());
    assert(funcIR != nullptr);
    __IR_TOP__.appendFunctionIR(funcIR);
    return &__IR_TOP__;
}

void CompUnitAST::GenRawProgram(koopa_raw_program_t &p) {
    p.funcs.len = 1;
    p.funcs.kind = KOOPA_RSIK_FUNCTION;
    auto* rawFuncPtrs = new koopa_raw_function_t[p.funcs.len];
    p.funcs.buffer = (const void**)rawFuncPtrs;
    funcDef_->GenRawFunc(rawFuncPtrs[0]);

    p.values.len = 0;
    p.values.kind = KOOPA_RSIK_VALUE;
    p.values.buffer = nullptr;
}

void FuncDefAST::Dump() const {
    std::cout << "FuncDef { ";
    funcType_->Dump();
    std::cout << ", " << ident_ << ", ";
    block_->Dump();
    std::cout << " }";
}

IRBase* FuncDefAST::GenIR() {
    auto* funcIr = new FunctionIR(ident_);
    auto* numIr = dynamic_cast<ValueNumIR*>(funcType_->GenIR());
    assert(numIr != nullptr);
    funcIr->setRet(numIr);
    auto* bbIr = dynamic_cast<BasicBlockIR*>(block_->GenIR());
    assert(bbIr != nullptr);
    funcIr->appendBB(bbIr);         // O_o?

    return funcIr;
}

void FuncDefAST::GenRawFunc(koopa_raw_function_t &fptr) {
    auto* f = new koopa_raw_function_data_t {};
    fptr = f;

    auto* funcTy = new koopa_raw_type_kind {};
    funcTy->tag = KOOPA_RTT_FUNCTION;
    // funcTy->data.function.params.len = 0;
    funcTy->data.function.params.kind = KOOPA_RSIK_TYPE;
    // funcTy->data.function.params.buffer = nullptr;

    auto* retKind = new koopa_raw_type_kind {};
    funcType_->GenRawFuncRetType(*retKind);
    funcTy->data.function.ret = retKind;

    f->ty = funcTy;

    auto name = std::string("@") + ident_;
    auto* funcName = new char[name.length()];
    memcpy(funcName, name.data(), name.length());
    f->name = funcName;

    f->params.len = 0;
    f->params.kind = KOOPA_RSIK_VALUE;
    f->params.buffer = nullptr;

    f->bbs.len = 1;
    f->bbs.kind = KOOPA_RSIK_BASIC_BLOCK;
    auto* bbPtrs = new koopa_raw_basic_block_t[f->bbs.len];
    block_->GenRawBasicBlock(bbPtrs[0]);
    f->bbs.buffer = (const void **)bbPtrs;
}

void FuncTypeAST::Dump() const {
    std::cout << "FuncType { ";
    std::cout << funcType_;
    std::cout << " }";
}

IRBase* FuncTypeAST::GenIR() {
    if (funcType_ == "int") {
        return new ValueNumIR();
    }

    return nullptr;
}

void FuncTypeAST::GenRawFuncRetType(koopa_raw_type_kind &t) {
    t.tag = KOOPA_RTT_INT32;
    // t->data
}

void BlockAST::Dump() const {
    std::cout << "Block { ";
    stmt_->Dump();
    std::cout << " }";
}

IRBase* BlockAST::GenIR() {
    auto* bb = new BasicBlockIR("entry");
    auto* instr = dynamic_cast<InstrBaseIR*>(stmt_->GenIR());
    assert(instr != nullptr);
    bb->appendInstr(instr);

    return bb;
}

void BlockAST::GenRawBasicBlock(koopa_raw_basic_block_t &bbPtr) {
    auto* bb = new koopa_raw_basic_block_data_t {};
    bbPtr = bb;

    bb->name = new char[] {"%entry"};

    // bb->params.len = 0;
    bb->params.kind = KOOPA_RSIK_VALUE;
    // bb->params.buffer = nullptr;
    // bb->used_by.len = 0;
    bb->used_by.kind = KOOPA_RSIK_VALUE;
    // bb->used_by.buffer = nullptr;

    bb->insts.len = 1;
    bb->insts.kind = KOOPA_RSIK_VALUE;
    auto* instrPtrs = new koopa_raw_value_t[bb->insts.len];
    stmt_->GenRawInstr(instrPtrs[0]);
    bb->insts.buffer = (const void**)instrPtrs;
}

void StmtAST::Dump() const {
    std::cout << "return " << num_;
}

IRBase* StmtAST::GenIR() {
    auto* ret = new ValueNumIR();
    ret->setValue(num_);
    auto* retInstr = new InstrRetIR(ret);
    return retInstr;
}

void StmtAST::GenRawInstr(koopa_raw_value_t &vptr) {
    auto* v = new koopa_raw_value_data_t {};
    vptr = v;

    koopa_raw_type_kind_t* ty = new koopa_raw_type_kind_t {};
    ty->tag = KOOPA_RTT_UNIT;
    // ty->data

    v->ty = ty;
    v->name = nullptr;
    v->used_by.kind = KOOPA_RSIK_VALUE;
    v->kind.tag = KOOPA_RVT_RETURN;

    // return value
    koopa_raw_value_data* retVal = new koopa_raw_value_data {};
    auto* retValTy = new koopa_raw_type_kind_t {};
    retValTy->tag = KOOPA_RTT_INT32;
    // retValTy->data

    retVal->ty = retValTy;
    retVal->name = nullptr;

    retVal->used_by.len = 1;
    retVal->used_by.kind = KOOPA_RSIK_VALUE;
    auto* retValUsedBys = new koopa_raw_value_t[retVal->used_by.len];
    retValUsedBys[0] = v;
    retVal->used_by.buffer = (const void **)retValUsedBys;

    retVal->kind.tag = KOOPA_RVT_INTEGER;
    retVal->kind.data.integer.value = num_;

    v->kind.data.ret.value = retVal;
}