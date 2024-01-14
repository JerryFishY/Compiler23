%code requires {
  #include <memory>
  #include <string>
  #include "AST.h"
}

%{
#include <iostream>
#include <cstring>
#include <memory>
#include <string>
#include "AST.h"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;
%}

%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval
%union {
  std::string *str_val;
  int int_val;
  char char_val;
  BaseAST *ast_val;
}

// Token declaration
%token INT RETURN LE GE EQ NE AND OR CONST IF ELSE WHILE BREAK CONTINUE VOID
%token <str_val> IDENT
%token <int_val> INT_CONST

// Non-terminal type
%type <ast_val> FuncDef Block Stmt Exp UnaryExp  PrimaryExp AddExp MulExp LOrExp LAndExp EqExp RelExp Decl ConstDecl BType ConstDef ConstDefList ConstInitVal BlockItem LVal ConstExp LeVal BlockItemList VarDecl VarDef InitVar VarDefList FuncFParams FuncFParam FuncRParams CompUnitList DefDecl
%type <int_val> Number
%type <char_val> UnaryOp

%%

// Parsing rule
// Lv1-Lv2
CompUnit
  : CompUnitList{
    auto comp_unit = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;
CompUnitList
  : DefDecl {
    $$ = $1;
  }
  | CompUnitList DefDecl {
    auto comp_unit = new CompUnitAST();
    auto list1 = (CompUnitAST* )$1;
    auto list2 = (CompUnitAST* )$2;
    for(auto &func : list1->func_def) {
      comp_unit->func_def.emplace_back(func.release());
    }
    for(auto &func : list2->func_def) {
      comp_unit->func_def.emplace_back(func.release());
    }
    for(auto &decl_ : list1->decl) {
      comp_unit->decl.emplace_back(decl_.release());
    }
    for(auto &decl_ : list2->decl) {
      comp_unit->decl.emplace_back(decl_.release());
    }
    $$ = comp_unit;
  }
  ;
DefDecl
  : Decl{
    auto comp_unit = new CompUnitAST();
    comp_unit->decl.emplace_back($1);
    $$=comp_unit;
  }
  | FuncDef {
    auto comp_unit = new CompUnitAST();
    comp_unit->func_def.emplace_back($1);
    $$=comp_unit;
  } 
  ;

FuncDef
  : BType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->tag=FuncDefAST::NOPARAM;
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | BType IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->tag=FuncDefAST::WITHPARAM;
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->func_params = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
  }
  ;


FuncFParams
  : FuncFParam ',' FuncFParams {
    auto ast = new FuncFParamsAST();
    auto ast_param = unique_ptr<FuncFParamsAST>((FuncFParamsAST*)$3);
    ast->func_fparams.emplace_back($1); 
    for(auto &fparam : ast_param->func_fparams) {
      ast->func_fparams.emplace_back(fparam.release());
    }
    $$ = ast;
  }
  | FuncFParam {
    auto ast = new FuncFParamsAST();
    ast->func_fparams.emplace_back($1);
    $$ = ast;
  }
  ;
FuncFParam
  : BType IDENT {
    auto ast = new FuncFParamAST();
    ast->ident = *unique_ptr<string>($2);
    $$ = ast;
  }
  ;
Block
  : '{' BlockItemList '}' {
    $$=$2;
  }
  ;

BlockItemList
  : {
    auto ast = new BlockAST();
    $$ = ast;
  }
  | BlockItem BlockItemList {
    auto ast = new BlockAST();
    auto ast_item_list = (BlockAST*)$2;
    ast->block_item.emplace_back($1);
    int n = ast_item_list->block_item.size();
    for(int i=0; i < n; i++) {
      ast->block_item.emplace_back(ast_item_list->block_item[i].release());
    }
    $$ = ast;
  }
  ;

