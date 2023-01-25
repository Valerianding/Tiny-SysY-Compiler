//
// Created by 12167 on 2022-11-28.
//


#include <stdbool.h>
#include "value.h"

#define MaxSize 50

#define value_stack_Size 50

//目前是专为cal_expr()设计的
typedef struct{
    Value* value_node[value_stack_Size];
    int top;
}value_stack;

void init_value_stack(value_stack *S); // 初始化栈
bool push_value(value_stack* S, Value *x); //x 入栈
bool pop_value(value_stack* S, Value **px); //出栈，元素保存到px所指的单元，函数返回true,栈为空时返回 false
bool top_value(value_stack* S, Value **px); //获取栈顶元素，将其保存到px所指的单元，函数返回true，栈满时返回 false
bool is_empty_value(value_stack S);  // 栈为空时返回 true，否则返回 false


