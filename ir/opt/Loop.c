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

    //对刚刚收集到的loop进行一系列的整合构建
    HashSetFirst(allLoops);
    for(Loop *l = HashSetNext(allLoops); l != NULL; l = HashSetNext(allLoops)){
        HashSetFirst(l->loopBody);
        printf("Loop head b%d: ",l->head->id);
        for(BasicBlock *block = HashSetNext(l->loopBody); block != NULL; block = HashSetNext(l->loopBody)){
            printf("b%d",block->id);
        }
        printf("\n");
    }


    //循环森林的构建
    HashSet *tempSet = HashSetInit();
    HashSetCopy(tempSet,allLoops);
    assert(HashSetSize(allLoops) == HashSetSize(tempSet));

    HashSetFirst(allLoops);
    for(Loop *l = HashSetNext(allLoops); l != NULL; l = HashSetNext(allLoops)){
        //找到一个头节点
        BasicBlock *head = l->head;
        //寻找包含它的最小body数的Loop

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
}


//找到loop的头和body
Loop *constructLoop(BasicBlock *head,BasicBlock *tail){
    Loop *loop = (Loop *)malloc(sizeof(Loop));

    //initialize data structure
    memset(loop,0,sizeof(Loop));
    loop->tail = tail;
    loop->head = head;
    loop->exit = HashSetInit();
    loop->loopBody = HashSetInit();
    loop->child = HashSetInit();


    findBody(loop);
    findExit(loop);
    HashSet *loopBody = loop->loopBody;
    HashSet *exit = loop->exit;
    // 打印看看循环找的对不对
    HashSetFirst(loopBody);
    printf("loop : ");
    for(BasicBlock *block = HashSetNext(loopBody); block != NULL; block = HashSetNext(loopBody)){
        printf("b%d",block->id);
    }
    printf("\n");



    return loop;
}

bool LICM(Function *currentFunction){
    bool effective = false;
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        //对于每一颗子树 对其进行深度优先遍历
        dfsLoopTree(root);
    }
    return effective;
}

bool dfsLoopTree(Loop *loop){
    bool effective = false;
    HashSetFirst(loop->child);
    for(Loop *childLoop = HashSetNext(loop->child); childLoop != NULL; childLoop = HashSetNext(loop->child)){
        /*内层循环先处理*/
       effective |= dfsLoopTree(childLoop);
    }
    effective |= LICM_EACH(loop);
    return effective;
}

