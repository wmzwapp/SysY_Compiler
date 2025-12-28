#pragma once

#include "BaseAST.hh"
#include <cassert>
#include <vector>


class BlockItemAST;
class DeclAST;

/* 
	Block ::= "{" {BlockItem} "}";
*/
class BlockAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(BlockAST);
  	BlockAST() { SET_TYPE_ID(BlockAST); }

public:
	void Dump(std::ostream& os) const override;
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }
	
public:
	void add_item(BlockItemAST* ast) { items_.push_back(ast); }
	std::vector<BlockItemAST*>& get_items() { return items_; }

	SymTabAST& get_sym_tab() { return symTable_; }

private:
	std::vector<BlockItemAST*> items_;
	SymTabAST	symTable_;
};


/*
	BlockItem		::= Decl | Stmt
*/
class BlockItemAST : public BaseAST {
public:
	STATIC_TYPE_ID_DECL(BlockItemAST);
	BlockItemAST(BaseAST* ast);

public:
	void Dump(std::ostream& os) const override;
	void accept(AstVisitor* v, VCtx* ctx) override { v->visit(this, ctx); }

public:
	bool is_decl() const { return item_.index() == 0; }
	DeclAST* get_decl_ast() { return std::get<0>(item_); }

	bool is_stmt() const { return item_.index() == 1; }
	StmtAST* get_stmt_ast() { return std::get<1>(item_); }

private:
	std::variant<DeclAST*, StmtAST*> item_;
};
