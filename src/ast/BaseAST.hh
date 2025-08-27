#pragma once

class BaseAST {
  public:
	virtual ~BaseAST() = default;

  public:
	virtual void Dump() const = 0;
	// virtual IRBase* GenIR() = 0;
};