#include "koopa.h"
#include "visit.h"
#include <cassert>
#include <iostream>

void parse_str(const char* str){
koopa_program_t program;
koopa_error_code_t ret = koopa_parse_from_string(str, &program);
assert(ret == KOOPA_EC_SUCCESS); 
koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
koopa_delete_program(program);
std::cout<<"   .text\n";

// Deal with raw program "raw"

for (size_t i = 0; i < raw.funcs.len; ++i) {
  assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
  koopa_raw_function_t func = (koopa_raw_function_t) raw.funcs.buffer[i];

  std::cout<<"   .globl "<<func->name+1<<"\n";
  std::cout<<func->name+1<<":\n";

  // Deal with raw function
  for (size_t j = 0; j < func->bbs.len; ++j) {
    assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
    koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t) func->bbs.buffer[j];
    for (size_t k = 0; k < bb->insts.len; ++k){
      koopa_raw_value_t value = (koopa_raw_value_t) bb->insts.buffer[k];

      assert(value->kind.tag == KOOPA_RVT_RETURN);
      koopa_raw_value_t ret_value = value->kind.data.ret.value;
      assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
      int32_t int_val = ret_value->kind.data.integer.value;
      std::cout << "   li "<<"a0 , "<<int_val<<"\n";
      std::cout << "   ret\n";
    }
    }  
}
koopa_delete_raw_program_builder(builder);
}