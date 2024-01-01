#include "AST.h"
#include <iostream>
#include "symbol.h"
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
    if(tag == RETURN){
        std::cout << "Stmt { return ";
        exp->Dump();
    }
    else{
    std::cout << "Stmt { ";
        lval->Dump();
        exp->Dump();
    }
    std::cout << "; }";
}
void StmtAST::Koopa() const{
    if(tag == RETURN){
        exp->Koopa();
        std::cout << "ret "<< "%" << index_sysy - 1<< "\n";
    }
    else{
        exp->Koopa();
        lval->Koopa();
    }
}

// BlockAST
void BlockAST::Dump() const {
    std::cout << "Block { ";
    for(auto &i : block_item){
        i->Dump();
        std::cout << ", ";
    }
    std::cout << " }";
}
void BlockAST::Koopa() const{
    std::cout << "{\n";
    std::cout << "%entry:\n";
    for(auto &i : block_item){
        i->Koopa();
    }
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
    else if(tag == NUM){
        std::cout << number;
    } 
    else{
        exp->Dump();
    }
    std::cout << " }";
}
void PrimaryExpAST::Koopa() const{
    if(tag == EXP){
        exp->Koopa();
    }else if(tag == LVal){
        exp->Koopa();
    }
    else{
        std::cout <<"%"<<index_sysy <<" = add 0, "<<number<<"\n";
        index_sysy++;
    }
}
int PrimaryExpAST::calc() const{
    if(tag == EXP){
        return exp->calc();
    }
    else if(tag == NUM){
        return number;
    }
    return exp->calc(); // should not touch here
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
int UnaryExpAST::calc() const{
    if(tag == OPUEXP){
        if(op=="-")
            return -uexp->calc();
        else if(op=="!")
            return !uexp->calc();
        else
            return uexp->calc();
    }
    else{
        return pexp->calc();
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
int AddExpAST::calc() const{
    if(tag == AddExpAST::MEXP){
        return mexp->calc();
    }
    else{
        if(op=="+")
            return aexp->calc()+mexp->calc();
        else
            return aexp->calc()-mexp->calc();
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
int MulExpAST::calc() const{
    if(tag == MulExpAST::UEXP){
        return uexp->calc();
    }
    else{
        if(op=="*")
            return mexp->calc()*uexp->calc();
        else if(op=="/")
            return mexp->calc()/uexp->calc();
        else
            return mexp->calc()%uexp->calc();
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
int RelExpAST::calc() const{
    if(tag == RelExpAST::AEXP){
        return aexp->calc();
    }
    else{
        if(op=="<")
            return relexp->calc()<aexp->calc();
        else if(op==">")
            return relexp->calc()>aexp->calc();
        else if(op=="<=")
            return relexp->calc()<=aexp->calc();
        else if(op==">=")
            return relexp->calc()>=aexp->calc();
    }
    return 0; // should not touch here
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
int EqExpAST::calc() const{
    if(tag == EqExpAST::RELEXP){
        return relexp->calc();
    }
    else{
        if(op=="==")
            return eqexp->calc()==relexp->calc();
        else
            return eqexp->calc()!=relexp->calc();
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
int LAndExpAST::calc() const{
    if(tag == LAndExpAST::EQEXP){
        return eqexp->calc();
    }
    else{
        return landexp->calc()&&eqexp->calc();
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
int LOrExpAST::calc() const{
    if(tag == LOrExpAST::LANDEXP){
        return landexp->calc();
    }
    else{
        return lorexp->calc()||landexp->calc();
    }
}
//Decl
void DeclAST::Dump() const{
    std::cout << "Decl { ";
    if(tag == DeclAST::CONST){
    const_decl->Dump();
    }else{
    var_decl->Dump();
    }
    std::cout << " }";
}
void DeclAST::Koopa() const{
    if(tag == DeclAST::CONST){
        const_decl->Koopa();
    }
    else{
        var_decl->Koopa();
    }
}

//ConstDecl
void ConstDeclAST::Dump() const{
    std::cout << "ConstDecl { ";
    for(auto &i : const_def){
        i->Dump();
        std::cout << ", ";
    }
    std::cout << " }";
}
void ConstDeclAST::Koopa() const{
    for(auto &i : const_def){
        i->Koopa();
    }
}

//ConstDef
void ConstDefAST::Dump() const{
    std::cout << "ConstDef { ";
    std::cout << ident << ", ";
    const_init_val->Dump();
    std::cout << " }";
}
void ConstDefAST::Koopa() const{
    var_type[ident] = CONST_INT;
    const_val[ident] = const_init_val->calc();
    // std::cout << var_type[ident] <<" "<<ident<< " = " << const_val[ident] << "\n";
}
//ConstInitVal
void ConstInitValAST::Dump() const{
    std::cout << "ConstInitVal { ";
    const_exp->Dump();
    std::cout << " }";
}
void ConstInitValAST::Koopa() const{
    //place holder
}

//BlockItem
void BlockItemAST::Dump() const{
    std::cout << "BlockItem { ";
    if(tag == BlockItemAST::DECL){
        decl->Dump();
    }
    else{
        stmt->Dump();
    }
    std::cout << " }";
}
void BlockItemAST::Koopa() const{
    if(tag == BlockItemAST::DECL){
        decl->Koopa();
    }
    else{
        stmt->Koopa();
    }
}

//LVal
void LValAST::Dump() const{
    std::cout << "LVal { ";
    std::cout << ident;
    std::cout << " }";
}
void LValAST::Koopa() const{
    if(var_type[ident] == CONST_INT){
    std::cout<<"%"<<index_sysy<<" = add 0 ,"<<const_val[ident]<<"\n";
    index_sysy++;}
    else{
        std::cout<<"%"<<index_sysy<<" = load @"<<ident<<"\n";
        index_sysy++;
    }
}
int LValAST::calc() const{
    return const_val[ident];
}
//ConstExp
void ConstExpAST::Dump() const{
    std::cout << "ConstExp { ";
    exp->Dump();
    std::cout << " }";
}
void ConstExpAST::Koopa() const{
    //place holder
}

//VarDeclAST
void VarDeclAST::Dump() const{
    std::cout << "VarDecl { ";
    for(auto &i : var_def){
        i->Dump();
        std::cout << ", ";
    }
    std::cout << " }";
}
void VarDeclAST::Koopa() const{
    for(auto &i : var_def){
        i->Koopa();
    }
}

//VarDef
void VarDefAST::Dump() const{
    std::cout << "VarDef { ";
    std::cout << ident << ", ";
    if(tag == VarDefAST::INIT){
        init_val->Dump();
    }
    std::cout << " }";
}
void VarDefAST::Koopa() const{
    if(tag == IDENT){
        var_type[ident] = VAR;
        std::cout<<"@"<<ident<<" = alloc i32 "<<"\n";
    }
    else{
        var_type[ident] = VAR;
        const_val[ident] = init_val->calc();
        std::cout<<"@"<<ident<<" = alloc i32 "<<"\n";
        init_val->Koopa();
        std::cout<<"store %"<<index_sysy-1<<", @"<<ident<<"\n";
        // std::cout<<"var def"<<ident<< var_type[ident] <<const_val[ident]<<"\n";
    }
}

//InitVal
void InitValAST::Dump() const{
    std::cout << "InitVal { ";
    exp->Dump();
    std::cout << " }";
}
void InitValAST::Koopa() const{
    exp->Koopa();
}
int InitValAST::calc() const{
    return exp->calc();
}

//LeVal
void LeValAST::Dump() const{
    std::cout << "LeVal { ";
    std::cout << ident;
    std::cout << " }";
}
void LeValAST::Koopa() const{
    std::cout<<"store %"<<index_sysy-1<<", @"<<ident<<"\n";
}