bool LICM_EACH(Loop *loop){
    bool effective = false;

    // reconstruct the loop body since it can have change due to its child loop
    reconstructLoop(loop);

    //first find all def in loop
    HashSet *def = HashSetInit();
    HashSetFirst(loop->loopBody);
    for(BasicBlock *body = HashSetNext(loop->loopBody); body != NULL; body = HashSetNext(loop->loopBody)){
        InstNode *currNode = body->head_node;
        InstNode *bodyTail = body->tail_node;
        while(currNode != bodyTail){
            if(!hasNoDestOperator(currNode)){
                Value *insValue = ins_get_dest(currNode->inst);
                printf("def %s\n",insValue->name);
                HashSetAdd(def,insValue);
            }
            currNode = get_next_inst(currNode);
        }
    }

    /* now the algorithm begins*/

    HashSet *loopInvariantVariable = HashSetInit();
    //def 全部知道了那我们就开始迭代吧 但是需要保证迭代的正确性
    bool changed = true;
    while(changed){
        printf("here!\n");
        changed = false;
        HashSetFirst(loop->loopBody);
        BasicBlock *block = NULL;
        for(block = HashSetNext(loop->loopBody);block != NULL; block = HashSetNext(loop->loopBody)){
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
                    }else if(!HashSetFind(def,lhs) && HashSetFind(loopInvariantVariable,rhs)){
                        if(!HashSetFind(loopInvariantVariable,dest)){
                            HashSetAdd(loopInvariantVariable,dest);
                            changed = true;
                        }
                    }else if(HashSetFind(loopInvariantVariable,lhs) && !HashSetFind(def,rhs)){
                        if(!HashSetFind(loopInvariantVariable,dest)){
                            HashSetAdd(loopInvariantVariable,dest);
                            changed = true;
                        }
                    }else if(HashSetFind(loopInvariantVariable,lhs) && HashSetFind(loopInvariantVariable,rhs)){
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
    // 还需要记录一下当前use到的lhs rhs如果是通过标记进来的话它的标记有无挪到前面去
    HashSet *moveOut = HashSetInit();
    //并且如果有一个放置出去了所有的都需要重新再次计算一次

    bool removeOne = true;
    while(removeOne){
        removeOne = false;
        printf("here!\n");
        HashSetFirst(loopInvariantVariable);
        for(Value *var = HashSetNext(loopInvariantVariable); var != NULL; var = HashSetNext(loopInvariantVariable)){
            //首先看看这个var
            Instruction *tempIns = (Instruction*)var;
            Value *lhs = ins_get_lhs(tempIns);
            Value *rhs = ins_get_rhs(tempIns);
            if(HashSetFind(loopInvariantVariable,lhs) && !HashSetFind(moveOut,lhs)){
                continue;
            }
            if(HashSetFind(loopInvariantVariable,rhs) && !HashSetFind(moveOut,rhs)){
                continue;
            }
            printf("var %s\n",var->name);
            Instruction *ins = (Instruction*)var;
            BasicBlock *block = ins->Parent;
            //必须满足两个条件
            //1 A 在循环L中的其他地方没有定值语句 SSA ！！

            //2 循环L中对A的使用只有S中对于A的定值能够到 SSA应该也满足！！

            //所以就两个条件
            bool cond1 = true;
            bool cond2 = true;

            //S节点是循环L的所有出口节点的必经节点
            HashSetFirst(loop->exit);
            for(BasicBlock *exitBlock = HashSetNext(loop->exit); exitBlock != NULL; exitBlock = HashSetNext(loop->exit)){
                if(!HashSetFind(exitBlock->dom,block)){
                    cond1 = false;
                }
                if(HashSetFind(exitBlock->out,var)){
                    cond2 = false;
                }
            }

            if(cond1 || cond2){
                removeOne = true;
                printf("remove!\n");
                HashSetRemove(loopInvariantVariable,var);
                BasicBlock *head = loop->head;
                HashSet *newBlockPrev = HashSetInit();
                HashSetFirst(head->preBlocks);
                for(BasicBlock *preBlock = HashSetNext(head->preBlocks); preBlock != NULL; preBlock = HashSetNext(head->preBlocks)){
                    //看它是不是在我们的loop里面
                    if(!HashSetFind(loop->loopBody,preBlock)){
                        HashSetAdd(newBlockPrev,preBlock);
                    }
                }
                BasicBlock *newPrevBlock = NULL;
                //需要判断是否需要新的基本块
                //如果除开循环前驱大于1
                if(HashSetSize(newBlockPrev) > 1){
                    newPrevBlock = newBlock(newBlockPrev,head);
                }else{
                    //找到唯一不属于循环的前驱节点
                    assert(HashSetSize(newBlockPrev) == 1);
                    HashSetFirst(newBlockPrev);
                    newPrevBlock = HashSetNext(newBlockPrev);
                }

                // remove from
                printf("move %s to block %d\n",var->name, newPrevBlock->id);
                InstNode *find = findNode(block,ins);
                removeIns(find);
                // 放到前驱基本块的前面
                InstNode *loopPrevTail = newPrevBlock->tail_node;
                ins->Parent = newPrevBlock;
                ins_insert_before(find,loopPrevTail);
            }
        }
    }

    HashSetDeinit(loopInvariantVariable);
    HashSetDeinit(def);
}


void findBody(Loop *loop){
    //reconstruct loop after optimize
    //clean current loop body
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
        assert(block != NULL);

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
    HashSetClean(loop->exit);

    //遍历一遍loop就行了
    HashSetFirst(loop->loopBody);

    //已经找到了loop 如何找到出口呢？ -》 所有loop里面后继不在loop里面的都是的
    BasicBlock *loopBlock = NULL;
    for(loopBlock = HashSetNext(loop->loopBody); loopBlock != NULL; loopBlock = HashSetNext(loop->loopBody)){
        if(loopBlock->true_block && !HashSetFind(loop->loopBody,loopBlock->true_block)){
            // 放在exit里面
            HashSetAdd(loop->exit,loopBlock->true_block);
        }
        if(loopBlock->false_block && !HashSetFind(loop->loopBody,loopBlock->false_block)){
            HashSetAdd(loop->exit,loopBlock->false_block);
        }
    }

    // 必须有一个出口
    assert(HashSetSize(loop->exit) >= 1);
}

void findInductionVariable(Loop *loop){
    BasicBlock *head = loop->head;
    HashSet *exit = loop->exit;

    // the entry of loop must have only two predecessors
    if(HashSetSize(head->preBlocks) != 2) return;

    //the loop must have only one exit and it must be the entry block
    if(HashSetSize(exit) == 1) return;

    HashSetFirst(exit);
    BasicBlock *exitBlock = HashSetNext(exit);
    if(exitBlock != head) return;


    //find the induction variable
    InstNode *exitTail = exitBlock->tail_node;
    assert(exitTail->inst->Opcode == br_i1);

    Value *cond = ins_get_lhs(exitTail->inst);
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

    //induction variable
    if(loop->inductionVariable){
        printf("inductionVariable is %s\n",loop->inductionVariable->name);

        //同时对这个inductionVariable而言，
        //assume this for all loops
        Instruction *phiIns = (Instruction *)loop->inductionVariable;
        HashSet *phiSet = phiIns->user.value.pdata->pairSet;
        assert(HashSetSize(phiSet) == 2);
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
}


void reconstructLoop(Loop *loop){
    findBody(loop);
    findExit(loop);
    findInductionVariable(loop);
}