Stmt
  : RETURN Exp';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2); 
    ast->tag = StmtAST::RETURN;
    $$ = ast;
  }
  | LeVal '=' Exp ';' {
    auto ast = new StmtAST();
    ast->tag = StmtAST::ASSIGN;
    ast->lval = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | Block {
    auto ast = new StmtAST();
    ast->tag = StmtAST::BLOCK;
    ast->block = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | ';' {
    auto ast = new StmtAST();
    ast->tag = StmtAST::EMPTY;
    $$ = ast;
  }
  | Exp ';'{
    auto ast = new StmtAST();
    ast->tag = StmtAST::EXP;
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->tag = StmtAST::IF;
    ast->ifexp = unique_ptr<BaseAST>($3);
    ast->ifstmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt ELSE Stmt {
    auto ast = new StmtAST();
    ast->tag = StmtAST::IFELSE;
    ast->ifexp = unique_ptr<BaseAST>($3);
    ast->ifstmt = unique_ptr<BaseAST>($5);
    ast->elsestmt = unique_ptr<BaseAST>($7);
    $$ = ast;
  }
  | WHILE '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->tag = StmtAST::WHILE;
    ast->exp = unique_ptr<BaseAST>($3);
    ast->whilestmt = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | BREAK ';' {
    auto ast = new StmtAST();
    ast->tag = StmtAST::BREAK;
    $$ = ast;
  }
  | CONTINUE ';' {
    auto ast = new StmtAST();
    ast->tag = StmtAST::CONTINUE;
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    int number;
    number = $1;
    $$ = number; 
  }
  ;

//Lv3.1
Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->lorexp = unique_ptr<BaseAST>($1);
    $$ = ast;
  };

PrimaryExp
  : Number {
    auto ast = new PrimaryExpAST();
    ast->tag = PrimaryExpAST::NUM;
    ast->number = $1;
    $$ = ast;
  }
  | '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->tag = PrimaryExpAST::EXP;
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->tag = PrimaryExpAST::LVal;
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

UnaryExp
  :PrimaryExp{
    auto ast = new UnaryExpAST();
    ast->tag = UnaryExpAST::PEXP;
    ast->pexp = unique_ptr<BaseAST>($1);
    $$ = ast;
  } 
  | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST();
    ast->tag = UnaryExpAST::OPUEXP;
    ast->op = $1;
    ast->uexp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | IDENT '(' ')' {
    auto ast = new UnaryExpAST();
    ast->tag = UnaryExpAST::NOFUNCR;
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  | IDENT '(' FuncRParams ')' {
    auto ast = new UnaryExpAST();
    ast->tag = UnaryExpAST::WITHFUNCR;
    ast->ident = *unique_ptr<string>($1);
    ast->func_rparams = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
FuncRParams
  : Exp {
    auto ast = new FuncRParamsAST();
    ast->func_rparams.emplace_back($1);
    $$ = ast;
  }
  | Exp ',' FuncRParams {
    auto ast = new FuncRParamsAST();
    auto ast_param = unique_ptr<FuncRParamsAST>((FuncRParamsAST* )$3);
    ast->func_rparams.emplace_back($1);
    int n = ast_param->func_rparams.size();
    for(int i = 0; i < n; i++) {
      ast->func_rparams.emplace_back(ast_param->func_rparams[i].release());
    }
    $$ = ast;
  }
  ;
UnaryOp
  : '+' {
    $$ = '+';
  }
  | '-' {
    $$ = '-';
  }
  | '!' {
    $$ = '!';
  }

AddExp
  : MulExp {
    auto ast = new AddExpAST();
    ast->mexp = unique_ptr<BaseAST>($1);
    ast->tag = AddExpAST::MEXP;
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST();
    ast->aexp = unique_ptr<BaseAST>($1);
    ast->mexp = unique_ptr<BaseAST>($3);
    ast->op='+';
    ast->tag = AddExpAST::AEXP;
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST();
    ast->aexp = unique_ptr<BaseAST>($1);
    ast->mexp = unique_ptr<BaseAST>($3);
    ast->op='-';
    ast->tag = AddExpAST::AEXP;
    $$ = ast;
  }
  ;
MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->uexp = unique_ptr<BaseAST>($1);
    ast->tag = MulExpAST::UEXP;
    $$ = ast;
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST();
    ast->mexp = unique_ptr<BaseAST>($1);
    ast->uexp = unique_ptr<BaseAST>($3);
    ast->op='*';
    ast->tag = MulExpAST::MUEXP;
    $$ = ast;
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST();
    ast->mexp = unique_ptr<BaseAST>($1);
    ast->uexp = unique_ptr<BaseAST>($3);
    ast->op='/';
    ast->tag = MulExpAST::MUEXP;
    $$ = ast;
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST();
    ast->mexp = unique_ptr<BaseAST>($1);
    ast->uexp = unique_ptr<BaseAST>($3);
    ast->op='%';
    ast->tag = MulExpAST::MUEXP;
    $$ = ast;
  }
  ;
RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->aexp = unique_ptr<BaseAST>($1);
    ast->tag = RelExpAST::AEXP;
    $$ = ast;
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST();
    ast->relexp = unique_ptr<BaseAST>($1);
    ast->aexp = unique_ptr<BaseAST>($3);
    ast->op = '<';
    ast->tag = RelExpAST::RELEXP;
    $$ = ast;
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST();
    ast->relexp = unique_ptr<BaseAST>($1);
    ast->aexp = unique_ptr<BaseAST>($3);
    ast->op = '>';
    ast->tag = RelExpAST::RELEXP;
    $$ = ast;
  }
  | RelExp LE AddExp {
    auto ast = new RelExpAST();
    ast->relexp = unique_ptr<BaseAST>($1);
    ast->aexp = unique_ptr<BaseAST>($3);
    ast->op = "<=";
    ast->tag = RelExpAST::RELEXP;
    $$ = ast;
  }
  | RelExp GE AddExp {
    auto ast = new RelExpAST();
    ast->relexp = unique_ptr<BaseAST>($1);
    ast->aexp = unique_ptr<BaseAST>($3);
    ast->op = ">=";
    ast->tag = RelExpAST::RELEXP;
    $$ = ast;
  }

EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->relexp = unique_ptr<BaseAST>($1);
    ast->tag = EqExpAST::RELEXP;
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST();
    ast->eqexp = unique_ptr<BaseAST>($1);
    ast->relexp = unique_ptr<BaseAST>($3);
    ast->op = "==";
    ast->tag = EqExpAST::EQEXP;
    $$ = ast;
  }
  | EqExp NE RelExp {
    auto ast = new EqExpAST();
    ast->eqexp = unique_ptr<BaseAST>($1);
    ast->relexp = unique_ptr<BaseAST>($3);
    ast->op = "!=";
    ast->tag = EqExpAST::EQEXP;
    $$ = ast;
  }
LAndExp
  : EqExp {
    auto ast = new LAndExpAST();
    ast->eqexp = unique_ptr<BaseAST>($1);
    ast->tag = LAndExpAST::EQEXP;
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST();
    ast->landexp = unique_ptr<BaseAST>($1);
    ast->eqexp = unique_ptr<BaseAST>($3);
    ast->op = "&&";
    ast->tag = LAndExpAST::LANDEXP;
    $$ = ast;
  }
LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->landexp = unique_ptr<BaseAST>($1);
    ast->tag = LOrExpAST::LANDEXP;
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST();
    ast->lorexp = unique_ptr<BaseAST>($1);
    ast->landexp = unique_ptr<BaseAST>($3);
    ast->op = "||";
    ast->tag = LOrExpAST::LOREXP;
    $$ = ast;
  }
  ;
