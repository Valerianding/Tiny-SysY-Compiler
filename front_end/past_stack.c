#include "past_stack.h"
// 初始化栈
void init_past_stack(past_stack *S)
{
    S->top=-1;
}

bool push(past_stack* s, past x)
{
    if (s->top== past_stack_Size - 1)
        return false;
    else
        s->past_node[++s->top] =x;
    return true;
}

bool pop(past_stack* s, past *px)
{
    if (s->top == -1)
        return false;
    else
        *px = s->past_node[s->top--];
    return true;
}

bool top(past_stack* S, past *px)
{
    if (S->top == -1)
        return false;
    else
        *px=S->past_node[S->top];
    return true;
}

bool is_empty(past_stack S)
{
    if(S.top==-1)
        return true;
    else
        return false;
}