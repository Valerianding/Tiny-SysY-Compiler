//
// Created by Valerian on 2022/12/1.
//

#ifndef C22V1_FUNCTION_H
#define C22V1_FUNCTION_H
#include "bblock.h"
struct _Function{
    BasicBlock *entry;
    BasicBlock *tail;

    HashMap *dominance;  // value: block key: Dom(block)->hashset(block)
    struct _DomNode *root;

    HashMap *loadSet;
    HashMap *storeSet;
    Function *Next;
};

void function_init(Function *this);

Function *function_create();

Function *get_next_func(Function *this);

void func_set(Function *this,BasicBlock *head,BasicBlock *tail);

InstNode *get_func_start(InstNode *cur);

void print_function_info(Function *this);
#endif //C22V1_FUNCTION_H
