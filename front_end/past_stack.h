//
// Created by 12167 on 2022-11-28.
//

#ifndef LABT_PAST_past_stack_H
#define LABT_PAST_past_stack_H

#include <stdbool.h>
#include "ast.h"

#define past_stack_Size 200

//目前是专为cal_expr()设计的
typedef struct{
    past past_node[past_stack_Size];
    int top;
}past_stack;

void init_past_stack(past_stack *S); // 初始化栈
bool push(past_stack* S, past x); //x 入栈
bool pop(past_stack* S, past *px); //出栈，元素保存到px所指的单元，函数返回true,栈为空时返回 false
bool top(past_stack* S, past *px); //获取栈顶元素，将其保存到px所指的单元，函数返回true，栈满时返回 false
bool is_empty(past_stack S);  // 栈为空时返回 true，否则返回 false


#endif
