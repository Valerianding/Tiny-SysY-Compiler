//
// Created by Valerian on 2023/4/18.
//
//
#include "Loop.h"
void HashSetCopyBlock(HashSet *dest, HashSet *src){
    HashSetFirst(src);
    BasicBlock* block = NULL;
    for(block = HashSetNext(src); block != NULL; block = HashSetNext(src)){
        HashSetAdd(dest,block);
    }
}

void loop(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;

    //showBlockInfo(instruction_list);
    // 寻找循环体结构
    HashSet *workList = HashSetInit();  // 先把所有的基本块放在这里面 bfs
    clear_visited_flag(entry);

    //bfs
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        //
        HashSetRemove(workList,block);
        //block 我们是一定没有visited
        block->visited = true;
        // 加入allBlocks
        //
        HashSet *dom = block->dom;
        if(block->true_block){
            if(block->true_block->visited){
                //回边
                BasicBlock *head = block->true_block;
                // 必然是回边 不是回边我吃屎
                if(HashSetFind(dom,head)){
                    // 寻找这个循环的结构体
                    findbody(block->true_block, block);
                }
            }else{
                HashSetAdd(workList,block->true_block);
            }
        }
        if(block->false_block){
            if(block->false_block->visited){
                BasicBlock *head = block->false_block;
                if(HashSetFind(dom,head)){
                    findbody(block->false_block, block);
                }
            }else{
                HashSetAdd(workList,block->false_block);
            }
        }
    }


    renameVariabels(currentFunction);
}

void findbody(BasicBlock *head,BasicBlock *tail){
    stack *workStack = stackInit();
    HashSet *loop = HashSetInit();
    // 将head 和 tail放进去
    HashSetAdd(loop, tail);
    HashSetAdd(loop,head);


    //现将tail 放进去的
    stackPush(workStack,tail);
    while(stackSize(workStack) != 0){
        BasicBlock *block = NULL;
        stackTop(workStack, (void *)&block);
        stackPop(workStack);
        assert(block != NULL);
        HashSetFirst(block->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
            if(!HashSetFind(loop,preBlock)){
                HashSetAdd(loop,preBlock);
                stackPush(workStack,preBlock);
            }
        }
    }

    // 打印看看循环找的对不对
    HashSetFirst(loop);
    printf("loop : ");
    for(BasicBlock *block = HashSetNext(loop); block != NULL; block = HashSetNext(loop)){
        printf("b%d",block->id);
    }
    printf("\n");


    printf("loopInvariant begins!\n");
    loopVariant(loop,head);
    printf("loopInvariant ends!\n ");
}


