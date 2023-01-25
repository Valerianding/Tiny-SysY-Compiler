#include "value_stack.h"
// 初始化栈
void init_value_stack(value_stack *S)
{
    S->top=-1;
}

bool push_value(value_stack* s, Value *x)
{
    if (s->top== MaxSize - 1)
        return false;
    else
        s->value_node[++s->top] =x;
    return true;
}

bool pop_value(value_stack* s, Value **px)
{
    if (s->top == -1)
        return false;
    else
        *px = s->value_node[s->top--];
    return true;
}

bool top_value(value_stack* S, Value **px)
{
    if (S->top == -1)
        return false;
    else
        *px=S->value_node[S->top];
    return true;
}

bool is_empty_value(value_stack S)
{
    if(S.top==-1)
        return true;
    else
        return false;
}