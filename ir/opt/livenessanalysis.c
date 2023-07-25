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

void printLiveness(Function *currentFunction){
    clear_visited_flag(currentFunction->entry);
    HashSet *workList = HashSetInit();
    HashSetAdd(workList,currentFunction->entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);
        //
        assert(block->visited == false);
        block->visited = true;
        printf("b%d live in: ",block->id);
        HashSetFirst(block->in);
        for(Value *liveIn = HashSetNext(block->in); liveIn != NULL; liveIn = HashSetNext(block->in)){
            printf("%s ",liveIn->name);
        }
        printf("\n");

        printf("b%d live out: ",block->id);
        HashSetFirst(block->out);
        for(Value *liveOut = HashSetNext(block->out); liveOut != NULL; liveOut = HashSetNext(block->out)){
            printf("%s ",liveOut->name);
        }
        printf("\n");
        if(block->true_block && block->true_block->visited != true){
            if(!HashSetFind(workList,block->true_block)){
                HashSetAdd(workList,block->true_block);
            }
        }
        if(block->false_block && block->false_block->visited != true){
            if(!HashSetFind(workList,block->false_block)){
                HashSetAdd(workList,block->false_block);
            }
        }
    }
}


void cleanLiveSet(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    clear_visited_flag(entry);


    InstNode *currNode = entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            HashSetFirst(block->in);
            for(void *key = HashSetNext(block->in); key != NULL; key = HashSetNext(block->in)){
                HashSetRemove(block->in,key);
            }
            HashSetFirst(block->out);
            for(void *key = HashSetNext(block->out); key != NULL; key = HashSetNext(block->out)){
                HashSetRemove(block->out,key);
            }
        }
        currNode = get_next_inst(currNode);
    }
}

