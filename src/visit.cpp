#include "koopa.h"
#include "visit.h"
#include <cassert>
#include <iostream>

class VarAlloc{
    public:
    std::unordered_map<koopa_raw_value_t,size_t> var_map;
} varalloc;

int index_riscv = 0;


void parse_str(const char* str){
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  assert(ret == KOOPA_EC_SUCCESS); 
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  koopa_delete_program(program);
  std::cout<<"   .text\n";

  visit(raw);
  koopa_delete_raw_program_builder(builder);
}

void visit(const koopa_raw_program_t &program){
  //Visit all functions in it
  visit(program.funcs);
}

void visit(const koopa_raw_slice_t &slice){
  for(size_t i = 0;i < slice.len ; i++){
    if(slice.kind == KOOPA_RSIK_FUNCTION){
      visit((koopa_raw_function_t)slice.buffer[i]);
    }
    if(slice.kind == KOOPA_RSIK_BASIC_BLOCK){
      visit((koopa_raw_basic_block_t)slice.buffer[i]);
    }
    if(slice.kind == KOOPA_RSIK_VALUE){
      visit((koopa_raw_value_t)slice.buffer[i]);
    }
  }
}

void visit(const koopa_raw_function_t &func){
  std::cout<<"   .globl "<<func->name+1<<"\n";
  std::cout<<func->name+1<<":\n";
  //Visit all basic blocks in it
  for(size_t i =0 ;i<func->bbs.len;i++){
    visit((koopa_raw_basic_block_t)func->bbs.buffer[i]);
  }
}

void visit(const koopa_raw_basic_block_t &bb){
  //Visit all values in it
  for(size_t i =0 ;i<bb->insts.len;i++){
    visit((koopa_raw_value_t)bb->insts.buffer[i]);
  }
}

void visit(const koopa_raw_value_t &value){
  //Visit all values in it
  if(value->kind.tag == KOOPA_RVT_RETURN){
    if(value->kind.data.ret.value->kind.tag == KOOPA_RVT_INTEGER){
      int i = value->kind.data.integer.value;
      std::cout<<"   li "<<"a0, "<<i<<"\n";
  } else{
    int i = varalloc.var_map[value->kind.data.ret.value];
      std::cout<<"   addi "<<"a0, t"<<i<<", 0\n";
  }
    std::cout<<"   ret\n";
  }
  if(value->kind.tag == KOOPA_RVT_BINARY){
    int lreg,rreg;
    koopa_raw_value_t value_l= value->kind.data.binary.lhs;
    koopa_raw_value_t value_r= value->kind.data.binary.rhs;
    if(value->kind.data.binary.op == KOOPA_RBO_ADD){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   add t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_SUB){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   sub t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_MUL){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   mul t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_DIV){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   div t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_MOD){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   rem t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_EQ){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   xor t"<<index_riscv<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t"<<index_riscv<<", t"<<index_riscv<<"\n";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_NOT_EQ){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   xor t"<<index_riscv<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   snez t"<<index_riscv<<", t"<<index_riscv<<"\n";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_LT){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   slt t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_LE){
      // less than
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   slt t"<<index_riscv<<", ";
      index_riscv++;
      binary_print_helper(lreg,rreg);
      // equal to
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   xor t"<<index_riscv<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t"<<index_riscv<<", t"<<index_riscv<<"\n";
      // or the above result
      index_riscv++;
      std::cout<<"   or t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      binary_print_helper(index_riscv-2,index_riscv-1);
      
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_GT){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   sgt t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_GE){
      // greater than
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   sgt t"<<index_riscv<<", ";
      index_riscv++;
      binary_print_helper(lreg,rreg);
      // equal to
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   xor t"<<index_riscv<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t"<<index_riscv<<", t"<<index_riscv<<"\n";
      // or the above result
      index_riscv++;
      std::cout<<"   or t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      binary_print_helper(index_riscv-2,index_riscv-1);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_AND){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   and t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_OR){
      binary_helper(value_l,value_r,lreg,rreg,index_riscv);
      std::cout<<"   or t"<<index_riscv<<", ";
      varalloc.var_map[value] = index_riscv;
      index_riscv++;
      binary_print_helper(lreg,rreg);
    }
}
}

void binary_helper(koopa_raw_value_t l,koopa_raw_value_t r,int & lreg,int &rreg,int &index_riscv ){
  if(l->kind.tag == KOOPA_RVT_INTEGER){
    int i = l->kind.data.integer.value;
    if(i == 0){
      // use x0
      lreg = -1;
    }else{
      std::cout<<"   li t"<<index_riscv<<", "<<l->kind.data.integer.value<<"\n";
      lreg=index_riscv;
      index_riscv++;
    }
  } else{
    lreg = varalloc.var_map[l];
  }
  if(r->kind.tag == KOOPA_RVT_INTEGER){
    int i = r->kind.data.integer.value;
    if(i == 0){
      // use x0
      rreg = -1;
    }else{
      std::cout<<"   li t"<<index_riscv<<", "<<r->kind.data.integer.value<<"\n";
      rreg=index_riscv;
      index_riscv++;
    }
  }else{
    rreg = varalloc.var_map[r];
  }
}

void binary_print_helper(int lreg,int rreg){
  if(lreg == -1){
    std::cout<<"x0";
  }else{
    std::cout<<"t"<<lreg;
  }
  if(rreg == -1){
    std::cout<<", x0\n";
  }else{
    std::cout<<", t"<<rreg<<"\n";
  }
}