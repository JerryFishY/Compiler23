#include "koopa.h"
#include "visit.h"
#include <cassert>
#include <iostream>

class VarAlloc{
    public:
    std::unordered_map<koopa_raw_value_t,size_t> var_offset; // To record the offset of each variable
} varalloc;


void allocstack(const koopa_raw_function_t &func,int &A){
    for(size_t i = 0; i < func->bbs.len; ++i){
        auto bb = reinterpret_cast<koopa_raw_basic_block_t>(func->bbs.buffer[i]);
        for(int j = 0; j < bb->insts.len; ++j){
            auto value = reinterpret_cast<koopa_raw_value_t>(bb->insts.buffer[j]);
            if(value->kind.tag == KOOPA_RVT_CALL){
                koopa_raw_call_t c = value->kind.data.call;
                if((int)c.args.len > 8){
                    A = A >((int)c.args.len - 8 ) * 4?A:((int)c.args.len - 8 ) * 4;
                }
            }
        }
    }
}

void parse_str(const char* str){
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(str, &program);
  assert(ret == KOOPA_EC_SUCCESS); 
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  koopa_delete_program(program);

  if(raw.values.len)
    {
        std::cout<<".data\n";
        for(size_t i=0;i<raw.values.len;++i)
        {
            koopa_raw_value_t data=(koopa_raw_value_t)raw.values.buffer[i];
            std::cout<<".globl "<<data->name+1<<"\n";
            std::cout<<data->name+1<<":\n";
            if(data->kind.data.global_alloc.init->kind.tag==KOOPA_RVT_INTEGER)
            {
                std::cout<<"   .word "<<data->kind.data.global_alloc.init->kind.data.integer.value<<"\n";
                std::cout<<"\n";
            }
        }
    }


  visit(raw);
  koopa_delete_raw_program_builder(builder);
}

void visit(const koopa_raw_program_t &program){
  //Visit all functions in it
  visit(program.funcs);
}

void visit(const koopa_raw_slice_t &slice){
  // visit all functions for the program
  for(size_t i = 0;i < slice.len ; i++){
    if(slice.kind == KOOPA_RSIK_FUNCTION){
      visit((koopa_raw_function_t)slice.buffer[i]);
    }
    else{
      // should not touch here
      assert(false);
    }
  }
}

void visit(const koopa_raw_function_t &func){
  if(func->bbs.len == 0) return;
  int A=0;
  allocstack(func,A);
  std::cout<<"   .text\n";
  std::cout<<"   .globl "<<func->name+1<<"\n";
  std::cout<<func->name+1<<":\n";
  //alloc stack for the function
  std::cout<<"  addi sp, sp, -512"<<"\n";
  std::cout<<"sw ra, 508(sp)\n";

  int soffset = A;// stack pointer offset in this function stack A+4 is reserved for the caller
  //Visit all basic blocks in it
  for(size_t i =0 ;i<func->bbs.len;i++){
    visit((koopa_raw_basic_block_t)func->bbs.buffer[i],soffset);
  }
}

void visit(const koopa_raw_basic_block_t &bb,int &soffset){
  if(bb->name){
    if(std::string(bb->name+1) != "entry"){
      std::cout<<"\n"<<bb->name+1<<":"<<"\n";
      }
  }
  //Visit all values in it
  for(size_t i =0 ;i<bb->insts.len;i++){
    visit((koopa_raw_value_t)bb->insts.buffer[i],soffset);
  }
}

