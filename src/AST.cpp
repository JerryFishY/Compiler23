#include "AST.h"
#include <iostream>


// lv1 and lv2

// CompUnit
void CompUnitAST::Dump() const {
  std::cout << "CompUnit {";
  func_def->Dump();
  std::cout << "}";
}
void CompUnitAST::Koopa() const{
    func_def->Koopa();
}

// FuncDef
void FuncDefAST::Dump() const {
    std::cout << "FuncDef {";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << "}";
}
void FuncDefAST::Koopa() const {
    std::cout << "fun ";
    std::cout << "@"<<ident<<"(): ";
    func_type->Koopa();
    block->Koopa();
}

// FuncType
void FuncTypeAST::Dump() const {
    std::cout << "FuncType { int }";
  }
void FuncTypeAST::Koopa() const{
    std::cout << "i32 ";
}

// StmtAST
void StmtAST::Dump() const {
    std::cout << "Stmt { return ";
    std::cout << number;
    std::cout << "; }";
}
void StmtAST::Koopa() const{
    std::cout << "ret ";
    std::cout << number;
    std::cout << "\n";
}

//BlockAST
void BlockAST::Dump() const {
    std::cout << "Block { ";
    stmt->Dump();
    std::cout << " }";
}
void BlockAST::Koopa() const{
    std::cout << "{\n";
    std::cout << "%entry:\n";
    stmt->Koopa();
    std::cout << "}\n";
}