void calculateLiveness1(Function *currentFunction){
    // 从exit开始
    BasicBlock *exit = currentFunction->tail;

    // 注意最后一个基本块有可能不是exit
    // 找到return
    InstNode *exitCurr = exit->tail_node;
    while(exitCurr->inst->Opcode != Return){
        exitCurr = get_prev_inst(exitCurr);
    }

    // exitCurr现在指向Return 语句
    exit = exitCurr->inst->Parent;
    InstNode *exitHead = exit->head_node;
    HashSet *exitLiveIn = exit->in;

    //因为是最后一个基本块所以我们直接
    while(exitCurr != exitHead){
        if(isValidOperator(exitCurr)){
            Value *def = NULL;
            Value *lhs = NULL;
            Value *rhs = NULL;
            //如果是return 语句的话就不需要左边这个
            //如果是CopyOperation也需要的特殊处理
            if(exitCurr->inst->Opcode == Return || exitCurr->inst->Opcode == Store || exitCurr->inst->Opcode == GIVE_PARAM){
                def = NULL;
            }else if(exitCurr->inst->Opcode == CopyOperation){
                // TODO 存在问题
                Value *insValue = ins_get_dest(exitCurr->inst);
                def = insValue->alias;
            }else{
                def = ins_get_dest(exitCurr->inst);
            }

            if(exitCurr->inst->user.value.NumUserOperands == (unsigned int)1){
                lhs = ins_get_lhs(exitCurr->inst);
            }

            if(exitCurr->inst->user.value.NumUserOperands == (unsigned int)2){
                lhs = ins_get_lhs(exitCurr->inst);
                rhs = ins_get_rhs(exitCurr->inst);
            }

            if(exitCurr->inst->Opcode == Call){
                lhs = NULL;
                rhs = NULL;
            }

            // give param的时候传递的function参数我们可以直接忽略
            if(exitCurr->inst->Opcode == GIVE_PARAM){
                rhs = NULL;
            }

            // 不是立即数且不是数组的话我们可以加入分析
            // 似乎所有的左边应该都要进入分析
            if(def != NULL){
                if(HashSetFind(exitLiveIn,def)){
                    HashSetRemove(exitLiveIn,def);
                }
            }

            if(lhs != NULL && !isImm(lhs) && !isGlobalArray(lhs) && !isGlobalVar(lhs) && !isLocalArray(lhs)){
                if(!HashSetFind(exitLiveIn,lhs)){
                    //printf("lhs add %s\n",lhs->name);
                    HashSetAdd(exitLiveIn,lhs);
                }
            }

            if(rhs != NULL && !isImm(rhs) && !isGlobalVar(rhs) && !isGlobalArray(rhs) && !isLocalArray(lhs)){
                if(!HashSetFind(exitLiveIn,rhs)){
                    //printf("rhs add %s\n",rhs->name);
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
                //printf("current  is %d\n",currNode->inst->i);

                Value *def = NULL;
                Value *lhs = NULL;
                Value *rhs = NULL;

                //如果是return 语句的话就不需要左边这个
                //不是最后的基本块也可能
                //如果是CopyOperation也需要的特殊处理
                if(currNode->inst->Opcode == Return || currNode->inst->Opcode == Store || currNode->inst->Opcode == GIVE_PARAM){
                    def = NULL;
                }else if(currNode->inst->Opcode == CopyOperation){
                    Value *insValue = ins_get_dest(currNode->inst);
                    def = insValue->alias;
                }else{
                    def = ins_get_dest(currNode->inst);
                }

                //现在是否只有可能是
                if(currNode->inst->user.value.NumUserOperands == (unsigned int)1){
                    lhs = ins_get_lhs(currNode->inst);
                }
                if(currNode->inst->user.value.NumUserOperands == (unsigned int)2){
                    lhs = ins_get_lhs(currNode->inst);
                    rhs = ins_get_rhs(currNode->inst);
                }

                if(currNode->inst->Opcode == Call){
                    lhs = NULL;
                    rhs = NULL;
                }

                if(currNode->inst->Opcode == GIVE_PARAM){
                    rhs = NULL;
                }

                // 不是立即数且不是数组的话我们可以加入分析
                if(def != NULL && !isImm(def)){
                    if(HashSetFind(tempSet,def)){
                        HashSetRemove(tempSet,def);
                    }
                }

                if(lhs != NULL && !isImm(lhs) && !isLocalArray(lhs) && !isGlobalArray(lhs) && !isGlobalVar(lhs)){
                    if(!HashSetFind(tempSet,lhs)){
                        //printf("lhs add %s\n",lhs->name);
                        HashSetAdd(tempSet,lhs);
                    }
                }

                if(rhs != NULL && !isImm(rhs) && !isLocalArray(rhs) && !isGlobalArray(rhs) && !isGlobalVar(rhs)){
                    if(!HashSetFind(tempSet,rhs)){
                        //printf("rhs add %s\n",rhs->name);
                        HashSetAdd(tempSet,rhs);
                    }
                }

                // TODO phi函数是不是别处理了啊 如果现在是phi函数的话我们特殊处理
                if(currNode->inst->Opcode == Phi){
                    HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        if(phiInfo->define != NULL){
                            HashSetAdd(tempSet, phiInfo->define);
                        }
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

// 包含phi函数的 活跃变量分析
//用作 loop invariant等地方
void calculateLiveness(Function *currentFunction){


    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    InstNode *tailNode = tail->tail_node;

    clear_visited_flag(entry);
    HashSet *allBlocks = HashSetInit();
    HashSet *list = HashSetInit();
    HashSetAdd(allBlocks, entry);
    HashSetAdd(list,entry);
    while(HashSetSize(list) != 0){
        HashSetFirst(list);
        BasicBlock *block = HashSetNext(list);
        HashSetAdd(allBlocks,block);
        HashSetRemove(list,block);
        block->visited = true;
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(list,block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(list,block->false_block);
        }
    }

    HashSetDeinit(list);



    clear_visited_flag(entry);
    //we need at least run for all the blocks once use the visited flag to achieve that purpose
    while(tailNode->inst->Opcode != Return){
        //向前查找
        tailNode = get_prev_inst(tailNode);
    }

    assert(tailNode->inst->Opcode == Return);
    //
    BasicBlock *exit = tailNode->inst->Parent;

    //开始迭代
    HashSet *workList = HashSetInit();

    HashSetAdd(workList,exit);

    bool firstTime = true;

    //for the first time we put all the blocks into the workList
    HashSetCopy(workList,allBlocks);


    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);

        bool changed = false;

        HashSet *originalOutSet = HashSetInit();

        HashSetCopyValue(originalOutSet,block->out);

        //
        if(block->true_block){
            HashSet *trueBlockLiveIn = block->true_block->in;
            HashSetCopyValue(block->out,trueBlockLiveIn);

            //live in - phiDefs(s)
            InstNode *trueCurrNode = block->true_block->head_node;
            InstNode *trueTailNode = block->true_block->tail_node;
            while(trueCurrNode != trueTailNode){
                if(trueCurrNode->inst->Opcode == Phi){
                    Value *phiDef = ins_get_dest(trueCurrNode->inst);
                    assert(phiDef->name != NULL);
                    HashSetRemove(block->out,phiDef);
                }
                trueCurrNode = get_next_inst(trueCurrNode);
            }
        }

        if(block->false_block){
            HashSet *falseBlockLiveIn = block->false_block->in;
            HashSetCopyValue(block->out,falseBlockLiveIn);

            InstNode *falseCurrNode = block->false_block->head_node;
            InstNode *falseTailNode = block->false_block->tail_node;
            while(falseCurrNode != falseTailNode){
                if(falseCurrNode->inst->Opcode == Phi){
                    Value *phiDef = ins_get_dest(falseCurrNode->inst);
                    assert(phiDef->name != NULL);
                    HashSetRemove(block->out,phiDef);
                }
                falseCurrNode = get_next_inst(falseCurrNode);
            }
        }

        //并上后继的Phi uses
        if(block->true_block){
            InstNode *trueCurrNode = block->true_block->head_node;
            InstNode *trueTailNode = block->true_block->tail_node;
            while(trueCurrNode != trueTailNode){
                if(trueCurrNode->inst->Opcode == Phi){
                    HashSet *phiSet = trueCurrNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        Value *define = phiInfo->define;
                        //TODO 这里也是自行添加的
                        if(define != NULL && !isImm(define) && phiInfo->from == block){
                            HashSetAdd(block->out,define);
                        }
                    }
                }
                trueCurrNode = get_next_inst(trueCurrNode);
            }
        }

        if(block->false_block){
            InstNode *falseCurrNode = block->false_block->head_node;
            InstNode *falseTailNode = block->false_block->tail_node;
            while(falseCurrNode != falseTailNode){
                if(falseCurrNode->inst->Opcode == Phi){
                    HashSet *phiSet = falseCurrNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        Value *define = phiInfo->define;
                        //TODO 这里是自己添加的
                        if(define != NULL && !isImm(define) && phiInfo->from == block){
                            HashSetAdd(block->out,define);
                        }
                    }
                }
                falseCurrNode = get_next_inst(falseCurrNode);
            }
        }

        if(HashSetDifferent(originalOutSet,block->out)){
            changed = true;
        }
        HashSetDeinit(originalOutSet);

        HashSet *tempSet = HashSetInit();
        HashSetCopyValue(tempSet,block->out);
        InstNode *currNode = block->tail_node;
        InstNode *headNode = block->head_node;

        while(currNode != headNode){
            if(isValidOperator(currNode)){
                Value *def = NULL;
                Value *rhs = NULL;
                Value *lhs = NULL;

                //处理def的计算
                if(currNode->inst->Opcode == Return || currNode->inst->Opcode == br || currNode->inst->Opcode == br_i1){
                    def = NULL;
                }else if(currNode->inst->Opcode == CopyOperation){
                    Value *insValue = ins_get_dest(currNode->inst);
                    def = insValue->alias;
                }else if(currNode->inst->Opcode == Phi){
                    //单独列出来但是我们
                }else{
                    def = ins_get_dest(currNode->inst);
                }

                //
                if(currNode->inst->user.value.NumUserOperands == (unsigned int)1){
                    lhs = ins_get_lhs(currNode->inst);
                }

                if(currNode->inst->user.value.NumUserOperands == (unsigned int)2){
                    lhs = ins_get_lhs(currNode->inst);
                    rhs = ins_get_rhs(currNode->inst);
                }

                //处理特殊情况
                if(currNode->inst->Opcode == Call) {
                    lhs = NULL;
                    rhs = NULL;
                }else if(currNode->inst->Opcode == GIVE_PARAM){
                    rhs = NULL;
                }else if(currNode->inst->Opcode == Phi){
                    lhs = NULL;
                    rhs = NULL;
                    def = NULL;
                    // 特别注意 ！添加的是def 进live in
                    Value *phiDef = ins_get_dest(currNode->inst);
                    HashSetAdd(tempSet,phiDef);
                }

                // 不是立即数且不是数组的话我们可以加入分析
                // 先减去def 再加上 lhs 和 rhs
                if(def != NULL && !isImm(def)){
                    if(HashSetFind(tempSet,def)){
                        HashSetRemove(tempSet,def);
                    }
                }

                if(lhs != NULL && !isImm(lhs) && !isLocalArray(lhs) && !isGlobalArray(lhs) && !isGlobalVar(lhs)){
                    if(!HashSetFind(tempSet,lhs)){
                        //printf("lhs add %s\n",lhs->name);
                        HashSetAdd(tempSet,lhs);
                    }
                }

                if(rhs != NULL && !isImm(rhs) && !isLocalArray(rhs) && !isGlobalArray(rhs) && !isGlobalVar(rhs)){
                    if(!HashSetFind(tempSet,rhs)){
                        //printf("rhs add %s\n",rhs->name);
                        HashSetAdd(tempSet,rhs);
                    }
                }
            }
            currNode = get_prev_inst(currNode);
        }

        if(HashSetDifferent(block->in,tempSet)){
            changed = true;
            HashSetClean(block->in);
            HashSetCopyValue(block->in, tempSet);
        }

        HashSetDeinit(tempSet);
//        printf("block %d live in:\n",block->id);
//        printf("function %s\n",block->Parent->name);
//        HashSetFirst(block->in);
//        for(Value *livein = HashSetNext(block->in); livein != NULL; livein = HashSetNext(block->in)){
//            printf(" %s",livein->name);
//        }
//        printf("\n");

        if(changed){
            HashSetFirst(block->preBlocks);
            for(BasicBlock *blocks = HashSetNext(block->preBlocks); blocks != NULL; blocks = HashSetNext(block->preBlocks)){
                if(blocks != exit){
                    HashSetAdd(workList,blocks);
                }
            }
        }
    }
    clear_visited_flag(entry);
}