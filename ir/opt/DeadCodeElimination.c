//
// Created by Valerian on 2023/3/31.
//

#include "DeadCodeElimination.h"

void Mark(Function *currentFunction){
    //
    HashSet *workList = HashSetInit();

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