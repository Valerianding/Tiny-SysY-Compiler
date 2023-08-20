//
// Created by Valerian on 2023/4/18.
//
//
#include "loopinfo.h"
void examineLoop(Loop *loop){
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        examineLoop(child);
    }

    //examineLoop now
    int preSize = HashSetSize(loop->head->preBlocks);
    HashSetFirst(loop->head->preBlocks);
    int count = 0;
    for(BasicBlock *preBlock = HashSetNext(loop->head->preBlocks); preBlock != NULL; preBlock = HashSetNext(loop->head->preBlocks)){
        if(HashSetFind(loop->loopBody,preBlock)){
            count++;
        }
    }

    //assert(preSize - count == 1);
}


// get all the loopAnalysis info
void loopAnalysis(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;

    HashSet *workList = HashSetInit();  // 先把所有的基本块放在这里面 bfs
    clear_visited_flag(entry);

    currentFunction->loops = HashSetInit();
    HashSet *allLoops = currentFunction->loops;
    //bfs
    //由于后续loop invariant的时候可能导致 visited flag 损失所以我们用一个HashSet来存
    HashSet *visited = HashSetInit();

    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        //
        HashSetRemove(workList,block);
        //block 我们是一定没有visited
        HashSetAdd(visited, block);

        HashSet *dom = block->dom;
        if(block->true_block){
            if(HashSetFind(visited,block->true_block)){
                BasicBlock *head = block->true_block;
                if(HashSetFind(dom,head)){
                    Loop * currentLoop = constructLoop(block->true_block, block);
                    //添加到当前的loop集合当中
                    HashSetAdd(allLoops, currentLoop);
                }
            }else{
                HashSetAdd(workList,block->true_block);
            }
        }

        if(block->false_block){
            if(HashSetFind(visited,block->false_block)){
                BasicBlock *head = block->false_block;
                if(HashSetFind(dom,head)){
                    Loop *currentLoop = constructLoop(block->false_block, block);
                    HashSetAdd(allLoops, currentLoop);
                }
            }else{
                HashSetAdd(workList,block->false_block);
            }
        }
    }

    HashSetDeinit(visited);
    HashSetDeinit(workList);

    //需要提前解决多重回边的问题
    HashSet *tempSet = HashSetInit();
    HashSetCopy(tempSet,allLoops);
    //assert(HashSetSize(allLoops) == HashSetSize(tempSet));


    HashSetFirst(allLoops);
    for(Loop *l1 = HashSetNext(allLoops); l1 != NULL; l1 = HashSetNext(allLoops)){
        BasicBlock *l1Head = l1->head;
        HashSetFirst(tempSet);
        for(Loop *l2 = HashSetNext(tempSet); l2 != NULL; l2 = HashSetNext(tempSet)){
            BasicBlock *l2Head = l2->head;
            if(l1Head == l2Head && l1 != l2){
                //it means loopAnalysis with multi back-edge
                l1->containMultiBackEdge = true;
                //copy loopAnalysis body
                HashSetFirst(l2->loopBody);
                for(BasicBlock *l2Body = HashSetNext(l2->loopBody); l2Body != NULL; l2Body = HashSetNext(l2->loopBody)){
                    HashSetAdd(l1->loopBody,l2Body);
                }


                //also for reconstruct use we need to push
                if(l1->latches == NULL){
                    l1->latches = HashSetInit();
                }

                HashSetAdd(l1->latches,l2->tail);

                //remove l2 from Loops
                HashSetRemove(tempSet,l2);
                HashSetRemove(allLoops,l2);
            }
        }
    }

    printf("--------\n");

    printf("function:%s contain loops:\n",currentFunction->name);
    //对刚刚收集到的loop
    HashSetFirst(allLoops);
    for(Loop *l = HashSetNext(allLoops); l != NULL; l = HashSetNext(allLoops)){
        HashSetFirst(l->loopBody);
        printf("Loop head b%d: ",l->head->id);
        for(BasicBlock *block = HashSetNext(l->loopBody); block != NULL; block = HashSetNext(l->loopBody)){
            printf("b%d",block->id);
        }

        //exiting的next block不能有前驱
        bool hasDedicatedExist = true;
        HashSetFirst(l->exitingBlock);
        for(BasicBlock *block = HashSetNext(l->exitingBlock); block != NULL; block = HashSetNext(l->exitingBlock)){
            //exitingblock的true / false不在我们body里面的block就是exit
            if(block->true_block && !HashSetFind(l->loopBody,block->true_block)){
                BasicBlock *exit = block->true_block;
                //如果前驱存在不在循环里面的block代表了
                HashSetFirst(exit->preBlocks);
                for(BasicBlock *exitPrev = HashSetNext(exit->preBlocks); exitPrev != NULL; exitPrev = HashSetNext(exit->preBlocks)){
                    if(!HashSetFind(l->loopBody,exitPrev)){
                        hasDedicatedExist = false;
                    }
                }
            }
            if(block->false_block && !HashSetFind(l->loopBody,block->false_block)){
                BasicBlock *exit = block->false_block;
                HashSetFirst(exit->preBlocks);
                for(BasicBlock *exitPrev = HashSetNext(exit->preBlocks); exitPrev != NULL; exitPrev = HashSetNext(exit->preBlocks)){
                    if(!HashSetFind(l->loopBody,exitPrev)){
                        hasDedicatedExist = false;
                    }
                }
            }
        }
        if(hasDedicatedExist){
            l->hasDedicatedExit = true;
            printf("with a dedicated exist!");
        }else{
            l->hasDedicatedExit = false;
            printf("with not a dedicated exist!");
        }
        printf("\n");
    }



    printf("loopAnalysis size %d\n", HashSetSize(allLoops));
    HashSetFirst(allLoops);
    for(Loop *l = HashSetNext(allLoops); l != NULL; l = HashSetNext(allLoops)){
        //找到一个头节点
        BasicBlock *head = l->head;
        //寻找包含它的最小body数的Loop

        printf("loopAnalysis info body size %d\n", HashSetSize(l->loopBody));
        Loop *parent = NULL;
        HashSetFirst(tempSet);
        unsigned int minSize = 0xffffffff;
        for(Loop *next = HashSetNext(tempSet); next != NULL; next = HashSetNext(tempSet)){
            if(next != l && HashSetFind(next->loopBody,head)){
                //存在嵌套
                if(minSize == 0xffffffff || minSize > HashSetSize(next->loopBody)){
                    parent = next;
                    minSize = HashSetSize(next->loopBody);
                }
            }
        }

        if(parent != NULL){
            printf("b%d parent is b%d\n",l->head->id,parent->head->id);
            l->parent = parent;
            HashSetAdd(parent->child,l);
            HashSetRemove(allLoops,l);
        }
    }

    //检查是否是所有loop都有preHeader
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        examineLoop(root);
    }
    HashSetDeinit(tempSet);
}


