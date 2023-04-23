//
// Created by Valerian on 2023/3/31.
//

#include "DeadCodeElimination.h"

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
            // mark as useful
            currNode->inst->isCritical = true;
            HashSetAdd(workList,currNode->inst);
        }
        currNode = get_next_inst(currNode);
    }

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        //remove i from worklist
        Instruction *instNode = HashSetNext(workList);

        HashSetRemove(workList,instNode);

        Value *insValue = ins_get_dest(instNode);

        Value *lhs = NULL;
        Value *rhs = NULL;
        Value *dest = NULL;
        if(insValue->NumUserOperands == (unsigned)1){
            lhs = ins_get_lhs(currNode->inst);
        }
        if(insValue->NumUserOperands == (unsigned int)2){
            lhs = ins_get_lhs(currNode->inst);
            rhs = ins_get_rhs(currNode->inst);
        }


        //TODO 找到def 不能是store的全局 并且不能是GiveParam的第二个value、
        //并且putint

        //  // 找到对应的instruction TODO 修改部分
        if(lhs != NULL){
            Instruction *defLhs = (Instruction*)lhs;
            assert(defLhs != NULL);
            HashSetAdd(workList,defLhs);
        }
        if(rhs != NULL){
            Instruction *defRhs = (Instruction*)rhs;
            assert(defRhs != NULL);
            HashSetAdd(workList,defRhs);
        }

        //计算每条
        BasicBlock *block = instNode->Parent;

        HashSetFirst(block->rdf);
        for(BasicBlock *rdf = HashSetNext(block->rdf); rdf != NULL; rdf = HashSetNext(block->rdf)){

            InstNode *rdfTail = rdf->tail_node;

            assert(rdfTail->inst->Opcode == br_i1);

            if(rdfTail->inst->isCritical == false){
                rdfTail->inst->isCritical = true;
                HashSetAdd()
            }
        }
    }
}

void Sweep(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    //
    InstNode *currNode = entry->head_node;
    while(currNode != tail->tail_node){
        if(currNode->inst->isCritical == false){
            if(currNode->inst->Opcode == br_i1){

            }else if(currNode->inst->Opcode == br){
                // br 不变
            }else{
                // 除了br不变的话其他的

                // TODO 解决delete_inst相关的问题
                InstNode *nextNode = get_next_inst(currNode);
                delete_inst(currNode);
                currNode = nextNode;
            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
}

bool DeadCodeElimination(Function *currentFunction) {

}


bool combineBlocks(Function *currentFunction){
    // SSA 构建后跑一次 SSA消除后跑一次


    // 如果一个Block只有phi函数和跳转语句我们就可以优化
    BasicBlock *entry = currentFunction->entry;
    //

}