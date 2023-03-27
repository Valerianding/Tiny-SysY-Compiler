//
// Created by Valerian on 2023/3/19.
//

#include "livenessanalysis.h"

bool HashSetCopyValue(HashSet *dest, HashSet *src){
    bool changed = false;
    HashSetFirst(src);
    for(Value *liveVariable = HashSetNext(src); liveVariable != NULL; liveVariable = HashSetNext(src)){
        if(!HashSetFind(dest,liveVariable)){
            HashSetAdd(dest,liveVariable);
            changed = true;
        }
    }
    return changed;
}


void printLiveness(BasicBlock *block){
    block->visited = true;
    printf("b%d ", block->id);
    HashSetFirst(block->in);
    printf("live in: ");
    for(Value *liveInVariable = HashSetNext(block->in); liveInVariable != NULL; liveInVariable = HashSetNext(block->in)){
        if(liveInVariable->name != NULL){
            printf("%s ",liveInVariable->name);
        }else{
            printf("null ");
        }
    }
    printf("\n");

    printf("b%d ", block->id);
    HashSetFirst(block->out);
    printf("live out: ");
    for(Value *liveOutVariable = HashSetNext(block->out); liveOutVariable != NULL; liveOutVariable = HashSetNext(block->out)){
        if(liveOutVariable->name != NULL){
            printf("%s",liveOutVariable->name);
        }else{
            printf("null ");
        }
    }
    printf("\n");

    //递归调用printLiveness
    if(block->true_block != NULL && block->true_block->visited == false) printLiveness(block->true_block);
    if(block->false_block != NULL && block->false_block->visited == false) printLiveness(block->false_block);
}

void calculateLiveness(Function *currentFunction){
    // 从exit开始
    BasicBlock *exit = currentFunction->tail;

    //分析exit的变换
    InstNode *exitCurr = exit->tail_node;
    InstNode *exitHead = exit->head_node;


    HashSet *exitLiveIn = exit->in;

    //因为是最后一个基本块所以我们直接
    while(exitCurr != exitHead){
        if(isValidOperator(exitCurr)){
            //printf("current Opcode is %d\n",exitCurr->inst->Opcode);
            Value *def = NULL;
            Value *lhs = NULL;
            Value *rhs = NULL;
            //如果是return 语句的话就不需要左边这个
            //如果是CopyOperation也需要的特殊处理
            if(exitCurr->inst->Opcode == Return || exitCurr->inst->Opcode == Store || exitCurr->inst->Opcode == GIVE_PARAM){
                def = NULL;
            }else if(exitCurr->inst->Opcode == CopyOperation){
                Value *insValue = ins_get_value(exitCurr->inst);
                def = insValue->alias;
            }else{
                def = ins_get_value(exitCurr->inst);
            }

            //现在是否只有可能是
            if(exitCurr->inst->user.value.NumUserOperands == (unsigned int)1){
                lhs = ins_get_lhs(exitCurr->inst);
            }
            if(exitCurr->inst->user.value.NumUserOperands == (unsigned int)2){
                lhs = ins_get_lhs(exitCurr->inst);
                rhs = ins_get_rhs(exitCurr->inst);
            }

            // 不是立即数且不是数组的话我们可以加入分析
            if(def != NULL && !isImm(def) && !isArray(def)){
                if(HashSetFind(exitLiveIn,def)){
                    HashSetRemove(exitLiveIn,def);
                }
            }
            if(lhs != NULL && !isImm(lhs) && !isArray(lhs) && !isGlobalVar(lhs)){
                if(!HashSetFind(exitLiveIn,lhs)){
                    HashSetAdd(exitLiveIn,lhs);
                }
            }

            if(rhs != NULL && !isImm(rhs) && !isArray(rhs) && !isGlobalVar(rhs)){
                if(!HashSetFind(exitLiveIn,rhs)){
                    HashSetAdd(exitLiveIn,rhs);
                }
            }
        }
        exitCurr = get_prev_inst(exitCurr);
    }
    //维护一个HashSet
    HashSet *workList = HashSetInit();

    // 如果exit有前驱的话我们就加入
    HashSetFirst(exit->preBlocks);
    for(BasicBlock *prevBlock = HashSetNext(exit->preBlocks); prevBlock != NULL; prevBlock = HashSetNext(exit->preBlocks)){
        assert(prevBlock != NULL);
        HashSetAdd(workList,prevBlock);
    }

    while(HashSetSize(workList) != 0){
        // 取出一个
        HashSetFirst(workList);
        BasicBlock *removeBlock = HashSetNext(workList);
        HashSetRemove(workList, removeBlock);

        // 检查是否会更新的
        bool changed = false;
        // 去找它的true的后继

        if(removeBlock->true_block != NULL){
            HashSet *trueBlockLiveIn = removeBlock->true_block->in;
            changed |= HashSetCopyValue(removeBlock->out,trueBlockLiveIn);
        }
        if(removeBlock->false_block != NULL){
            HashSet *falseBlockLiveIn = removeBlock->false_block->in;
            changed |= HashSetCopyValue(removeBlock->out,falseBlockLiveIn);
        }


        HashSet *tempSet = HashSetInit();
        HashSetCopyValue(tempSet, removeBlock->out);
        InstNode *currNode = removeBlock->tail_node;
        //然后更新live-in
        while(currNode != removeBlock->head_node){

            if(isValidOperator(currNode)){
                //printf("current Opcode is %d\n",currNode->inst->Opcode);

                Value *def = NULL;
                Value *lhs = NULL;
                Value *rhs = NULL;

                //如果是return 语句的话就不需要左边这个
                //如果是CopyOperation也需要的特殊处理
                if(currNode->inst->Opcode == Return || currNode->inst->Opcode == Store || currNode->inst->Opcode == GIVE_PARAM){
                    def = NULL;
                }else if(currNode->inst->Opcode == CopyOperation){
                    Value *insValue = ins_get_value(currNode->inst);
                    def = insValue->alias;
                }else{
                    def = ins_get_value(currNode->inst);
                }

                //现在是否只有可能是
                if(currNode->inst->user.value.NumUserOperands == (unsigned int)1){
                    lhs = ins_get_lhs(currNode->inst);
                }
                if(currNode->inst->user.value.NumUserOperands == (unsigned int)2){
                    lhs = ins_get_lhs(currNode->inst);
                    rhs = ins_get_rhs(currNode->inst);
                }

                // 不是立即数且不是数组的话我们可以加入分析
                if(def != NULL && !isImm(def) && !isArray(def)){
                    if(HashSetFind(tempSet,def)){
                        HashSetRemove(tempSet,def);
                    }
                }
                if(lhs != NULL && !isImm(lhs) && !isArray(lhs)){
                    if(!HashSetFind(tempSet,lhs)){
                        HashSetAdd(tempSet,lhs);
                    }
                }

                if(rhs != NULL && !isImm(rhs) && !isArray(rhs)){
                    if(!HashSetFind(tempSet,rhs)){
                        HashSetAdd(tempSet,rhs);
                    }
                }
            }
            currNode = get_prev_inst(currNode);
        }
        changed |= HashSetCopyValue(removeBlock->in, tempSet);

        if(changed){
            HashSetFirst(removeBlock->preBlocks);
            for(BasicBlock *prevBlock = HashSetNext(removeBlock->preBlocks); prevBlock != NULL; prevBlock = HashSetNext(removeBlock->preBlocks)){
                HashSetAdd(workList,prevBlock);
            }
        }
    }
}