#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include "AST.h"
using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[]) {
  // compiler mode input -o output
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // open input file and parse it with lexer
  yyin = fopen(input, "r");
  assert(yyin);


  // call parser , parser will call lexer to decompose input file
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  freopen(output,"w",stdout);
  // dump AST
  // ast->Dump();
  ast->Koopa();

  return 0;
}
