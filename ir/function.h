//
// Created by Valerian on 2022/12/1.
//

#ifndef C22V1_FUNCTION_H
#define C22V1_FUNCTION_H
#include "bblock.h"
#include "hash_map.h"
#include "priority_queue.h"
struct _Function{
    BasicBlock *entry;
    BasicBlock *tail;
    HashSet *nonLocals;
    struct _DomNode *root;
    HashMap *loadSet;
    HashMap *storeSet;
    HashSet *loops; // 存储loop结构
    Function *Next;
    Vector *ToPoBlocks; //拓扑遍历出来的block
    HashMap *lineScanReg; //line scan产生的寄存器分配方案，后端翻译的时候用到
    PriorityQueue *live_interval;
    char *name;
};

Function *function_create();

void func_set(Function *this,BasicBlock *head,BasicBlock *tail);

InstNode *get_func_start(InstNode *cur);

void print_function_info(Function *this);

char *getName(Function *this);
#endif //C22V1_FUNCTION_H
