#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "symbol.h"

#pragma once

//Declarations for the AST

extern int index_sysy; // To avoid dependency for sysy compiler
// CompUnit  ::= FuncDef;

// FuncDef   ::= FuncType IDENT "(" ")" Block;
// FuncType  ::= "int";

// Block     ::= "{" Stmt "}";
// Stmt      ::= "return" Number ";";
// Number    ::= INT_CONST;

//lv1-lv2
class CompUnitAST;
class FuncDefAST;
class FuncTypeAST;
class BlockAST;
class StmtAST;
class NumberAST;

//lv3.1

// Stmt      ::= "return" Exp ";";
// Exp         ::= UnaryExp;
// PrimaryExp  ::= "(" Exp ")" | Number;
// Number      ::= INT_CONST;
// UnaryExp    ::= PrimaryExp | UnaryOp UnaryExp;
// UnaryOp     ::= "+" | "-" | "!";

class ExpAST;
class PrimaryExpAST;
class UnaryExpAST;

//lv3.2

// Exp         ::= AddExp;
// PrimaryExp  ::= ...;
// Number      ::= ...;
// UnaryExp    ::= ...;
// UnaryOp     ::= ...;
// MulExp      ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
// AddExp      ::= MulExp | AddExp ("+" | "-") MulExp;
class AddExpAST;
class MulExpAST;

//lv3.3

// Exp         ::= LOrExp;
// PrimaryExp  ::= ...;
// Number      ::= ...;
// UnaryExp    ::= ...;
// UnaryOp     ::= ...;
// MulExp      ::= ...;
// AddExp      ::= ...;
// RelExp      ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
// EqExp       ::= RelExp | EqExp ("==" | "!=") RelExp;
// LAndExp     ::= EqExp | LAndExp "&&" EqExp;
// LOrExp      ::= LAndExp | LOrExp "||" LAndExp;
class RelExpAST;
class EqExpAST;
class LAndExpAST;
class LOrExpAST;
//lv4.1
// Decl          ::= ConstDecl;
// ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
// BType         ::= "int";
// ConstDef      ::= IDENT "=" ConstInitVal;
// ConstInitVal  ::= ConstExp;

// Block         ::= "{" {BlockItem} "}";
// BlockItem     ::= Decl | Stmt;

// LVal          ::= IDENT;
// PrimaryExp    ::= "(" Exp ")" | LVal | Number;

// ConstExp      ::= Exp;
class DeclAST;
class ConstDeclAST;
class BTypeAST;
class ConstDefAST;
class ConstInitValAST;
class BlockItemAST;
class LValAST;
class ConstExpAST;

//lv4.2
// Decl          ::= ConstDecl | VarDecl;
// ConstDecl     ::= ...;
// BType         ::= ...;
// ConstDef      ::= ...;
// ConstInitVal  ::= ...;
// VarDecl       ::= BType VarDef {"," VarDef} ";";
// VarDef        ::= IDENT | IDENT "=" InitVal;
// InitVal       ::= Exp;

// ...

// Block         ::= ...;
// BlockItem     ::= ...;
// Stmt          ::= LVal "=" Exp ";"
//                 | "return" Exp ";";
class VarDeclAST;
class VarDefAST;
class InitValAST;
class LeValAST;

//lv5
// Stmt ::= LVal "=" Exp ";"
//        | [Exp] ";"
//        | Block
//        | "return" [Exp] ";";

// Base class for all AST
class BaseAST {
 public:
  virtual ~BaseAST() = default;     
  virtual void Dump() const = 0;    // Dump the derived AST
  virtual void Koopa() = 0;   // Give the derived Koopa code
  virtual int  calc()  const = 0;   // Calculate the const expression
};

// lv1-lv2
// CompUnit
class CompUnitAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_def;
  void Dump() const override;
  void Koopa() override;
  int calc() const override{return 0;};
};

// FuncDef
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump() const override;
  void Koopa() override;
  int calc() const override{return 0;};
};

// FuncType
class FuncTypeAST : public BaseAST {
 public:
  void Dump() const override;
  void Koopa() override;
  int calc() const override{return 0;};
};

