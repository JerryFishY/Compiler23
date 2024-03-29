#include "AST.h"
#include <iostream>
#include "symbol.h"
// To ensure no dependency for sysy compiler.
int index_sysy = 0;

// lv1 and lv2

// CompUnit
void CompUnitAST::Dump() const {
    std::cout << "CompUnit {";
    std::cout << "Decl in CompUnit {";
    for(auto &i : decl){
        i->Dump();
        std::cout << ", ";
    }
    std::cout<<"}, ";
    std::cout << "FuncDef in CompUnit {";
    for(auto &i : func_def){
        i->Dump();
        std::cout << ", \n";
    }
    std::cout<<"}";
    std::cout << "}";
}
void CompUnitAST::Koopa(){
    std::cout<<"decl @getint(): i32\ndecl @getch(): i32\ndecl @getarray(*i32): i32\ndecl @putint(i32)\ndecl @putch(i32)\ndecl @putarray(i32, *i32)\ndecl @starttime()\ndecl @stoptime()"<<"\n";

    for(auto &i : decl){
        DeclAST* decl = dynamic_cast<DeclAST*>(i.get());
        decl->global=1;
        decl->Koopa();
    }
    for(auto &i : func_def){
        i->Koopa();
    }
}

// FuncDef
void FuncDefAST::Dump() const {
    std::cout << "FuncDef {";
    if(tag == NOPARAM){
        func_type->Dump();
        std::cout << ", " << ident << ", ";
        block->Dump();
    }else if(tag == WITHPARAM){
        func_type->Dump();
        std::cout << ", " << ident << ", ";
        func_params->Dump();
        block->Dump();
    }
    std::cout << "}";
    
}
void FuncDefAST::Koopa() {
    std::cout << "fun ";
    std::cout << "@"<<ident<<"(";
    FuncFParamsAST* func_params = dynamic_cast<FuncFParamsAST*>(this->func_params.get());
    if(tag == WITHPARAM) {
        int idx= 0;
        for(auto &i : func_params->func_fparams){
            FuncFParamAST* func_param = dynamic_cast<FuncFParamAST*>(i.get());
            if(idx !=0){
                std::cout<<",";
            }
            std::cout << " @"<<func_param->ident<<": i32";
            idx++;
        }
    }
    std::cout<<") ";
    func_type->Koopa();
    std::cout<<"{\n";
    std::cout << "%entry"<<":\n";

    blk_idx++;
    block_tree[blk_idx] = now_idx;
    now_idx=blk_idx;
    if(tag == WITHPARAM) {
        func_params->Koopa();
    }
    //block index 
    if_dep++;
    now_if_dep = if_dep;
    int ifunctype = 0;
    BTypeAST* bfunc_type = dynamic_cast<BTypeAST*>(this->func_type.get());
    if(bfunc_type->tag == BTypeAST::VOID){
        ifunctype=0;
        func_type_dict[ident] = FUNC_VOID;
    }
    else{
        ifunctype=1;
        func_type_dict[ident] = FUNC_INT;
    }

    block->Koopa();

    if(!end_blk[now_if_dep]){
        if(ifunctype) std::cout<<"ret 0\n";
        else std::cout<<"ret\n";
    }

    now_idx = block_tree[now_idx];
    std::cout<<"}\n";
}

void BTypeAST::Dump() const {
    std::cout << "BType { ";
    if(tag == VOID){
        std::cout << "void";
    }
    else{
        std::cout << "int"; 
    }
    std::cout<< "}";
  }
