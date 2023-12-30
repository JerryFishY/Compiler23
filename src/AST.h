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

  virtual void Dump() const = 0;

  virtual void Koopa() const = 0;
};

// CompUnit
class CompUnitAST : public BaseAST {
 public:

  std::unique_ptr<BaseAST> func_def;

  void Dump() const override {
    std::cout << "CompUnit {";
    func_def->Dump();
    std::cout << "}";
  }
  void Koopa() const override{
    func_def->Koopa();
  }
};

// FuncDef
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDef {";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << "}";
  }

  void Koopa() const override{
    std::cout << "fun ";
    std::cout << "@"<<ident<<"(): ";
    func_type->Koopa();
    block->Koopa();
  }
};

class FuncTypeAST : public BaseAST {
 public:
  void Dump() const override {
    std::cout << "FuncType { int }";
  }
  void Koopa() const override{
    std::cout << "i32 ";
  }
};

// StmtAST definition
class StmtAST : public BaseAST {
 public:
  int number;

  void Dump() const override {
    std::cout << "StmtAST { return ";
    std::cout << number;
    std::cout << "; }";
  }

  void Koopa() const override{
    std::cout << "ret ";
    std::cout << number;
    std::cout << "\n";
  }
};

// BlockAST definition
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;

  void Dump() const override {
    std::cout << "Block { ";
    stmt->Dump();
    std::cout << " }";
  }

  void Koopa() const override{
    std::cout << "{\n";
    std::cout << "%entry:\n";
    stmt->Koopa();
    std::cout << "}\n";
  }
};





