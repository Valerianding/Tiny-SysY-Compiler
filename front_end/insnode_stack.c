#include "insnode_stack.h"
// 初始化栈
void init_insnode_stack(insnode_stack *S) {
    S->top = -1;
}

bool insnode_push(insnode_stack* s, InstNode *x)
{
    if (s->top== insnode_stack_Size - 1)
        return false;
    else
    {
        s->top++;
        s->ins_nodes[s->top] =x;
    }
    return true;
}

bool insnode_pop(insnode_stack* s, InstNode **px)
{
    if (s->top == -1)
        return false;
    else
        *px = s->ins_nodes[s->top--];
    return true;
}

bool insnode_top(insnode_stack* S, InstNode **px)
{
    if (S->top == -1)
        return false;
    else
        *px=S->ins_nodes[S->top];
    return true;
}

bool insnode_is_empty(insnode_stack S)
{
    if(S.top==-1)
        return true;
    else
        return false;
}

void insnode_stack_new(insnode_stack *S)
{
    for(int i=0;i<10;i++)
        S->ins_nodes[i]=0;
    S->top=-1;
}