//找到loop的头和body
Loop *constructLoop(BasicBlock *head,BasicBlock *tail){
    Loop *loop = (Loop *)malloc(sizeof(Loop));

    //initialize data structure
    memset(loop,0,sizeof(Loop));
    loop->tail = tail;
    loop->head = head;
    loop->exitingBlock = HashSetInit();
    loop->loopBody = HashSetInit();
    loop->child = HashSetInit();
    loop->containMultiBackEdge = false;
    //body
    findBody(loop);


    //exit
    findExit(loop);

    //induction variable
    findInductionVariable(loop);

    HashSet *loopBody = loop->loopBody;
    HashSet *exit = loop->exitingBlock;
    // 打印看看循环找的对不对
    HashSetFirst(loopBody);
    printf("loopAnalysis body: ");
    for(BasicBlock *block = HashSetNext(loopBody); block != NULL; block = HashSetNext(loopBody)){
        printf("b%d",block->id);
    }
    printf("\n");


    printf("loopAnalysis exiting:");
    HashSetFirst(exit);
    for(BasicBlock *block = HashSetNext(exit); block != NULL; block = HashSetNext(exit)){
        printf("b%d",block->id);
    }
    printf("\n");



    return loop;
}



void findBody(Loop *loop){
    //reconstruct loopAnalysis after optimize
    //clean current loopAnalysis body
    HashSetClean(loop->loopBody);

    //
    BasicBlock *tail = loop->tail;
    BasicBlock *head = loop->head;

    //
    stack *stack = stackInit();
    HashSetAdd(loop->loopBody,tail);
    HashSetAdd(loop->loopBody,head);

    stackPush(stack,tail);

    BasicBlock *block = NULL;
    while(stackSize(stack) != 0){
        stackTop(stack,(void *)&block);
        stackPop(stack);
        //assert(block != NULL);

        //
        HashSetFirst(block->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
            if(!HashSetFind(loop->loopBody,preBlock)){
                HashSetAdd(loop->loopBody,preBlock);
                stackPush(stack,preBlock);
            }
        }
    }
}

//use only after find body
void findExit(Loop *loop){
    HashSetClean(loop->exitingBlock);

    //遍历一遍loop就行了
    HashSetFirst(loop->loopBody);

    //已经找到了loop 如何找到出口呢？ -》 所有loop里面后继不在loop里面的都是的
    BasicBlock *loopBlock = NULL;
    for(loopBlock = HashSetNext(loop->loopBody); loopBlock != NULL; loopBlock = HashSetNext(loop->loopBody)){
        if(loopBlock->true_block && !HashSetFind(loop->loopBody,loopBlock->true_block)){
            // 放在exit里面
            HashSetAdd(loop->exitingBlock, loopBlock);
        }
        if(loopBlock->false_block && !HashSetFind(loop->loopBody,loopBlock->false_block)){
            HashSetAdd(loop->exitingBlock, loopBlock);
        }
    }

    // 必须有一个出口
    //assert(HashSetSize(loop->exitingBlock) >= 1);
}