Decl
  : ConstDecl {
    auto ast = new DeclAST();
    ast->tag = DeclAST::CONST;
    ast->global = 0;
    ast->const_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | VarDecl{
      auto ast = new DeclAST();
      ast->tag = DeclAST::VAR;
      ast->global = 0;
      ast->var_decl = unique_ptr<BaseAST>($1);
      $$ = ast;
  }
ConstDecl
  :CONST BType ConstDefList ';' {
    auto ast = $3;
    $$ = ast;
  }
ConstDefList
  : ConstDef ',' ConstDefList {
    auto ast = new ConstDeclAST();
    auto ast_def = (ConstDeclAST* )$3;
    ast->const_def.emplace_back($1); 
    int n = ast_def->const_def.size();
    for(int i = 0; i < n; i++) {
      ast->const_def.emplace_back(ast_def->const_def[i].release());
    }
    $$ = ast;
  }
  | ConstDef {
    auto ast = new ConstDeclAST();
    ast->const_def.emplace_back($1);
    $$ = ast;
  }
ConstDef
  : IDENT '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;
ConstInitVal
  :ConstExp {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;
BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->decl = unique_ptr<BaseAST>($1);
    ast->tag = BlockItemAST::DECL;
    $$ = ast;
  }
  | Stmt {
    auto ast = new BlockItemAST();
    ast->stmt = unique_ptr<BaseAST>($1);
    ast->tag = BlockItemAST::STMT;
    $$ = ast;
  }
  ;
LVal 
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  ;
LeVal
  : IDENT{
    auto ast = new LeValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;
BType
  : INT {
    auto ast = new BTypeAST();
    ast->tag = BTypeAST::INT;
    $$=ast;
  }
  | VOID {
    auto ast = new BTypeAST();
    ast->tag = BTypeAST::VOID;
    $$=ast;
  }
VarDecl
  : BType VarDefList ';' {
    $$ = $2;
  }
VarDefList
  : VarDef ',' VarDefList {
    auto ast = new VarDeclAST();
    auto ast_def = (VarDeclAST* )$3;
    ast->var_def.emplace_back($1); 
    int n = ast_def->var_def.size();
    for(int i = 0; i < n; i++) {
      ast->var_def.emplace_back(ast_def->var_def[i].release());
    }
    $$ = ast;
  }
  | VarDef {
    auto ast = new VarDeclAST();
    ast->var_def.emplace_back($1);
    $$ = ast;
  }
VarDef
  : IDENT {
    auto ast = new VarDefAST();
    ast->tag = VarDefAST::IDENT;
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  | IDENT '=' InitVar {
    auto ast = new VarDefAST();
    ast->tag = VarDefAST::INIT;
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
InitVar
  : Exp {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;
%%

// Error handler
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
    extern int yylineno;    
    extern char *yytext; 
    int len=strlen(yytext);
    int i;
    char buf[512]={0};
    for (i=0;i<len;++i)
    {
        sprintf(buf,"%s%d ",buf,yytext[i]);
    }

    fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, buf, yylineno);
}
