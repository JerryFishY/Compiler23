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
%token INT RETURN LE GE EQ NE AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

// Non-terminal type
%type <ast_val> FuncDef FuncType Block Stmt Exp UnaryExp  PrimaryExp AddExp MulExp LOrExp LAndExp EqExp RelExp 
%type <int_val> Number
%type <char_val> UnaryOp

%%

// Parsing rule
// Lv1-Lv2
CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT {
    auto ast = new FuncTypeAST();
    $$ = ast; 
  }
  ;

Block
  : '{' Stmt '}' {
    auto ast = new BlockAST();
    ast->stmt = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : RETURN Exp';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2); 
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
