//
// Created by Valerian on 2023/4/7.
//

#include "ConstFolding.h"
void ConstFolding(Function *currentFunction){
    // runs on function

    // 我们仅仅去检查那些
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    bool changed = true;
    while(changed){
        changed = false;
        InstNode *currNode = entry->head_node;
        while(currNode != tail->tail_node){
            currNode = get_next_inst(currNode);
        }
    }
}