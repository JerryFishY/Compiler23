#pragma once
#include <unordered_map>

extern std::unordered_map<std::string, int> const_val;

enum TYPE {CONST_INT,VAR};
extern std::unordered_map<std::string, TYPE> var_type;
enum FUNCTYPE {FUNC_VOID,FUNC_INT};
extern std::unordered_map<std::string, FUNCTYPE> func_type_dict;
extern int blk_idx;
extern int now_idx;
extern int block_tree[1000]; // Store the parent node of each block
extern int if_idx;// The index of the basic block 
extern int if_dep;// To record if there has been a return intruction, to avoid two jump instruction in one basic block
extern int now_if_dep;
extern int end_blk[1000];

extern int while_idx;
extern int while_block[1000];
extern int now_while_idx;