void visit(const koopa_raw_value_t &value,int &soffset){
  //Visit all values in it

  if(value->kind.tag == KOOPA_RVT_RETURN){
    if(value->kind.data.ret.value == nullptr){

    }
    else if(value->kind.data.ret.value->kind.tag == KOOPA_RVT_INTEGER){
      int i = value->kind.data.integer.value;
      std::cout<<"   li "<<"a0, "<<i<<"\n";
  } else{
    int retoff = varalloc.var_offset[value->kind.data.ret.value];
      std::cout<<"   lw t0, "<<retoff<<"(sp)\n";
      std::cout<<"   add "<<"a0, x0 ,t0\n";
  }
    std::cout<<"lw ra, 508(sp)\n";
    std::cout<<"  addi sp, sp, 512"<<"\n";
    std::cout<<"   ret\n\n";
  }
  if(value->kind.tag == KOOPA_RVT_BINARY){
    int lreg,rreg;
    koopa_raw_value_t value_l= value->kind.data.binary.lhs;
    koopa_raw_value_t value_r= value->kind.data.binary.rhs;
    if(value->kind.data.binary.op == KOOPA_RBO_ADD){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   add t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_SUB){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   sub t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_MUL){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   mul t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_DIV){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   div t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_MOD){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   rem t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_EQ){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   xor t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t2, t2\n";
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_NOT_EQ){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   xor t2, ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   snez t2, t2\n";
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_LT){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   slt t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_LE){
      // less than
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   slt t2, ";
      binary_print_helper(lreg,rreg);
      // equal to
      std::cout<<"   xor t3, ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t3, t3\n";
      // or the above result
      std::cout<<"   or t2, t3, t2\n";
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_GT){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   sgt t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_GE){
      // greater than
      // less than
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   sgt t2, ";
      binary_print_helper(lreg,rreg);
      // equal to
      std::cout<<"   xor t3, ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   seqz t3, t3\n";
      // or the above result
      std::cout<<"   or t2, t3, t2\n";
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_AND){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   and t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
    else if(value->kind.data.binary.op == KOOPA_RBO_OR){
      binary_helper(value_l,value_r,lreg,rreg);
      std::cout<<"   or t2"<<", ";
      binary_print_helper(lreg,rreg);
      std::cout<<"   sw t2, "<<soffset<<"(sp)\n";
      varalloc.var_offset[value] = soffset;
      soffset+=4;
    }
  } else if(value->kind.tag == KOOPA_RVT_STORE){
    store_helper(value,soffset);
  }
  else if(value->kind.tag == KOOPA_RVT_LOAD){
    load_helper(value,soffset);
  }else if(value->kind.tag == KOOPA_RVT_BRANCH){
    branch_helper(value,soffset);
  }else if(value->kind.tag == KOOPA_RVT_JUMP){
    jump_helper(value,soffset);
  }
  else if(value->kind.tag == KOOPA_RVT_CALL){
    call_helper(value,soffset);
  }
  else{
    // should not touch here
    // assert(false);
  }
}

void binary_helper(koopa_raw_value_t l,koopa_raw_value_t r,int & lreg,int &rreg){
  if(l->kind.tag == KOOPA_RVT_INTEGER){
    int i = l->kind.data.integer.value;
    if(i == 0){
      // use x0
      lreg = -1;
    }else{
      std::cout<<"   li t0"<<", "<<l->kind.data.integer.value<<"\n";
      lreg=0;
    }
  } else{
    std::cout<<"  lw t0, "<<varalloc.var_offset[l]<<"(sp)\n";
    lreg = 0;
  }
  if(r->kind.tag == KOOPA_RVT_INTEGER){
    int i = r->kind.data.integer.value;
    if(i == 0){
      // use x0
      rreg = -1;
    }else{
      std::cout<<"   li t1"<<", "<<r->kind.data.integer.value<<"\n";
      rreg=1;
    }
  }else{
    std::cout<<"  lw t1, "<<varalloc.var_offset[r]<<"(sp)\n";
    rreg = 1;
  }
}

void store_helper(koopa_raw_value_t value,int &soffset){
    koopa_raw_store_t store_inst=value->kind.data.store;
    koopa_raw_value_t store_val=store_inst.value;
    koopa_raw_value_t store_dest=store_inst.dest;
    if(store_val->kind.tag==KOOPA_RVT_INTEGER)
    {
        std::cout<<"li t0, "<<store_val->kind.data.integer.value<<"\n";
    }else if(store_val->kind.tag==KOOPA_RVT_FUNC_ARG_REF)
    {
      koopa_raw_func_arg_ref_t arg=store_val->kind.data.func_arg_ref;
      if(arg.index < 8){
        std::cout<<"mv t0, a"<<arg.index<<"\n";
      }else{
        std::cout<<"li t5, "<<512+(arg.index-8)*4<<"\n";
        std::cout<<"add t5, t5, sp\n";
        std::cout<<"lw t0, (t5)"<<"\n";
      }
    }
    else
    {
        std::cout<<"   lw t0, "<<varalloc.var_offset[store_val]<<"(sp)"<<"\n";
    }

    // Store
    if(store_dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
      std::cout<<"la t1, "<<store_dest->name+1<<"\n";
      std::cout<<"sw t0, (t1)\n";
      return;
    }
    else if(varalloc.var_offset.find(store_dest) == varalloc.var_offset.end())
    {
        varalloc.var_offset[store_dest]=soffset;
        soffset+=4;
    }
    std::cout<<"   sw t0, "<<varalloc.var_offset[store_dest]<<"(sp)"<<"\n";
}

void load_helper(koopa_raw_value_t load,int& soffset)
{ if(load->kind.data.load.src->kind.tag==KOOPA_RVT_GLOBAL_ALLOC){
    std::cout<<"   la t0, "<<load->kind.data.load.src->name+1<<"\n";
    std::cout<<"   lw t0, 0(t0)\n";
    std::cout<<"   li t5, "<<soffset<<"\n";
    std::cout<<"   add t5, t5, sp"<<"\n";
    std::cout<<"   sw t0, (t5)"<<"\n";
    varalloc.var_offset[load]=soffset;
    soffset+=4;
  }
else{
  varalloc.var_offset[load]=varalloc.var_offset[load->kind.data.load.src];
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

void branch_helper(koopa_raw_value_t value,int &soffset){
  koopa_raw_branch_t branch_inst=value->kind.data.branch;
  std::cout<<"   li t4, "<<varalloc.var_offset[branch_inst.cond]<<"\n";
  std::cout<<"   add t4, t4, sp\n";
  std::cout<<"   lw t3, (t4)\n";
  std::cout<<"   beqz t3, "<<branch_inst.false_bb->name+1<<"helper"<<"\n";
  std::cout<<"   bnez t3, "<<branch_inst.true_bb->name+1<<"helper"<<"\n";
  std::cout<<branch_inst.false_bb->name+1<<"helper"<<":\n";
  std::cout<<"   j "<<branch_inst.false_bb->name+1<<"\n";
  std::cout<<branch_inst.true_bb->name+1<<"helper"<<":"<<"\n";
  std::cout<<"   j "<<branch_inst.true_bb->name+1<<"\n";
  std::cout<<"\n";
}

void jump_helper(koopa_raw_value_t value,int &soffset){
    koopa_raw_jump_t jump_inst=value->kind.data.jump;
    std::cout<<"   j "<<jump_inst.target->name+1<<"\n\n";
}

void call_helper(koopa_raw_value_t value,int &soffset){
  koopa_raw_function_t func=value->kind.data.call.callee;
  koopa_raw_slice_t args=value->kind.data.call.args;
  int nowsoffset=0;
  for(int i=0;i<args.len;i++)
  {
      if(varalloc.var_offset.find((koopa_raw_value_t)args.buffer[i]) != varalloc.var_offset.end())
      {
          if(i<8)
          {
              std::cout<<"li t5, "<<varalloc.var_offset[(koopa_raw_value_t)(args.buffer[i])]<<"\nadd t5, t5, sp\n";
              std::cout<<"lw a"<<i<<", (t5)"<<"\n";
          }else
          {
              std::cout<<"   li t5, "<<varalloc.var_offset[(koopa_raw_value_t)args.buffer[i]]<<"\n";
              std::cout<<"   add t5, t5, sp"<<"\n";
              std::cout<<"   lw t0, (t5)"<<"\n";
              std::cout<<"   li t5, "<<nowsoffset<<"\n";
              std::cout<<"   add t5, t5, sp"<<"\n";
              std::cout<<"   sw t0, (t5)"<<"\n";
              nowsoffset+=4;
          }
      } 
  }
  std::cout<<"   call "<<func->name+1<<"\n";
  varalloc.var_offset[value]=soffset;
  soffset+=4;
  std::cout<<"   li t5, "<<varalloc.var_offset[value]<<"\n";
  std::cout<<"   add t5, t5, sp"<<"\n";
  std::cout<<"   sw a0, (t5)"<<"\n";
}