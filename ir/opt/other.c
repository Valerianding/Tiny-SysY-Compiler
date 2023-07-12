//
// Created by Valerian on 2023/5/25.
//


#include "other.h"

//meaning less calculation
//1. function return value is void
//2. the return value is not used


//but we should make sure no memory operation and in / out put is needed
bool removeUseless(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;

    //找到function的value
    InstNode *funcHead = entry->head_node;
    Value *function = funcHead->inst->user.use_list[0].Val;

    //
    assert(function != NULL);

    //判断前提条件

}