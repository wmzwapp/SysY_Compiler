#include "ir.hh"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <format>

void ProgramIR::dump() {
    for (auto* v : gValues_) {
        v->dump();
    }

    for (auto* func : funcs_) {
        func->dump();
    }
}

std::string ProgramIR::toStr() {
    std::string ret;
    for (auto* v : gValues_) {
        ret += v->toStr();
    }

    for (auto* func : funcs_) {
        ret += func->toStr();
    }

    return ret;
}

void FunctionIR::dump() {
    std::cout << "fun @" << name_ << "(";
    for (auto* v : args_) {
        v->dump();
    }
    std::cout << "): ";
    auto* numRet = dynamic_cast<ValueNumIR*>(ret_);
    assert(numRet != nullptr);
    std::cout << numRet->getValueTyStr() << "{\n";
    for (auto* bb : bbs_) {
        bb->dump();
    }
    std::cout << "} //" << name_ << '\n';
}

std::string FunctionIR::toStr() {
    std::string ret;
    ret = std::format("fun @{} (", name_);
    for (auto* v : args_) {
        ret += v->toStr();
    }
    ret += "): ";
    auto* numRet = dynamic_cast<ValueNumIR*>(ret_);
    assert(numRet != nullptr);
    ret += numRet->getValueTyStr();
    ret += " {\n";
    for (auto* bb : bbs_) {
        ret += bb->toStr();
    }
    ret += "}";
    // ret += std::format("}} // {}\n", name_);
    return ret;
}

void BasicBlockIR::dump() {
    std::cout << "%" << name_ << ":\n";
    for (auto* instr : instrs_) {
        instr->dump();
    }
}

std::string BasicBlockIR::toStr() {
    std::string ret;
    ret = std::format("%{}:\n", name_);
    for (auto* instr : instrs_) {
        ret += instr->toStr();
    }
    return ret;
}

void InstrRetIR::dump() {
    auto* numRet = dynamic_cast<ValueNumIR*>(ret_);
    assert(numRet != nullptr);
    std::cout << "\tret " << numRet->getValue() << std::endl;
}

std::string InstrRetIR::toStr() {
    std::string ret;
    auto* numRet = dynamic_cast<ValueNumIR*>(ret_);
    assert(numRet != nullptr);
    ret = std::format("\tret {}\n", numRet->toStr());
    return ret;
}
 