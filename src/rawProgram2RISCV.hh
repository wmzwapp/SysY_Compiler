#pragma once

#include "koopa.h"
#include <sstream>

class rawProgram2RISCV {
  public:
	std::string generate_RISCV_str(const koopa_raw_program_t &program);

  public:
	void Visit(const koopa_raw_program_t &program);
	void Visit(const koopa_raw_slice_t &slice);
	void Visit(const koopa_raw_function_t &func);
	void Visit(const koopa_raw_basic_block_t &bb);
	void Visit(const koopa_raw_value_t &value);

	void Visit(const koopa_raw_return_t &v);
	void Visit(const koopa_raw_integer_t &v);

  private:
	std::stringstream	output_;
};