void loopVariant(HashSet *loop, BasicBlock *head){
    //
    HashSet *def = HashSetInit();
    // 计算循环内def
    HashSetFirst(loop);
    for(BasicBlock *block = HashSetNext(loop); block != NULL; block = HashSetNext(loop)){
        printf("block %d!\n",block->id);
        InstNode *currNode = block->head_node;
        while(currNode != block->tail_node){
            if(!hasNoDestOperator(currNode)){
                Value *insValue = ins_get_dest(currNode->inst);
                printf("def %s",insValue->name);
                HashSetAdd(def,insValue);
            }
            currNode = get_next_inst(currNode);
        }
    }

    // 打印看看
    printf("def : ");
    HashSetFirst(def);
    for(Value *defValue = HashSetNext(def); defValue != NULL; defValue = HashSetNext(def)){
        printf("%s ",defValue->name);
    }
    printf("\n");

    HashSet *loopInvariantVariable = HashSetInit();
    //def 全部知道了那我们就开始迭代吧
    bool changed = true;
    while(changed){
        changed = false;
        HashSetFirst(loop);
        BasicBlock *block = NULL;
        for(block = HashSetNext(loop);block != NULL; block = HashSetNext(loop)){
            // 我们需要
            InstNode *currNode = block->head_node;
            while(currNode != block->tail_node){
                // TODO 解决所有Operator的情况 请仔细思考
                if(isCalculationOperator(currNode)){
                    Value *lhs = ins_get_lhs(currNode->inst);
                    Value *rhs = ins_get_rhs(currNode->inst);
                    Value *dest = ins_get_dest(currNode->inst);
                    if(!HashSetFind(def,lhs) && !HashSetFind(def,rhs)){
                        if(!HashSetFind(loopInvariantVariable,dest)){
                            HashSetAdd(loopInvariantVariable,dest);
                            changed = true;
                        }
                    }
                }
                currNode = get_next_inst(currNode);
            }
        }
    }

    printf("find body!\n");



    // 所有从循环外到这个基本块的都引导到这个基本块上去
    HashSet *newBlockPrev = HashSetInit();
    HashSetFirst(head->preBlocks);
    for(BasicBlock *preBlock = HashSetNext(head->preBlocks); preBlock != NULL; preBlock = HashSetNext(head->preBlocks)){
        //看它是不是在我们的loop里面
        if(!HashSetFind(loop,preBlock)){
            //如果不在我们就要用一个新的block去装它了
            HashSetAdd(newBlockPrev,preBlock);
        }
    }

    BasicBlock *newPrevBlock = NULL;
    //需要判断是否需要新的基本块
    //如果除开循环前驱大于1或者没有前驱
    if(HashSetSize(newBlockPrev) > 1 || HashSetSize(newBlockPrev) == 0){
       newPrevBlock = newBlock(newBlockPrev,head);
    }else{
        //找到唯一不属于循环的前驱节点
        assert(HashSetSize(newBlockPrev) == 1);
        HashSetFirst(newBlockPrev);
        newPrevBlock = HashSetNext(newBlockPrev);
    }

    printf("here !\n");
    HashSet *exit = HashSetInit();
    //找到循环的出口语句
    //遍历一遍loop就行了
    HashSetFirst(loop);

    //已经找到了loop 如何找到出口呢？ -》 所有loop里面后继不在loop里面的都是的
    BasicBlock *loopBlock = NULL;
    for(loopBlock = HashSetNext(loop); loopBlock != NULL; loopBlock = HashSetNext(loop)){
        printf("loop %d",loopBlock->id);
        printf("true block %d ",loopBlock->true_block->id);
        if(loopBlock->true_block && !HashSetFind(loop,loopBlock->true_block)){
            // 放在exit里面
            printf("true block : %d",loopBlock->true_block->id);

            HashSetAdd(exit,loopBlock->true_block);
        }
        if(loopBlock->false_block && !HashSetFind(loop,loopBlock->false_block)){
            printf("false block : %d",loopBlock->false_block->id);
            // 放在exit里面
            HashSetAdd(exit,loopBlock->false_block);
        }
    }

    // 必须有一个出口
    assert(HashSetSize(exit) >= 1);

    HashSetFirst(loopInvariantVariable);
    for(Value *defValue = HashSetNext(loopInvariantVariable); defValue != NULL; defValue = HashSetNext(loopInvariantVariable)){

        printf("loopInvariant Variable %s\n",defValue->name);
        //强制转换成InstNode类型
        Instruction *instNode = (Instruction*)defValue;
        BasicBlock *defineBlock = instNode->Parent;
        bool moveAble = true;


        //必要条件 def dominates all uses
        // 统计use
        HashSetFirst(loop);
        for(BasicBlock *useBlock = HashSetNext(loop); useBlock != NULL; useBlock = HashSetNext(loop)){
            InstNode *currNode = useBlock->head_node;
            while(currNode != useBlock->tail_node){
                if(currNode->inst->Opcode == Phi){
                    // 去判断phi里面有没有使用
                    HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        Value *define = phiInfo->define;
                        BasicBlock *from = phiInfo->from;
                        if(define == defValue){
                            //根据phi的语意 考虑是其from基本块的使用

                            // 好像一定是支配的吧 如果不支配就不对了反而
                        }
                    }
                }else{
                    // 除了phi其他的情况我们都需要对其统计use
                    Value *insValue = ins_get_dest(currNode->inst);
                    Value *lhs = NULL;
                    Value *rhs = NULL;
                    if(insValue->NumUserOperands == (unsigned int)1){
                      lhs = ins_get_lhs(currNode->inst);
                    }else if(insValue->NumUserOperands == (unsigned int)2){
                       lhs = ins_get_lhs(currNode->inst);
                       rhs = ins_get_rhs(currNode->inst);
                    }

                    // 如果存在use
                    if(lhs != NULL && lhs == defValue && !HashSetFind(useBlock->dom,defineBlock)){
                        //不能move
                        printf("cant move!\n");
                        moveAble = false;
                    }

                    // TODO 因为自己dom自己所以不用另外判断
                    if(rhs != NULL && rhs == defValue && !HashSetFind(useBlock->dom,defineBlock)){
                        printf("cant move!\n");
                        moveAble = false;
                    }
                }
                currNode = get_next_inst(currNode);
            }
        }


        bool choose1 = true;
        bool choose2 = true;

        // dominates all exits
        HashSetFirst(exit);
        for(BasicBlock *exitBlock = HashSetNext(exit); exitBlock != NULL; exitBlock = HashSetNext(exit)){
            if(!HashSetFind(exitBlock->dom,defineBlock)){
                choose1 = false;
            }
            HashSet *liveIn = exitBlock->in;
            //如果live-in里面没有找到这个的话说明OK
            if(HashSetFind(liveIn,defValue)){
                choose2 = false;
            }
        }

        moveAble &= (choose1 | choose2);

        if(moveAble){
            printf("move %s",defValue->name);

            //找到InstNode
            InstNode *Node = findNode(defineBlock,instNode);
            Node->inst->Parent = newPrevBlock;
            // remove from
            removeIns(Node);
            // 放到前驱基本块的前面
            InstNode *loopPrevTail = newPrevBlock->tail_node;
            ins_insert_before(Node,loopPrevTail);
        }else{
            printf("cant move!\n");
            // 如果不是move able我们从loopInvariant里面去掉然后重新计算
            HashSetRemove(loopInvariantVariable,defValue);
        }
    }




    HashSet *extraVariable = HashSetInit();
    //现在loopInvariant里面的变量已经是提取到了循环外的了 我们再次迭代寻找定值
    bool extraChanged = true;
    while(extraChanged){
        extraChanged= false;
        HashSetFirst(loop);
        BasicBlock *block = NULL;
        for(block = HashSetNext(loop);block != NULL; block = HashSetNext(loop)){
            // 我们需要
            InstNode *currNode = block->head_node;
            while(currNode != block->tail_node){
                // TODO 解决所有Operator的情况 请仔细思考
                if((currNode)){
                    Value *lhs = ins_get_lhs(currNode->inst);
                    Value *rhs = ins_get_rhs(currNode->inst);
                    Value *dest = ins_get_dest(currNode->inst);
                    //
                    if(HashSetFind(loopInvariantVariable,lhs) && HashSetFind(loopInvariantVariable,rhs)){
                        HashSetAdd(extraVariable,dest);
                    }else if(!HashSetFind(def,lhs) && HashSetFind(loopInvariantVariable,rhs)){
                        HashSetAdd(extraVariable,dest);
                    }else if(HashSetFind(loopInvariantVariable,lhs) && !HashSetFind(def,rhs)){
                        HashSetAdd(extraVariable,dest);
                    }
                }
                currNode = get_next_inst(currNode);
            }
        }
    }

    // 不能用while 我们for循环只循环一次就行
    //对extraVariable再跑一次
    HashSetFirst(extraVariable);
    for(Value *defValue = HashSetNext(extraVariable); defValue != NULL; defValue = HashSetNext(extraVariable)){
        Instruction *instNode = (Instruction*)defValue;
        BasicBlock *defineBlock = instNode->Parent;
        bool moveAble = true;
        //def dominates all uses
        // 统计use
        HashSetFirst(loop);
        for(BasicBlock *useBlock = HashSetNext(loop); useBlock != NULL; useBlock = HashSetNext(loop)){
            InstNode *currNode = useBlock->head_node;
            while(currNode != useBlock->tail_node){
                if(currNode->inst->Opcode == Phi){
                    // 去判断phi里面有没有使用
                    HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        Value *define = phiInfo->define;
                        BasicBlock *from = phiInfo->from;
                        if(define == defValue){
                            //根据phi的语意 考虑是其from基本块的使用

                            // 好像一定是支配的吧 如果不支配就不对了反而
                        }
                    }
                }else{
                    // 除了phi其他的情况我们都需要对其统计use
                    Value *insValue = ins_get_dest(currNode->inst);
                    Value *lhs = NULL;
                    Value *rhs = NULL;
                    if(insValue->NumUserOperands == (unsigned int)1){
                        lhs = ins_get_lhs(currNode->inst);
                    }else if(insValue->NumUserOperands == (unsigned int)2){
                        lhs = ins_get_lhs(currNode->inst);
                        rhs = ins_get_rhs(currNode->inst);
                    }

                    // 如果存在use
                    if(lhs != NULL && lhs == defValue && !HashSetFind(useBlock->dom,defineBlock)){
                        //不能move
                        moveAble = false;
                    }

                    if(rhs != NULL && rhs == defValue && !HashSetFind(useBlock->dom,defineBlock)){
                        moveAble = false;
                    }
                }
                currNode = get_next_inst(currNode);
            }
        }

        bool choose1 = true;
        bool choose2 = true;
        // dominates all exits
        HashSetFirst(exit);
        for(BasicBlock *exitBlock = HashSetNext(exit); exitBlock != NULL; exitBlock = HashSetNext(exit)){
            if(!HashSetFind(exitBlock->dom,defineBlock)){
                choose1 = false;
            }
            HashSet *liveIn = exitBlock->in;
            if(HashSetFind(liveIn,defValue)){
                choose2 = false;
            }
        }

        moveAble &= (choose2 | choose1);

        if(moveAble){

            // remove from
            printf("extra move %s to block %d\n",defValue->name, newPrevBlock->id);
            InstNode *find = findNode(defineBlock,instNode);
            removeIns(find);
            // 放到前驱基本块的前面
            InstNode *loopPrevTail = newPrevBlock->tail_node;
            instNode->Parent = newPrevBlock;
            ins_insert_before(find,loopPrevTail);
        }
    }
    //
    //销毁内存
    HashSetDeinit(loopInvariantVariable);
    HashSetDeinit(def);
}