#include "koopa.h"
#include <cassert>
#include <unordered_map>


void parse_str(const char* str);

void visit(const koopa_raw_program_t &program);
void visit(const koopa_raw_slice_t &slice);
void visit(const koopa_raw_function_t &func);
void visit(const koopa_raw_basic_block_t &bb,int &soffset);
void visit(const koopa_raw_value_t &value,int &soffset);

void binary_helper(koopa_raw_value_t l,koopa_raw_value_t r,int & lreg,int &rreg);
void binary_print_helper(int lreg,int rreg);
void store_helper(koopa_raw_value_t value,int &soffset);
void load_helper(koopa_raw_value_t load,int& soffset);
void branch_helper(koopa_raw_value_t value,int &soffset);
void jump_helper(koopa_raw_value_t value,int &soffset);
void call_helper(koopa_raw_value_t value,int &soffset);