//
// Created by Valerian on 2022/12/1.
//

#ifndef C22V1_FUNCTION_H
#define C22V1_FUNCTION_H
#include "bblock.h"

typedef struct _BasicBlockNode{
    BasicBlock *block;
    struct sc_list list;
}BasicBlockNode;

typedef struct _Function{
    BasicBlockNode *bb_list;
    //Function *Prev;
    Function *Next;
}Function;

BasicBlockNode *new_BasicBlockNode(BasicBlock *this);

void Function_init(Function *this);

void Function_add_block_back(Function *this,BasicBlock *block);

void Function_pop_block_back(Function *this);

BasicBlockNode *Function_get_block_back(Function *this);

void Function_add_function(Function *this,Function *next);
#endif //C22V1_FUNCTION_H
