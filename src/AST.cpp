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
void CompUnitAST::Koopa(){
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
void FuncDefAST::Koopa() {
    std::cout << "fun ";
    std::cout << "@"<<ident<<"(): ";
    func_type->Koopa();
    std::cout<<"{\n";
    std::cout << "%entry:\n";
    block->Koopa();
    std::cout<<"}\n";
}

// FuncType
void FuncTypeAST::Dump() const {
    std::cout << "FuncType { int }";
  }
void FuncTypeAST::Koopa(){
    std::cout << "i32 ";
}

// StmtAST
void StmtAST::Dump() const {
    if(tag == RETURN){
        std::cout << "Stmt { return ";
        exp->Dump();
        std::cout<<"}";
    }
    else if(tag == BLOCK){
        std::cout << "Stmt { ";
        block->Dump();
        std::cout<<"}";
    }
    else if(tag == EXP){
        std::cout << "Stmt { ";
        exp->Dump();
        std::cout<<"}";
    }
    else if(tag == ASSIGN){
    std::cout << "Stmt { ";
        lval->Dump();
        exp->Dump();
        std::cout<<"}";
    }else if(tag == EMPTY){
        std::cout << "Stmt { ";
        std::cout<<"}";
    }else if(tag == IF){
        std::cout << "Stmt { if ";
        ifexp->Dump();
        std::cout << " ";
        ifstmt->Dump();
        std::cout << "; }";
    }else if(tag == IFELSE){
        std::cout << "Stmt { if ";
        ifexp->Dump();
        std::cout << " else";
        ifstmt->Dump();
        elsestmt->Dump();
        std::cout<<"}";
        }

}
void StmtAST::Koopa(){
    //place holder
    if(tag == RETURN){
        if(end_blk[now_if_dep]) return;
        end_blk[now_if_dep] = 1;
        exp->Koopa();
        std::cout << "ret "<< "%" << index_sysy - 1<< "\n";
    }
    else if(tag == ASSIGN){
        if(end_blk[now_if_dep]) return;
        exp->Koopa();
        lval->Koopa();
    }else if(tag == EMPTY){
        //do nothing
    }else if(tag == EXP){
        if(end_blk[now_if_dep]) return;
        exp->Koopa();
    }else if(tag == BLOCK){
        if(end_blk[now_if_dep]) return;
        block->Koopa();
    }else if(tag == IF){
        if(end_blk[now_if_dep]) return;
        if_idx++;
        int now_if_idx=if_idx;

        ifexp->Koopa();
        std::cout<<"br %"<<index_sysy-1<<", %if"<<now_if_idx<<", %end"<<now_if_idx<<"\n\n";

        std::cout<<"%if"<<now_if_idx<<":\n";
        if_dep++;
        now_if_dep = if_dep;
        ifstmt->Koopa();
        if(!end_blk[now_if_dep])std::cout<<"jump %end"<<now_if_idx<<"\n\n";
        std::cout<<"%end"<<now_if_idx<<":"<<"\n";
        if_dep++;
        now_if_dep=if_dep;
    } else if(tag == IFELSE){
        if(end_blk[now_if_dep]) return;
        if_idx++;
        int now_if_idx=if_idx;

        ifexp->Koopa();
        std::cout<<"br %"<<index_sysy-1<<", %if"<<now_if_idx<<", %else"<<now_if_idx<<"\n\n";
        std::cout<<"%if"<<now_if_idx<<":\n";

        if_dep++;
        now_if_dep = if_dep;

        ifstmt->Koopa();
        if(!end_blk[now_if_dep])std::cout<<"jump %end"<<now_if_idx<<"\n\n";
        std::cout<<"\n%else"<<now_if_idx<<":\n";
        if_dep++;
        now_if_dep = if_dep;
        elsestmt->Koopa();
        if(!end_blk[now_if_dep])std::cout<<"jump %end"<<now_if_idx<<"\n\n";
        std::cout<<"\n%end"<<now_if_idx<<":"<<std::endl;
        if_dep++;
        now_if_dep=if_dep;
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
void BlockAST::Koopa(){
    blk_idx++;
    block_tree[blk_idx] = now_idx;
    now_idx=blk_idx;

    for(auto &i : block_item){
        i->Koopa();
    }

    now_idx = block_tree[now_idx];
}

// lv3.1
// ExpAST
void ExpAST::Dump() const {
    std::cout << "Exp { ";
    lorexp->Dump();
    std::cout << " }";
}
void ExpAST::Koopa(){
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
void PrimaryExpAST::Koopa(){
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
void UnaryExpAST::Koopa(){
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
void AddExpAST::Koopa(){
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
void MulExpAST::Koopa(){
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
void RelExpAST::Koopa(){
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
void EqExpAST::Koopa(){
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
void LAndExpAST::Koopa(){
    if(tag == LAndExpAST::EQEXP){
        eqexp->Koopa();
    }
    else{
        landexp->Koopa();
        int now1 = index_sysy - 1;
        int if_res = index_sysy; 
        std::cout<<"@result_"<<if_res<<" = alloc i32"<<"\n"<<"%"<<if_res<<"= ne 0, %"<<now1<<"\n"<<"store %"<<if_res<<", @result_"<<if_res<<"\n";
        index_sysy++;

        if_idx++;
        int now_if_idx=if_idx;
        std::cout<<"br %"<<if_res<<", %if"<<now_if_idx<<", %end"<<now_if_idx<<"\n\n";
        std::cout<<"%if"<<now_if_idx<<":\n";
        eqexp->Koopa();
        int now2=index_sysy-1;
        std::cout<<"%"<<index_sysy<<"= ne 0, %"<<now2<<"\n";
        index_sysy++;
        std::cout<<"store "<<'%'<<index_sysy-1<<", @result_"<<if_res<<std::endl;
        std::cout<<"jump %end"<<now_if_idx<<"\n\n";
        std::cout<<"%end"<<now_if_idx<<":"<<std::endl;
        std::cout<<"\t%"<<index_sysy<<"= load @result_"<<if_res<<std::endl;
        index_sysy++;
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
        std::cout<<"LOrExp {";
        landexp->Dump();
        std::cout<<"}";
    }
    else{
        std::cout<<"LOrExp {";
        lorexp->Dump();
        std::cout << op << ", ";
        landexp->Dump();
        std::cout<<"}";
    }
}
void LOrExpAST::Koopa(){
    if(tag == LOrExpAST::LANDEXP){
        landexp->Koopa();
    }
    else{
        lorexp->Koopa();
        int now1 = index_sysy - 1;
        int if_res = index_sysy; 
        std::cout<<"@result_"<<if_res<<" = alloc i32"<<"\n"<<"%"<<if_res<<"= eq 0, %"<<now1<<"\n";
        index_sysy++;
        std::cout<<"%"<<index_sysy<<"= ne 0, %"<<now1<<"\n"<<"store %"<<index_sysy<<", @result_"<<if_res<<"\n";
        index_sysy++;

        if_idx++;
        int now_if_idx=if_idx;
        std::cout<<"br %"<<if_res<<", %if"<<now_if_idx<<", %end"<<now_if_idx<<"\n\n";
        std::cout<<"%if"<<now_if_idx<<":\n";
        landexp->Koopa();
        int now2=index_sysy-1;
        std::cout<<"%"<<index_sysy<<"= ne 0, %"<<now2<<std::endl;
        index_sysy++;
        std::cout<<"store "<<'%'<<index_sysy-1<<", @result_"<<if_res<<std::endl;
        std::cout<<"jump %end"<<now_if_idx<<"\n\n";
        std::cout<<"%end"<<now_if_idx<<":"<<"\n";
        std::cout<<"\t%"<<index_sysy<<"= load @result_"<<if_res<<std::endl;
        index_sysy++;
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
void DeclAST::Koopa(){
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
void ConstDeclAST::Koopa(){
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
void ConstDefAST::Koopa(){
    std::string temp_name=ident+"_"+std::to_string(now_idx);
    var_type[temp_name] = CONST_INT;
    const_val[temp_name] = const_init_val->calc();
    // std::cout << var_type[temp_name] <<" "<<ident<< " = " << const_val[temp_name] << "\n";
}
//ConstInitVal
void ConstInitValAST::Dump() const{
    std::cout << "ConstInitVal { ";
    const_exp->Dump();
    std::cout << " }";
}
void ConstInitValAST::Koopa(){
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
void BlockItemAST::Koopa(){
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
void LValAST::Koopa(){
    int temp_idx = now_idx;
    while(var_type.find(ident+"_"+std::to_string(temp_idx)) == var_type.end()){
        temp_idx = block_tree[temp_idx];
    }
    std::string temp_name=ident+"_"+std::to_string(temp_idx);
    if(var_type[temp_name] == CONST_INT){
    std::cout<<"%"<<index_sysy<<" = add 0 ,"<<const_val[temp_name]<<"\n";
    index_sysy++;}
    else{
        std::cout<<"%"<<index_sysy<<" = load @"<<temp_name<<"\n";
        index_sysy++;
    }
}
int LValAST::calc() const{
    int temp_idx = now_idx;
    while(var_type.find(ident+"_"+std::to_string(temp_idx)) == var_type.end()){
        temp_idx = block_tree[temp_idx];
    }
    std::string temp_name=ident+"_"+std::to_string(temp_idx);
    return const_val[temp_name];
}
//ConstExp
void ConstExpAST::Dump() const{
    std::cout << "ConstExp { ";
    exp->Dump();
    std::cout << " }";
}
void ConstExpAST::Koopa(){
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
void VarDeclAST::Koopa(){
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
void VarDefAST::Koopa(){
    if(tag == IDENT){
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        std::cout<<"@"<<temp_name<<" = alloc i32 "<<"\n";
    }
    else{
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        const_val[temp_name] = init_val->calc();
        std::cout<<"@"<<temp_name<<" = alloc i32 "<<"\n";
        init_val->Koopa();
        std::cout<<"store %"<<index_sysy-1<<", @"<<temp_name<<"\n";
        // std::cout<<"var def"<<ident<< var_type[ident] <<const_val[temp_name]<<"\n";
    }
}

//InitVal
void InitValAST::Dump() const{
    std::cout << "InitVal { ";
    exp->Dump();
    std::cout << " }";
}
void InitValAST::Koopa(){
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
void LeValAST::Koopa(){
    int temp_idx = now_idx;
    while(var_type.find(ident+"_"+std::to_string(temp_idx)) == var_type.end()){
        temp_idx = block_tree[temp_idx];
    }
    std::string temp_name=ident+"_"+std::to_string(temp_idx);
    std::cout<<"store %"<<index_sysy-1<<", @"<<temp_name<<"\n";
}