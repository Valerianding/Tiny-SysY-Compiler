//
// Created by Valerian on 2023/3/31.
//

#include "DeadCodeElimination.h"

void Mark(Function *currentFunction){
    //
    HashSet *workList = HashSetInit();

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    //
    InstNode *currNode = entry->head_node;
    while(currNode != tail->tail_node){
        // clear mark
        currNode->inst->isCritical = false;

        if(isCriticalOperator(currNode)){
            // mark as useful
            currNode->inst->isCritical = true;
            HashSetAdd(workList,currNode);
        }
        currNode = get_next_inst(currNode);
    }

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        //remove i from worklist
        InstNode *instNode = HashSetNext(workList);

        HashSetRemove(workList,instNode);

        if(instNode->)
    }
    //计算每条
}

void Sweep(){

}
bool DeadCodeElimination(Function *currentFunction) {


}


bool combineBlocks(Function *currentFunction){
    // SSA 构建后跑一次 SSA消除后跑一次


    // 如果一个Block只有phi函数和跳转语句我们就可以优化
    BasicBlock *entry = currentFunction->entry;
    //

}