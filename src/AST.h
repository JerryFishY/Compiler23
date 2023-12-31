#include <iostream>
#include <memory>
#include <string>

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

// Base class for all AST
class BaseAST {
 public:
  virtual ~BaseAST() = default;     
  virtual void Dump() const = 0;    // Dump the derived AST
  virtual void Koopa() const = 0;   // Give the derived Koopa code
};

// lv1-lv2
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

// FuncType
class FuncTypeAST : public BaseAST {
 public:
  void Dump() const override;
  void Koopa() const override;
};

// StmtAST
class StmtAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> exp;
  void Dump() const override;
  void Koopa() const override;
};

// BlockAST
class BlockAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> stmt;
  void Dump() const override;
  void Koopa() const override;
};

// lv3.1
// ExpAST
class ExpAST : public BaseAST {
  public:
    std::unique_ptr<BaseAST> lorexp;
    void Dump() const override;
    void Koopa() const override;
};

// PrimaryExpAST
class PrimaryExpAST : public BaseAST {
  public:
    enum TAG {EXP,NUM};
    TAG tag;
    std::unique_ptr<BaseAST> exp;
    int number;
    void Dump() const override;
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
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
    void Koopa() const override;
};