void BTypeAST::Koopa(){
    if(tag == VOID){
        std::cout << "";
    }
    else{
        std::cout << ": i32";
    }
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
    else if(tag == WHILE){
        std::cout << "Stmt { while ";
        exp->Dump();
        std::cout << " ";
        whilestmt->Dump();
        std::cout<<"}";
    }else if(tag == BREAK){
        std::cout << "Stmt { break ";
        std::cout<<"}";
    }else if(tag == CONTINUE){
        std::cout << "Stmt { continue ";
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
    } else if(tag == WHILE){
        if(end_blk[now_if_dep]) return;

        while_idx++;
        while_block[while_idx]=now_while_idx;
        now_while_idx=while_idx;

        if(!end_blk[now_if_dep])std::cout<<"jump %whilecheck"<<now_while_idx<<"\n\n";
        std::cout<<"%whilecheck"<<now_while_idx<<":"<<std::endl;

        exp->Koopa();
        if(!end_blk[now_if_dep])std::cout<<"br %"<<index_sysy-1<<", %whilestmt"<<now_while_idx<<", %endwhile"<<now_while_idx<<"\n\n";
        std::cout<<"%whilestmt"<<now_while_idx<<":"<<std::endl;

        if_dep++;
        now_if_dep=if_dep;


        whilestmt->Koopa();
        if(!end_blk[now_if_dep])std::cout<<"jump %whilecheck"<<now_while_idx<<"\n\n";
        std::cout<<"%endwhile"<<now_while_idx<<":"<<std::endl;
        now_while_idx=while_block[now_while_idx];
        if_dep++;
        now_if_dep=if_dep;
    }else if(tag == BREAK){
        if(end_blk[now_if_dep]) return;
        std::cout<<"\tjump %endwhile"<<now_while_idx<<std::endl;
        end_blk[now_if_dep] = 1;

    }else if(tag == CONTINUE){
        if(end_blk[now_if_dep]) return;
        std::cout<<"\tjump %whilecheck"<<now_while_idx<<std::endl;
        end_blk[now_if_dep] = 1;
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
    else if(tag == PEXP) {
        pexp->Dump();
    }else if(tag == WITHFUNCR){
        std::cout << ident << ", ";
        func_rparams->Dump();
    }else if(tag == NOFUNCR){
        std::cout << ident << ", ";
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
    }else{
        if(tag==WITHFUNCR){
            std::vector <int> rparams;
            FuncRParamsAST* rfunc_rparams = dynamic_cast<FuncRParamsAST*>(this->func_rparams.get());
            rparams = rfunc_rparams->Koopa_vec();
            if(ident == "getint" || ident == "getch" || ident == "getarray" ||  func_type_dict[ident] == FUNC_INT){
            std::cout<<"%"<<index_sysy<<"= call @"<<ident<<"(";
            index_sysy++;
            }
            else{
                std::cout<<"call @"<<ident<<"(";
            }
            
            int idx=0;
            for(auto &i : rparams){
                if(idx==0)
                std::cout<<"%"<<i;
                else
                std::cout<<",%"<<i;
                idx++;
            }
            std::cout<<")\n";
        }
        else{
            if(ident == "getint" || ident == "getch" || ident == "getarray" ||  func_type_dict[ident] == FUNC_INT){
                std::cout<<"%"<<index_sysy<<" = call @"<<ident<<"()\n";
                index_sysy++;
            }
            else{
                std::cout<<"call @"<<ident<<"()\n";
            }
        }
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
        ConstDeclAST* dconst_decl = dynamic_cast<ConstDeclAST*>(this->const_decl.get());
        dconst_decl->global=global;
        dconst_decl->Koopa();
    }
    else{
        VarDeclAST* dvar_decl = dynamic_cast<VarDeclAST*>(this->var_decl.get());
        dvar_decl->global=global;
        dvar_decl->Koopa();
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
        ConstDefAST* dconst_def = dynamic_cast<ConstDefAST*>(i.get());
        dconst_def->global=global;
        dconst_def->Koopa();
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
    if(!global){
    std::string temp_name=ident+"_"+std::to_string(now_idx);
    var_type[temp_name] = CONST_INT;
    const_val[temp_name] = const_init_val->calc();
    // std::cout << var_type[temp_name] <<" "<<ident<< " = " << const_val[temp_name] << "\n";
    }else{
    std::string temp_name=ident+"_"+std::to_string(now_idx);
    var_type[temp_name] = CONST_INT;
    const_val[temp_name] = const_init_val->calc();
    }
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
        VarDefAST* dvar_def = dynamic_cast<VarDefAST*>(i.get());
        dvar_def->global=global;
        dvar_def->Koopa();
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
        if(!global){
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        std::cout<<"@"<<temp_name<<" = alloc i32 "<<"\n";
        }
        else{
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        std::cout<<"global @"<<temp_name<<" = alloc i32, 0"<<"\n";
        }
    }
    else{
        if(!global){
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        // const_val[temp_name] = init_val->calc();
        std::cout<<"@"<<temp_name<<" = alloc i32 "<<"\n";
        init_val->Koopa();
        std::cout<<"store %"<<index_sysy-1<<", @"<<temp_name<<"\n";
        // std::cout<<"var def"<<ident<< var_type[ident] <<const_val[temp_name]<<"\n";
        }else{
        std::string temp_name=ident+"_"+std::to_string(now_idx);
        var_type[temp_name] = VAR;
        const_val[temp_name] = init_val->calc();
        std::cout<<"global @"<<temp_name<<" = alloc i32, "<<const_val[temp_name]<<"\n";
        // init_val->Koopa();
        }
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

//FuncFParams
void FuncFParamsAST::Dump() const{
    std::cout << "FuncFParams { ";
    for(auto &i : func_fparams){
        i->Dump();
        std::cout << ", ";
    }
    std::cout << " }";
}
void FuncFParamsAST::Koopa(){
    for(auto &i : func_fparams){
        i->Koopa();
    }
}

//FuncFParam
void FuncFParamAST::Dump() const{
    std::cout << "FuncFParam { ";
    std::cout << ident;
    std::cout << " }";
}
void FuncFParamAST::Koopa(){
    std::cout<<"@"<<ident<<"_"<<now_idx<<" = alloc i32\n";
    std::cout<<"store @"<<ident<<", @"<<""<<ident+"_"+std::to_string(now_idx)<<"\n";
    var_type[ident+"_"+std::to_string(now_idx)]=VAR;
}
//FuncRParams
void FuncRParamsAST::Dump() const{
    std::cout << "FuncRParams { ";
    for(auto &i : func_rparams){
        i->Dump();
        std::cout << ", ";
    }
    std::cout << " }";
}
void FuncRParamsAST::Koopa(){
    // place holder
}
std::vector<int> FuncRParamsAST::Koopa_vec(){
    std::vector<int> rparam;
    for(auto &i : func_rparams){
        i->Koopa();
        rparam.push_back(index_sysy-1);
    }
    return rparam;
}