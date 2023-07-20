//
// Created by 12167 on 2023-05-10.
//

#ifndef C22V1_INLINER_H
#define C22V1_INLINER_H

#define Instr_cost 5
#define min(a, b) ((a) < (b) ? (a) : (b))

#include "instruction.h"
#include "bblock.h"
#include "value_stack.h"
#include "past_stack.h"
#include "insnode_stack.h"
#include "symtab.h"
#include <stdio.h>
#include <stdbool.h>
#include "mem2reg.h"
#include "travel.h"

typedef struct Callsite_ {
    Value *caller;
    Value *callee;
}Callsite;


//内联结果，0代表内联失败，1代表基本块无变化，2代表有新增基本块
int func_inline(struct _InstNode* instruction_node, int threshold);
InstNode *find_func_begin(struct _InstNode* instruction_node,char* func_name);
void label_func_inline(struct _InstNode* instNode_list);
int get_name_index(Value* v);

//将phi的信息补充完整
void reduce_phi(HashMap* phi_map,HashMap* alias_map,HashMap* block_map, int param);
//连接加入后的基本块
void connect_caller_block(HashMap* block_map, HashSet* callee_block_set, BasicBlock* caller_cur_block, Function* callee_func, BasicBlock* last_cur_new_block);
bool is_particular_instr(Opcode opcode);
bool find_one_callsite(Value* caller, Value* callee);
void remove_one_callsite(Value* caller, Value* callee);
void label_func_inline_llvm(struct _InstNode* instNode_list, int threshold);

#endif //C22V1_INLINER_H
