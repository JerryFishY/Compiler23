#include <iostream>
#include <memory>
#include <string>

#pragma once

//Declarations for the AST

//lv1
class CompUnitAST;
class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class NumberAST;

// Base class for all AST
class BaseAST {
 public:
  virtual ~BaseAST() = default;     
  virtual void Dump() const = 0;    // Dump the derived AST
  virtual void Koopa() const = 0;   // Give the derived Koopa code
};

// CompUnit
class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;
  void Dump() const override;
  void Koopa() const override;
};

// FuncDef
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump() const override;
  void Koopa() const override;
};

//FuncType
class FuncTypeAST : public BaseAST {
 public:
  void Dump() const override;
  void Koopa() const override;
};

// StmtAST
class StmtAST : public BaseAST {
 public:
  int number;
  void Dump() const override;
  void Koopa() const override;
};

// BlockAST definition
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  void Dump() const override;
  void Koopa() const override;
};





