//
// Created by 12167 on 2023-01-07.
//
//
#ifndef C22V1_INSNODE_STACK_H
#define C22V1_INSNODE_STACK_H

#include "../ir/bblock.h"
#include <stdbool.h>

#define insnode_stack_Size 10

//目前是专为cal_expr()设计的
typedef struct{
    InstNode * ins_nodes[insnode_stack_Size];
    int top;
}insnode_stack;

void init_insnode_stack(insnode_stack *S); // 初始化栈
bool insnode_push(insnode_stack* S, InstNode * x); //x 入栈
bool insnode_pop(insnode_stack* S, InstNode **px); //出栈，元素保存到px所指的单元，函数返回true,栈为空时返回 false
bool insnode_top(insnode_stack* S, InstNode **px); //获取栈顶元素，将其保存到px所指的单元，函数返回true，栈满时返回 false
bool insnode_is_empty(insnode_stack S);  // 栈为空时返回 true，否则返回 false
void insnode_stack_new(insnode_stack *S);


#endif
