#pragma once

#include "BaseAST.hh"
#include "StmtAST.hh"
#include "common/utils.hh"
#include <cassert>
#include <utility>
#include <vector>


class BlockItemAST;

/* 
	Block ::= "{" {BlockItem} "}";
*/
class BlockAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(BlockAST);
  	BlockAST(std::vector<BlockItemAST*>* items) : items_(std::move(*items)) { SET_TYPE_ID(BlockAST); }
	~BlockAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

  private:
	std::vector<BlockItemAST*> items_;
};


/*
	BlockItem		::= Decl | Stmt
*/
class BlockItemAST : public BaseAST {
  public:
	STATIC_TYPE_ID_DECL(BlockItemAST);
	BlockItemAST(BaseAST* ast);
	~BlockItemAST() override;

  public:
	void Dump() const override;
	void gen_ir(GenIRCfg* cfg) override;

	bool is_decl() const { return item_.index() == 0; }
	DeclAST* get_decl_ast() { return std::get<0>(item_); }

	bool is_stmt() const { return item_.index() == 1; }
	StmtAST* get_stmt_ast() { return std::get<1>(item_); }

  private:
	std::variant<DeclAST*, StmtAST*> item_;
};
