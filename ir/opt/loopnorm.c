//
// Created by Valerian on 2023/7/30.
//

#include "loopnorm.h"
//create a empty preheader for each loop

void dfsTravelLoop(Loop *loop){
    printf("in loop %d\n",loop->head->id);
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        dfsTravelLoop(child);
    }
    BasicBlock *loopEntry = loop->head;
    unsigned n = HashSetSize(loopEntry->preBlocks);

    BasicBlock *guard = NULL;
    HashSetFirst(loopEntry->preBlocks);
    for(BasicBlock *preBlock = HashSetNext(loopEntry->preBlocks); preBlock != NULL; preBlock = HashSetNext(loopEntry->preBlocks)){
        if(HashSetFind(loop->loopBody,preBlock)){
            n = n - 1;
        }else{
            printf("n %d\n",preBlock->id);
            guard = preBlock;
        }
    }
    printf("now %d\n",loopEntry->id);

    //must have only one pred which will be our
    assert(n == 1);
    assert(guard != NULL);
    BasicBlock *preHeader = bb_create();
    if(guard->true_block == loopEntry){
        guard->true_block = preHeader;
    }else{
        guard->false_block = preHeader;
    }


    preHeader->true_block = loopEntry;

    HashSetRemove(loopEntry->preBlocks,guard);
    HashSetAdd(loopEntry->preBlocks,preHeader);

    HashSetAdd(preHeader->preBlocks,guard);
    InstNode *prevTail = get_prev_inst(loopEntry->head_node);
    InstNode *prevNext = loopEntry->head_node;

    // 创建两个语句
    Instruction *newBlockLabel = ins_new_zero_operator(Label);
    Instruction *newBlockBr = ins_new_zero_operator(br);
    InstNode *newBlockLabelNode = new_inst_node(newBlockLabel);
    InstNode *newBlockBrNode = new_inst_node(newBlockBr);

    // 进行一些链接
    ins_insert_after(newBlockLabelNode,prevTail);
    ins_insert_after(newBlockBrNode,newBlockLabelNode);

    newBlockBrNode->list.next = &prevNext->list;
    prevNext->list.prev = &newBlockBrNode->list;

    bb_set_block(preHeader,newBlockLabelNode,newBlockBrNode);
    preHeader->Parent = loopEntry->Parent;

    //还需要修改来自guard的phi -> 来自preHeader的phi
    InstNode *entryHead = loopEntry->head_node;
    InstNode *entryTail = loopEntry->tail_node;
    while(entryHead != entryTail){
        if(entryHead->inst->Opcode == Phi){
            HashSet *phiSet = entryHead->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                if(phiInfo->from == guard){
                    phiInfo->from = preHeader;
                }
            }
        }
        entryHead = get_next_inst(entryHead);
    }
}


//requires dominance & loop info before
//requires recompute dominance & loop info after
void LoopNormalize(Function *currentFunction){
    HashSet *loops = currentFunction->loops;
    HashSetFirst(loops);
    for(Loop *loop = HashSetNext(loops); loop != NULL; loop = HashSetNext(loops)){
        dfsTravelLoop(loop);
    }
    renameVariables(currentFunction);
}