// StmtAST
class StmtAST : public BaseAST {
 public:
  enum TAG {RETURN,ASSIGN,BLOCK,EXP,EMPTY,IF,IFELSE};
  TAG tag;
  std::unique_ptr<BaseAST> exp;
  std::unique_ptr<BaseAST> lval;
  std::unique_ptr<BaseAST> block;
  std::unique_ptr<BaseAST> ifexp;
  std::unique_ptr<BaseAST> ifstmt;
  std::unique_ptr<BaseAST> elsestmt;
  int expnum;
  void Dump() const override;
  void Koopa() override;
  int calc() const override{return 0;};
};

// BlockAST
class BlockAST : public BaseAST {
 public:
  std::vector<std::unique_ptr<BaseAST>> block_item;
  void Dump() const override;
  void Koopa() override;
  int calc() const override{return 0;};
};

// lv3.1
// ExpAST
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> lorexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return lorexp->calc();};
};

// PrimaryExpAST
class PrimaryExpAST : public BaseAST {
  public:
    enum TAG {EXP,NUM,LVal};
    TAG tag;
    std::unique_ptr<BaseAST> exp;
    int number;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

// UnaryExpAST
class UnaryExpAST : public BaseAST {
  public:
    enum TAG {PEXP,OPUEXP};
    TAG tag;
    std::unique_ptr<BaseAST> pexp;
    std::string op;
    std::unique_ptr<BaseAST> uexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

// lv3.2
// AddExpAST
class AddExpAST : public BaseAST {
  public:
    enum TAG {MEXP,AEXP};
    TAG tag;
    std::unique_ptr<BaseAST> mexp;
    std::string op;
    std::unique_ptr<BaseAST> aexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//MulExpAST
class MulExpAST : public BaseAST {
  public:
    enum TAG {UEXP,MUEXP};
    TAG tag;
    std::unique_ptr<BaseAST> uexp;
    std::string op;
    std::unique_ptr<BaseAST> mexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//lv3.3
//RelExp
class RelExpAST : public BaseAST {
  public:
    enum TAG {AEXP,RELEXP};
    TAG tag;
    std::unique_ptr<BaseAST> aexp;
    std::string op;
    std::unique_ptr<BaseAST> relexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//EqExp
class EqExpAST : public BaseAST {
  public:
    enum TAG {RELEXP,EQEXP};
    TAG tag;
    std::unique_ptr<BaseAST> relexp;
    std::string op;
    std::unique_ptr<BaseAST> eqexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//LandExp
class LAndExpAST : public BaseAST {
  public:
    enum TAG {EQEXP,LANDEXP};
    TAG tag;
    std::unique_ptr<BaseAST> eqexp;
    std::string op;
    std::unique_ptr<BaseAST> landexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//LOrExp
class LOrExpAST : public BaseAST {
  public:
    enum TAG {LANDEXP,LOREXP};
    TAG tag;
    std::unique_ptr<BaseAST> landexp;
    std::string op;
    std::unique_ptr<BaseAST> lorexp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

//lv4.1
class DeclAST : public BaseAST {
  public:
    enum TAG {CONST,VAR};
    TAG tag;
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};
//ConstDecl
class ConstDeclAST : public BaseAST {
  public:
    std::vector<std::unique_ptr<BaseAST>> const_def;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};
//BType
class BTypeAST : public BaseAST {
  public:
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};
//ConstDef
class ConstDefAST : public BaseAST {
  public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{
      const_val[ident] = const_init_val->calc(); 
      return const_val[ident];};
};
//ConstInitVal
class ConstInitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> const_exp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return const_exp->calc();};
};

class BlockItemAST : public BaseAST {
  public:
    enum TAG {DECL,STMT};
    TAG tag;
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};

class LValAST : public BaseAST {
  public:
    std::string ident;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

class ConstExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return exp->calc();};
};

//lv4.2
//VarDecl
class VarDeclAST : public BaseAST {
  public:
    std::vector<std::unique_ptr<BaseAST>> var_def;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};

//VarDef
class VarDefAST : public BaseAST {
  public:
    enum TAG {IDENT,INIT};
    TAG tag;
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    void Dump() const override;
    void Koopa() override;
    int calc() const override{return 0;};
};

class InitValAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> exp;
    void Dump() const override;
    void Koopa() override;
    int calc() const override;
};

class LeValAST : public BaseAST{
  public:
    std::string ident;
    void Dump() const override;
    void Koopa() override;
    int calc() const override {return 0;};
};