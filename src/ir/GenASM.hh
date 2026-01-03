#include "BaseIR.hh"

class TOPASM;
class FuncASM;
class BasicBlockASM;
class VarASM;

namespace IR {

class GenASMVisitorContest : public IVCtx {
    TOPASM* asmTop_ { nullptr };
    FuncASM* currentFunc_ { nullptr };
    BasicBlockASM* currentBB_ { nullptr };
    uint32_t stackOffset_ { 0 };

    VarASM* x0 { nullptr };
    VarASM* a0 { nullptr };
    VarASM* sp { nullptr };
    std::vector<VarASM*> tmpVars_;
    unsigned int tmpVarCount_ { 0 };
public:
    GenASMVisitorContest();

    void set_asm_top(TOPASM* top) { asmTop_ = top; }
    TOPASM* get_asm_top() { return asmTop_; }

    void set_current_func(FuncASM* f) { currentFunc_ = f; }
    FuncASM* get_current_func() { return currentFunc_; }

    void set_current_BB(BasicBlockASM* bb) { currentBB_= bb; }
    BasicBlockASM* get_current_BB() { return currentBB_; }

    void set_current_stask_offset(uint32_t ofs) { stackOffset_ = ofs; }
    uint32_t get_current_stack_offset() { return stackOffset_; }

    void gen_asm_func_prologue(Function* func);
    void gen_asm_func_epilogue(Function* func);

    VarASM* get_reg_var_x0() { return x0; }
	VarASM* get_reg_var_a0() { return a0; }
	VarASM* get_reg_var_sp() { return sp; }
    VarASM* get_tmp_var() { return tmpVars_[tmpVarCount_++]; }
    void return_all_tmp_var() { tmpVarCount_ = 0; }
    VarASM* get_a_reg_var(Value* b);

    void gen_asm_binary_stmt_eq(InstrBExpr* stmt);
	void gen_asm_binary_stmt_ne(InstrBExpr* stmt);
	void gen_asm_binary_stmt_sub(InstrBExpr* stmt);
	void gen_asm_binary_stmt_add(InstrBExpr* stmt);
	void gen_asm_binary_stmt_mul(InstrBExpr* stmt);
	void gen_asm_binary_stmt_div(InstrBExpr* stmt);
	void gen_asm_binary_stmt_mod(InstrBExpr* stmt);
	void gen_asm_binary_stmt_lt(InstrBExpr* stmt);
	void gen_asm_binary_stmt_gt(InstrBExpr* stmt);
	void gen_asm_binary_stmt_le(InstrBExpr* stmt);
	void gen_asm_binary_stmt_ge(InstrBExpr* stmt);
	void gen_asm_binary_stmt_and(InstrBExpr* stmt);
	void gen_asm_binary_stmt_or(InstrBExpr* stmt);
};

using GACTX = GenASMVisitorContest;

class GenASMVisitor : public IRVisitor {
public:
    using IRVisitor::visit;
	virtual void visit(Function* func, IVCtx* ctx) override;
	virtual void visit(InstrRet* stmt, IVCtx* ctx) override;
	virtual void visit(InstrBExpr* stmt, IVCtx* ctx) override;
	virtual void visit(InstrLoad* stmt, IVCtx* ctx) override;
	virtual void visit(InstrStore* stmt, IVCtx* ctx) override;
};

}