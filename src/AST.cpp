#include "AST.h"
#include <iostream>

// To ensure no dependency for sysy compiler.
int index_sysy = 0;

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
    exp->Dump();
    std::cout << "; }";
}
void StmtAST::Koopa() const{
    exp->Koopa();
    std::cout << "ret "<< "%" << index_sysy - 1<< "\n";
}

// BlockAST
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

// lv3.1
// ExpAST
void ExpAST::Dump() const {
    std::cout << "Exp { ";
    lorexp->Dump();
    std::cout << " }";
}
void ExpAST::Koopa() const{
    lorexp->Koopa();
}

// PrimaryExpAST
void PrimaryExpAST::Dump() const {
    std::cout << "PrimaryExp { ";
    if(tag == EXP){
        exp->Dump();
    }
    else{
        std::cout << number;
    }
    std::cout << " }";
}
void PrimaryExpAST::Koopa() const{
    if(tag == EXP){
        exp->Koopa();
    }
    else{
        std::cout <<"%"<<index_sysy <<" = add 0, "<<number<<"\n";
        index_sysy++;
    }
}

// UnaryExpAST
void UnaryExpAST::Dump() const {
    std::cout << "UnaryExp { ";
    if(tag == OPUEXP){
        std::cout << op << ", ";
        uexp->Dump();
    }
    else{
        pexp->Dump();
    }
    std::cout << " }";
}
void UnaryExpAST::Koopa() const{
    if(tag == OPUEXP){
        if(op == "-"){
            uexp->Koopa();
            std::cout << "%"<<index_sysy<< " = sub 0, %"<<index_sysy - 1 <<"\n";
            index_sysy++;
        }
        else if(op == "!"){
            uexp->Koopa();
            std::cout << "%"<<index_sysy<< " = eq %"<<index_sysy - 1<<", 0\n";
            index_sysy++;
        }else{
            uexp->Koopa();
        }
    }else if(tag == PEXP){
        pexp->Koopa();
    }
}

// lv3.2
// AddExpAST
void AddExpAST::Dump() const {
    std::cout << "AddExp { ";
    if(tag == AddExpAST::MEXP){
        mexp->Dump();
    }
    else{
        aexp->Dump();
        std::cout << op << ", ";
        mexp->Dump();
    }
    std::cout << " }";
}
void AddExpAST::Koopa() const{
    if(tag == AddExpAST::MEXP){
        mexp->Koopa();
    }
    else{
        aexp->Koopa();
        int now1 = index_sysy - 1;
        mexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "+"){
            std::cout << "%"<<index_sysy<< " = add %"<<now1<<", %"<<now2 <<"\n";
            index_sysy++;
        }
        else if(op == "-"){
            std::cout << "%"<<index_sysy<< " = sub %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
    }
}

// MulExpAST
void MulExpAST::Dump() const {
    std::cout << "MulExp { ";
    if(tag == MulExpAST::UEXP){
        uexp->Dump();
    }
    else{
        mexp->Dump();
        std::cout << op << ", ";
        uexp->Dump();
    }
    std::cout << " }";
}
void MulExpAST::Koopa() const{
    if(tag == MulExpAST::UEXP){
        uexp->Koopa();
    }
    else{
        mexp->Koopa();
        int now1 = index_sysy - 1;
        uexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "*"){
            std::cout << "%"<<index_sysy<< " = mul %"<<now1<<", %"<<now2 <<"\n";
            index_sysy++;
        }
        else if(op == "/"){
            std::cout << "%"<<index_sysy<< " = div %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
        else if(op == "%"){
            std::cout << "%"<<index_sysy<< " = mod %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
    }
}

// lv3.3
// RelExpAST
void RelExpAST::Dump() const {
    if(tag == RelExpAST::AEXP){
        aexp->Dump();
    }
    else{
        relexp->Dump();
        std::cout << op << ", ";
        aexp->Dump();
    }
}
void RelExpAST::Koopa() const{
    if(tag==AEXP){
        aexp->Koopa();
    }
    else{
        relexp->Koopa();
        int now1 = index_sysy - 1;
        aexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "<"){
            std::cout << "%"<<index_sysy<< " = lt %"<<now1<<", %"<<now2 <<"\n";
            index_sysy++;
        }
        else if(op == ">"){
            std::cout << "%"<<index_sysy<< " = gt %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
        else if(op == "<="){
            std::cout << "%"<<index_sysy<< " = le %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
        else if(op == ">="){
            std::cout << "%"<<index_sysy<< " = ge %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
    }
}

// EqExpAST
void EqExpAST::Dump() const {
    if(tag == EqExpAST::RELEXP){
        relexp->Dump();
    }
    else{
        eqexp->Dump();
        std::cout << op << ", ";
        relexp->Dump();
    }
}
void EqExpAST::Koopa() const{
    if(tag == EqExpAST::RELEXP){
        relexp->Koopa();
    }
    else{
        eqexp->Koopa();
        int now1 = index_sysy - 1;
        relexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "=="){
            std::cout << "%"<<index_sysy<< " = eq %"<<now1<<", %"<<now2 <<"\n";
            index_sysy++;
        }
        else if(op == "!="){
            std::cout << "%"<<index_sysy<< " = ne %"<<now1<<", %"<<now2<<"\n";
            index_sysy++;
        }
    }
}

// LAndExpAST
void LAndExpAST::Dump() const {
    if(tag == LAndExpAST::EQEXP){
        eqexp->Dump();
    }
    else{
        landexp->Dump();
        std::cout << op << ", ";
        eqexp->Dump();
    }
}
void LAndExpAST::Koopa() const{
    if(tag == LAndExpAST::EQEXP){
        eqexp->Koopa();
    }
    else{
        landexp->Koopa();
        int now1 = index_sysy - 1;
        eqexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "&&"){
            std::cout << "%"<<index_sysy<< " = eq %"<<now1<<", 0" <<"\n";
            index_sysy++;
            std::cout << "%"<<index_sysy<< " = eq %"<<now2<<", 0" <<"\n";
            index_sysy++;
            std::cout << "%"<<index_sysy<< " = or %"<<index_sysy - 2<<", %"<<index_sysy - 1 <<"\n";
            index_sysy++;
            std::cout << "%"<<index_sysy<< " = eq %"<<index_sysy - 1<<", 0\n";
            index_sysy++;
        }
    }
}

// LOrExpAST
void LOrExpAST::Dump() const {
    if(tag == LOrExpAST::LANDEXP){
        landexp->Dump();
    }
    else{
        lorexp->Dump();
        std::cout << op << ", ";
        landexp->Dump();
    }
}
void LOrExpAST::Koopa() const{
    if(tag == LOrExpAST::LANDEXP){
        landexp->Koopa();
    }
    else{
        lorexp->Koopa();
        int now1 = index_sysy - 1;
        landexp->Koopa();
        int now2 = index_sysy - 1;
        if(op == "||"){
            std::cout << "%"<<index_sysy<< " = ne %"<<now1<<", 0" <<"\n";
            index_sysy++;
            std::cout << "%"<<index_sysy<< " = ne %"<<now2<<", 0" <<"\n";
            index_sysy++;
            std::cout << "%"<<index_sysy<< " = or %"<<index_sysy - 2<<", %"<<index_sysy - 1 <<"\n";
            index_sysy++;
        }
    }
}
