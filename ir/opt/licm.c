//
// Created by Valerian on 2023/7/1.
//

#include "licm.h"

bool LICM(Function *currentFunction){
    bool effective = false;
    HashSetFirst(currentFunction->loops);
    printf("current func %d\n", HashSetSize(currentFunction->loops));
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        //对于每一颗子树 对其进行深度优先遍历
        dfsLoopTree(root);
    }
    return effective;
}

bool dfsLoopTree(Loop *loop){
    bool effective = false;
    printf("child %d\n", HashSetSize(loop->child));
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

    // reconstruct the loop body since it can have change due to its child loopAnalysis
    //reconstructLoop(loopAnalysis);

    //first find all def in loopAnalysis
    HashSet *def = HashSetInit();
    HashSetFirst(loop->loopBody);
    for(BasicBlock *body = HashSetNext(loop->loopBody); body != NULL; body = HashSetNext(loop->loopBody)){
        InstNode *currNode = body->head_node;
        InstNode *bodyTail = body->tail_node;
        while(currNode != bodyTail){
            if(!hasNoDestOperator(currNode)){
                Value *insValue = ins_get_dest(currNode->inst);
                //printf("def %s\n",insValue->name);
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
        //printf("here!\n");
        changed = false;
        HashSetFirst(loop->loopBody);
        BasicBlock *block = NULL;
        for(block = HashSetNext(loop->loopBody);block != NULL; block = HashSetNext(loop->loopBody)){
            // 我们需要
            InstNode *currNode = block->head_node;
            while(currNode != block->tail_node){
                // TODO 解决所有Operator的情况 请仔细思考 !!!
                if(isSimpleOperator(currNode)){
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
        //printf("here!\n");
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
            //printf("var %s\n",var->name);
            Instruction *ins = (Instruction*)var;
            BasicBlock *block = ins->Parent;
            //必须满足两个条件
            //1 A 在循环L中的其他地方没有定值语句 SSA ！！

            //2 循环L中对A的使用只有S中对于A的定值能够到 SSA应该也满足！！ ——> 除了Phi函数的情况
            bool cond0 = true;
            HashSetFirst(loop->loopBody);
            for(BasicBlock *bodyBlock = HashSetNext(loop->loopBody); bodyBlock != NULL; bodyBlock = HashSetNext(loop->loopBody)){
                InstNode *phiNode = bodyBlock->head_node;
                InstNode *tailNode = bodyBlock->tail_node;
                while(phiNode != tailNode){
                    if(phiNode->inst->Opcode == Phi){
                        //see if phi operand contain this Var;
                        HashSet *phiSet = phiNode->inst->user.value.pdata->pairSet;
                        HashSetFirst(phiSet);
                        for(pair *phiPair = HashSetNext(phiSet); phiPair != NULL; phiPair = HashSetNext(phiSet)){
                            Value *define = phiPair->define;
                            if(define == var){
                                cond0 = false;
                            }
                        }
                    }
                    phiNode = get_next_inst(phiNode);
                }
            }
            //所以就两个条件
            bool cond1 = true;
            bool cond2 = true;

            //S节点是循环L的所有出口节点的必经节点
            HashSetFirst(loop->exitingBlock);
            for(BasicBlock *exitBlock = HashSetNext(loop->exitingBlock); exitBlock != NULL; exitBlock = HashSetNext(loop->exitingBlock)){
                if(!HashSetFind(exitBlock->dom,block)){
                    cond1 = false;
                }
                if(HashSetFind(exitBlock->out,var)){
                    cond2 = false;
                }
            }

            if((cond1 || cond2) && cond0){
                removeOne = true;
                //printf("remove!\n");
                HashSetRemove(loopInvariantVariable,var);


                //we need to see if the loopAnalysis can perform at least once
                Value *cond = loop->end_cond;
                Value *initValue = loop->initValue;

                //see if the initValue is constant??
                //maybe we'd better perform this pass
                //after remove useless loops

                //so

                BasicBlock *head = loop->head;
                HashSet *newBlockPrev = HashSetInit();
                HashSetFirst(head->preBlocks);
                for(BasicBlock *preBlock = HashSetNext(head->preBlocks); preBlock != NULL; preBlock = HashSetNext(head->preBlocks)){
                    //看它是不是在我们的loop里面
                    if(!HashSetFind(loop->loopBody,preBlock)){
                        printf("not block %d\n",preBlock->id);
                        HashSetAdd(newBlockPrev,preBlock);
                    }
                }

                printf("loop head %d loopAnalysis body %d\n",loop->head->id, HashSetSize(loop->loopBody));
                BasicBlock *newPrevBlock = NULL;
                //需要判断是否需要新的基本块
                //如果除开循环前驱大于1


                //找到唯一不属于循环的前驱节点
                assert(HashSetSize(newBlockPrev) == 1);
                HashSetFirst(newBlockPrev);
                newPrevBlock = HashSetNext(newBlockPrev);
//                assert(newPrevBlock == loop->preHeader);
//                assert(loop->guard != NULL);
//                assert(loop->guard->tail_node->inst->Opcode == br);

                //change the guard terminator to be a br_i1
                //if we hoist a store we need to put a guard

                // remove from
                //printf("move %s to block %d\n",var->name, newPrevBlock->id);
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