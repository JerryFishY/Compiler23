#include "koopa.h"
#include <cassert>
#include <unordered_map>

extern int index_riscv;


void parse_str(const char* str);

void visit(const koopa_raw_program_t &program);
void visit(const koopa_raw_slice_t &slice);
void visit(const koopa_raw_function_t &func);
void visit(const koopa_raw_basic_block_t &bb);
void visit(const koopa_raw_value_t &value);

void binary_helper(koopa_raw_value_t l,koopa_raw_value_t r,int & lreg,int &rreg,int &index_sysy );
void binary_print_helper(int lreg,int rreg);