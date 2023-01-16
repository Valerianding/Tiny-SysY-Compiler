//
// Created by Valerian on 2022/12/1.
//

#ifndef C22V1_FUNCTION_H
#define C22V1_FUNCTION_H
#include "bblock.h"


typedef struct _Function{
    BasicBlock *head;
    BasicBlock *tail;
    Function *Next;
}Function;

void function_init(Function *this);

Function *function_create();

Function *get_next_func(Function *this);

void func_set(Function *this,BasicBlock *head,BasicBlock *tail);
#endif //C22V1_FUNCTION_H
