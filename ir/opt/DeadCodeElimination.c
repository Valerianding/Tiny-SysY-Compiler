//
// Created by Valerian on 2023/3/31.
//

#include "DeadCodeElimination.h"
BasicBlock *findNearestMarkedPostDominator(PostDomNode *postDomNode){
    bool marked = false;
    BasicBlock *parent = postDomNode->parent;
    InstNode *currNode = parent->head_node;
    while(currNode != parent->tail_node){
        if(currNode->inst->isCritical == true){
            return parent;
        }
        currNode = get_next_inst(currNode);
    }
    return findNearestMarkedPostDominator(parent->postDomNode);
}

void Mark(Function *currentFunction){
    //
    HashSet *workList = HashSetInit();   //放的是instruction *类型

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    //
    InstNode *currNode = entry->head_node;
    while(currNode != tail->tail_node){
        // clear mark
        currNode->inst->isCritical = false;

        if(isCriticalOperator(currNode)){
            printf("first add critical %d\n",currNode->inst->i);
            // mark as useful
            currNode->inst->isCritical = true;
            HashSetAdd(workList,currNode->inst);
        }
        currNode = get_next_inst(currNode);
    }

    printf("after frist!\n");

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        //remove i from worklist
        Instruction *instNode = HashSetNext(workList);

        printf("current at %d\n",instNode->i);
        HashSetRemove(workList,instNode);

        Value *insValue = ins_get_dest(instNode);

        Value *lhs = NULL;
        Value *rhs = NULL;
        if(insValue->NumUserOperands == (unsigned)1){
            lhs = ins_get_lhs(instNode);
        }
        if(insValue->NumUserOperands == (unsigned int)2){

            lhs = ins_get_lhs(instNode);
            rhs = ins_get_rhs(instNode);
        }


        //TODO 找到def 不能是store的全局 并且不能是GiveParam的第二个value、
        //并且putint


        if(instNode->Opcode == Call){
            lhs = NULL;
            rhs = NULL;
        }

        if(instNode->Opcode == GIVE_PARAM){
            rhs = NULL;
        }

        //  // 找到对应的instruction TODO 修改部分
        if(lhs != NULL && !isImm(lhs)){
            printf("lhs : %s\n ",lhs->name);
            Instruction *defLhs = (Instruction*)lhs;
            assert(defLhs != NULL);
            if(defLhs->isCritical == false){
                defLhs->isCritical = true;
                HashSetAdd(workList,defLhs);
            }
        }

        if(rhs != NULL && !isImm(rhs)){
            printf("rhs : %s\n ",rhs->name);
            Instruction *defRhs = (Instruction*)rhs;
            assert(defRhs != NULL);
            if(defRhs->isCritical == false){
                defRhs->isCritical = true;
                HashSetAdd(workList,defRhs);
            }
        }

        // 如果是phi函数的话还需要special care about this
        if(instNode->Opcode == Phi){
            //如果是phi的话还需要特殊处理
            HashSetFirst(insValue->pdata->pairSet);
            for(pair *phiInfo = HashSetNext(insValue->pdata->pairSet); phiInfo != NULL; phiInfo = HashSetNext(insValue->pdata->pairSet)){
                Value* src = phiInfo->define;
                // 有
                if(!isImm(src)) {
                    //不是
                    Instruction *ins = (Instruction*)src;
                    if(ins->isCritical == false){
                        ins->isCritical = true;
                        //添加进workList
                        HashSetAdd(workList,ins);
                    }
                }
            }
        }

        //除了br之外的每一条语句我们都需要去找reverseBlock
        if(instNode->Opcode != br){
            //计算每条
            BasicBlock *block = instNode->Parent;

            printf("block : %d\n",block->id);
            HashSetFirst(block->rdf);
            for(BasicBlock *rdf = HashSetNext(block->rdf); rdf != NULL; rdf = HashSetNext(block->rdf)){

                InstNode *rdfTail = rdf->tail_node;

                assert(rdfTail->inst->Opcode == br_i1);

                if(rdfTail->inst->isCritical == false){
                    rdfTail->inst->isCritical = true;
                    HashSetAdd(workList,rdfTail->inst);
                }
            }
        }
    }
}

void Sweep(Function *currentFunction) {
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    //
    InstNode *currNode = entry->head_node;
    while (currNode != tail->tail_node) {
        if (currNode->inst->isCritical == false) {
            if (currNode->inst->Opcode == br_i1) {
                //rewrite i with a jump to i's nearest marked postDominator
                BasicBlock *block = currNode->inst->Parent;

                // 找到它的post Dominator
                PostDomNode *postDomNode = block->postDomNode;

                BasicBlock *markedPostDominator = findNearestMarkedPostDominator(postDomNode);

                //修改它的后继节点
                block->true_block = markedPostDominator;
                //

                // TODO 修改marked postDominator的前驱节点  暂时不修改后面需要的时候重新计算


                // rewrite this branch with a jump instruction
                InstNode *branchNode = block->tail_node;

                //山区
            } else if (currNode->inst->Opcode == br) {
                // br 不变
            } else {
                // 除了br不变的话其他的

                // TODO 解决delete_inst相关的问题
                InstNode *nextNode = get_next_inst(currNode);
                delete_inst(currNode);
                currNode = nextNode;
            }
        } else {
            currNode = get_next_inst(currNode);
        }
    }
}


void Clean(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    bool changed = true;
    while(changed){
        changed = false;
        // compute postorder
        Queue *postQueue = QueueInit();
        //
        QueuePush(postQueue,entry);
        changed = OnePass(postQueue);
        QueueDeinit(postQueue);
    }
}

bool OnePass(Queue* postQueue){
    //for each block i in postorder
    BasicBlock *block = NULL;
    QueueFront(postQueue,(void *)&block);
    assert(block != NULL);

}