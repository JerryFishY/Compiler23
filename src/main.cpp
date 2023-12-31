#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include "AST.h"
#include "visit.h"
using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  FILE* koopafile; // file contains koopa IR

  // Compiler mode input -o output
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // Open input file and parse it with lexer
  yyin = fopen(input, "r");
  assert(yyin);


  // Call parser , parser will call lexer to decompose input file
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);
  // Mode choosing
  if(mode[1] == 'k'){
    freopen(output,"w",stdout);
    ast->Koopa();
    return 0;
  }else if(mode[1] == 'd'){
    ast->Dump();
    std::cout<<"\n";
    return 0;
  }

  freopen("KoopaIR.out","w",stdout);
  ast->Koopa();
  fclose(stdout);

  koopafile=fopen("KoopaIR.out","r");
  char *buf=(char *)malloc(50000000);
  fread(buf,1,50000000,koopafile);
  fclose(koopafile);

  freopen(output,"w",stdout);
  parse_str(buf);
  return 0;
}