void findInductionVariable(Loop *loop){
    BasicBlock *head = loop->head;
    HashSet *exit = loop->exitingBlock;

    // the entry of loopAnalysis must have only two predecessors
    if(HashSetSize(head->preBlocks) != 2) return;


    //the loopAnalysis must have only one exitingBlock
    if(HashSetSize(exit) != 1) return;

   // and it must be the entry block
    HashSetFirst(exit);
    BasicBlock *exitBlock = HashSetNext(exit);
    if(exitBlock != head) return;


    //get the condition
    //TODO can exitingBlock terminator Opcode is not br_i1?
    InstNode *exitTail = exitBlock->tail_node;
    //assert(exitTail->inst->Opcode == br_i1);
    Value *cond = ins_get_lhs(exitTail->inst);
    loop->end_cond = cond;
    printf("end cond is %s\n",cond->name);

    //get the exitingBlock block & body block
    BasicBlock *trueBlock = exitBlock->true_block;
    BasicBlock *falseBlock = exitBlock->false_block;
    loop->body_block = HashSetFind(loop->loopBody,trueBlock) ? trueBlock : falseBlock;
    loop->exit_block = HashSetFind(loop->loopBody,trueBlock) ? falseBlock : trueBlock;


    printf("loopAnalysis body block %d exit block %d\n",loop->body_block->id,loop->exit_block->id);
    //get the induction variable
    BasicBlock *tail = loop->tail;
    InstNode *currNode = exitBlock->head_node;
    while(currNode != exitTail){
        if(currNode->inst->Opcode == Phi){
            Value *phiValue = ins_get_dest(currNode->inst);
            Use *uses = phiValue->use_list;
            while(uses != NULL){
                printf("one use!\n");
                //find the user
                Value *phiUser = (Value*)uses->Parent;
                if(phiUser == cond){
                    loop->inductionVariable = phiValue;
                    break;
                }
                uses = uses->Next;
            }

            //有点问题因为这个value 的use还有可能是在phiInstruction里面的
            InstNode *tempNode = exitBlock->head_node;
            while(tempNode != tail->tail_node){
                if(tempNode->inst->Opcode == Phi){
                    HashSet *phiSet = tempNode->inst->user.value.pdata->pairSet;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        if(phiInfo->define == phiValue){
                            Value *dest = ins_get_dest(tempNode->inst);
                            if(dest == cond) {
                                loop->inductionVariable = phiValue;
                                break;
                            }
                        }
                    }
                }
                tempNode = get_next_inst(tempNode);
            }
        }
        currNode = get_next_inst(currNode);
    }

    if(!loop->inductionVariable) return;

    //induction variable
    if(loop->inductionVariable){
        printf("inductionVariable is %s\n",loop->inductionVariable->name);

        //同时对这个inductionVariable而言，
        //assume this for all loops
        Instruction *phiIns = (Instruction *)loop->inductionVariable;
        HashSet *phiSet = phiIns->user.value.pdata->pairSet;
        //assert(HashSetSize(phiSet) == 2);
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
            //找到
            Value *phiOperand = phiInfo->define;
            BasicBlock *from = phiInfo->from;
            if(!HashSetFind(loop->loopBody,from)){
                loop->initValue = phiOperand;
            }else{
                //modifier is the instruction
                loop->modifier = phiOperand;
            }
        }
        printf("initValue %s, modifier %s\n",loop->initValue->name,loop->modifier->name);
    }


    //check if the condition changes over the iterations

    //找到 end_cond -> 找lhs 、 rhs Instruction -> 如果还是在entry基本块内就加入workList 一旦读到load -> gg
    //assert(cond != NULL);
    Instruction *endIns = (Instruction *)cond;

    Function *func = loop->head->Parent;
    int paramNum = func->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;
    bool changeOverIteration = false;
    HashSet *workList = HashSetInit();
    HashSetAdd(workList,endIns);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        Instruction *ins = HashSetNext(workList);
        HashSetRemove(workList,ins);
        if(ins->Opcode == Load){
            changeOverIteration = true;
            break;
        }
        Value *dest = ins_get_dest(ins);
        Value *lhs = NULL;
        Value *rhs = NULL;
        int numOfOperand = dest->NumUserOperands;
        if(numOfOperand == 1){
            lhs = ins_get_lhs(ins);
        }else{
            lhs = ins_get_lhs(ins);
            rhs = ins_get_rhs(ins);
        }
        if(lhs && !isImm(lhs) && !isParam(lhs,paramNum)){
            Instruction *lhsIns = (Instruction *)lhs;
            if(lhsIns->Parent == loop->head){
                HashSetAdd(workList,lhsIns);
            }
        }
        if(rhs && !isImm(rhs) && !isParam(rhs,paramNum)){
            Instruction *rhsIns = (Instruction *)rhs;
            if(rhsIns->Parent == loop->head){
                HashSetAdd(workList,rhsIns);
            }
        }
    }

    loop->conditionChangeWithinLoop = changeOverIteration;
    printf("loopAnalysis change %d\n",changeOverIteration);
    HashSetDeinit(workList);
}

