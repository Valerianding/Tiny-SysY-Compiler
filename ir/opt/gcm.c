//
// Created by Valerian on 2023/7/22.
//

#include "gcm.h"

//load 明显是pinned 同时也代表乐store必须是pinned
//call 和 give_param 也是pinned是因为我们想避免不必要的讨论了


//当前还没有考虑memset 和 sysymemset
const Opcode pinnedOperations[] = {Load,Store, Phi,br,br_i1, Call, Return,GIVE_PARAM,EQ,NOTEQ,GREAT,GREATEQ,LESS,LESSEQ,Div,Mod,SysYMemset,SysYMemcpy};

void bfsTravelDomTree(DomTreeNode *root,int level){
    root->depth = level;
    HashSetFirst(root->children);
    for(DomTreeNode *child = HashSetNext(root->children); child != NULL; child = HashSetNext(root->children)){
        bfsTravelDomTree(child,level + 1);
    }
}

void markDominanceDepth(Function *function){
    //bfs 遍历支配树
    bfsTravelDomTree(function->root,0);
}

void bfsTravelLoopTree(Loop *loop,int nest){
    //对loop里面
    HashSetFirst(loop->loopBody);
    for(BasicBlock *loopBlock = HashSetNext(loop->loopBody); loopBlock != NULL; loopBlock = HashSetNext(loop->loopBody)){
        loopBlock->domTreeNode->loopNest =  nest;
    }

    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        bfsTravelLoopTree(child,nest + 1);
    }
}

void markLoopNest(Function *function){
    HashSetFirst(function->loops);
    for(Loop *root = HashSetNext(function->loops); root != NULL; root = HashSetNext(function->loops)){
        bfsTravelLoopTree(root,1);
    }
}

bool isPinnedIns(InstNode *instNode){
    int n = sizeof(pinnedOperations) / sizeof(Opcode);
    for(int i = 0; i < n; i++){
        if(instNode->inst->Opcode == pinnedOperations[i]){
            return true;
        }
    }
    return false;
}

void Schedule_Early(Instruction *ins){
    BasicBlock *curBlock = ins->Parent;

    //printf("curIns %d, curBlock %d\n",ins->i,curBlock->id);
    Function *function = curBlock->Parent;
    assert(function != NULL);

    BasicBlock *entry = function->entry;
    int paramNum = entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;
    if(ins->visited == true){
        return;
    }

    //
    ins->visited = true;

    //i.block = root start with shallowest dominator
    DomTreeNode *iNode = function->root;

    BasicBlock *originalBlock = ins->Parent;
    InstNode *insNode = findNode(originalBlock,ins);

    bool Limited = false; // true -> 必须放在input block对应node的后面  为false -> 放在最前面

    //for all inputs x to i do
    if(ins->Opcode == Phi){
        HashSet *phiSet = ins->user.value.pdata->pairSet;
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)) {
            Value *define = phiInfo->define;
            Instruction *inputIns = NULL;
            if (define != NULL && !isImm(define) && !isParam(define, paramNum) ) {
                //now this define must be produced by an instruction in the function
                inputIns = (Instruction *) define;
                Schedule_Early(inputIns);
            }
        }
        iNode = NULL;
    }else if(ins->Opcode == Call){
        //we do not want to move this call
        //make iNode to be NULL so we know we can't modify this instruction
        iNode = NULL;
    }else if(ins->Opcode == GIVE_PARAM){
        Value *param = ins_get_lhs(ins);
        //同样我们不希望分析 参数 / 立即数
        if(!isImm(param) && !isParam(param,paramNum)){
            //找到那条instruction
            Instruction *inputIns = (Instruction *)param;
            Schedule_Early(inputIns);
        }

        //also it is a pinned instruction we don't want to modify this instruction
        iNode = NULL;
    }else if(ins->Opcode == Load){
        Value *loadPlace = ins_get_lhs(ins);
        if(!isGlobalVar(loadPlace)){
            //那么一定是gep出来的一条指令
            //printf("load place %s\n",loadPlace->name);
            Instruction *inputIns = (Instruction *)loadPlace;
            Schedule_Early(inputIns);
        }

        iNode = NULL;
    }else if(ins->Opcode == Store){
        Value *stored = ins_get_lhs(ins);
        Value *storedPlace = ins_get_rhs(ins);

        if(!isImm(stored) && !isParam(stored,paramNum)){
            Instruction *inputIns = (Instruction *)stored;
            Schedule_Early(inputIns);
        }

        if(!isGlobalVar(storedPlace)){
            Instruction *inputIns = (Instruction *)storedPlace;
            Schedule_Early(inputIns);
        }

        iNode = NULL;
    }else if(ins->Opcode == Return){
        int numOfOperand = ins->user.value.NumUserOperands;
        if(numOfOperand == 1){
            Value *returnValue = ins_get_lhs(ins);
            if(!isImm(returnValue) && !isParam(returnValue,paramNum)){
                Instruction *inputIns = (Instruction *)returnValue;
                Schedule_Early(inputIns);
            }
        }

        iNode = NULL;
    }else if(ins->Opcode == br_i1){
        Value *cond = ins_get_lhs(ins);
        Instruction *inputIns = (Instruction *)cond;
        Schedule_Early(inputIns);

        iNode = NULL;
    }else if(ins->Opcode == Alloca){
        //如果是alloca的话 我们就不用分析了
        iNode = NULL;
    }else if(isCompareOperator(insNode)){
        Value *lhs = ins_get_lhs(ins);
        Value *rhs = ins_get_rhs(ins);
        if(!isImm(lhs) && !isParam(lhs,paramNum)){
            Instruction *inputIns = (Instruction *)lhs;
            Schedule_Early(inputIns);
        }
        if(!isImm(rhs) && !isParam(rhs,paramNum)){
            Instruction *inputIns = (Instruction *)rhs;
            Schedule_Early(inputIns);
        }
        iNode = NULL;
    }else if(ins->Opcode == Div || ins->Opcode == Mod){
        //避免 / 0 或者 % 0的情况出现
        Value *lhs = ins_get_lhs(ins);
        Value *rhs = ins_get_rhs(ins);
        if(!isImm(lhs) && !isParam(lhs,paramNum)){
            Instruction *inputIns = (Instruction *)lhs;
            Schedule_Early(inputIns);
        }
        if(!isImm(rhs) && !isParam(rhs,paramNum)){
            Instruction *inputIns = (Instruction *)rhs;
            Schedule_Early(inputIns);
        }
        iNode = NULL;
    }else{
        //not a pinned instruction we can move these instructions forward
        //printf("current ins is %d\n",ins->i);
        Value *insDest = ins_get_dest(ins);

        int numOfOperand = insDest->NumUserOperands;

        Value *lhs = NULL;
        Value *rhs = NULL;

        //从中去取出Value1 和 Value2
        if(numOfOperand == 2){
            lhs = ins_get_lhs(ins);

            rhs = ins_get_rhs(ins);

            //判断lhs 和 rhs是不是可以溯源 并且还不能是全局数组！
            if(lhs != NULL && !isImm(lhs) && !isParam(lhs,paramNum) && !isGlobalArray(lhs)){
                Instruction *lhsIns = (Instruction *)lhs;
                Schedule_Early(lhsIns);
                DomTreeNode *lhsNode = lhsIns->Parent->domTreeNode;
                if(iNode->depth < lhsNode->depth){
                    Limited = true;
                    iNode = lhsNode;
                }
                //printf("lhs dom depth %d, ",lhsNode->depth);
            }

            if(rhs != NULL && !isImm(rhs) && !isParam(rhs,paramNum) && !isGlobalArray(rhs)){
                Instruction *rhsIns = (Instruction *)rhs;
                Schedule_Early(rhsIns);
                DomTreeNode *rhsNode = rhsIns->Parent->domTreeNode;
                if(iNode->depth < rhsNode->depth){
                    Limited = true;
                    iNode = rhsNode;
                }
                //("rhs dom depth %d\n",rhsNode->depth);
            }


        }else if(numOfOperand == 1){
            lhs = ins_get_lhs(ins);
            if(!isImm(lhs) && !isParam(lhs,paramNum)){
                Instruction *lhsIns = (Instruction *)lhs ;
                Schedule_Early(lhsIns);
                DomTreeNode *lhsNode = lhsIns->Parent->domTreeNode;
                if(iNode->depth < lhsNode->depth){
                    Limited = true;
                    iNode = lhsNode;
                }
                //printf("lhsNode depth %d\n",lhsNode->depth);
            }
        }
    }
    //after all we can schedule this instruction to the iNode -> block tail ?
    //we need to

    if(iNode != NULL && iNode->block != originalBlock){
        //we can schedule this instruction now
        BasicBlock *insertBlock = iNode->block;


        assert(insNode != NULL);

        //remove currNode from curr Block

        removeIns(insNode);

        InstNode *insertPoint = NULL;
        if(insertBlock->tail_node->inst->Opcode == br_i1){
            //找到对应的Value *
            Value *lhs = ins_get_lhs(insertBlock->tail_node->inst);
            Instruction *icmpIns = (Instruction *)lhs;
            insertPoint = findNode(insertBlock,icmpIns);
        }else{
            insertPoint = insertBlock->tail_node;
        }
        //我们是先schedule input 然后再schedule self所以我们每次尾插是可以的
        assert(insertPoint != NULL);
        //我们需要跳过
        if(Limited){
            //放在最后？
            ins_insert_before(insNode,insertPoint);
            insNode->inst->Parent = insertBlock;
        }else{
            //not limited -> root
            ins_insert_before(insNode,insertPoint);
            insNode->inst->Parent = insertBlock;
        }
    }
}


//TODO we should follow a dfs travel over the pinned instructions ?
void ScheduleEarly(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    InstNode *pinnedNode = entry->head_node;
    InstNode *tailNode = tail->tail_node;

    markDominanceDepth(currentFunction);

    int paramNum = currentFunction->entry->head_node->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    //printf("function %s has %d param!\n",currentFunction->name,paramNum);

    //for all instructions i
    while(pinnedNode != tailNode){
        //if i is pinned
        if(isPinnedIns(pinnedNode)){
            //printf("at pinned instruction %d\n",pinnedNode->inst->i);
            //i.visited = true
            pinnedNode->inst->visited = true;

            //for all inputs x to i
            if(pinnedNode->inst->Opcode == Phi){
                HashSet *phiSet = pinnedNode->inst->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    Value *define = phiInfo->define;
                    if(define != NULL && !isImm(define) && !isParam(define,paramNum)){
                        //now this define must be produced by an instruction in the function
                        Instruction *inputIns = (Instruction *)define;
                        Schedule_Early(inputIns);
                    }
                }
            }else if(pinnedNode->inst->Opcode == Call){
                //do nothing -> called 没什么用 我们主要关心Give_param

            }else if(pinnedNode->inst->Opcode == GIVE_PARAM){
                //only lhs is usefull
                Value *param = ins_get_lhs(pinnedNode->inst);
                //同样我们不希望分析 参数 / 立即数
                if(!isImm(param) && !isParam(param,paramNum)){
                    //找到那条instruction
                    Instruction *inputIns = (Instruction *)param;
                    Schedule_Early(inputIns);
                }
            }else if(pinnedNode->inst->Opcode == Load){
                //load的只有可能是全局变量 / 某个地址
                Value *loadPlace = ins_get_lhs(pinnedNode->inst);
                if(!isGlobalVar(loadPlace)){
                    //那么一定是gep出来的一条指令
                    Instruction *inputIns = (Instruction *)loadPlace;
                    Schedule_Early(inputIns);
                }
            }else if(pinnedNode->inst->Opcode == Store){
                Value *stored = ins_get_lhs(pinnedNode->inst);
                Value *storedPlace = ins_get_rhs(pinnedNode->inst);

                if(!isImm(stored) && !isParam(stored,paramNum)){
                    Instruction *inputIns = (Instruction *)stored;
                    Schedule_Early(inputIns);
                }

                if(!isGlobalVar(storedPlace)){
                    Instruction *inputIns = (Instruction *)storedPlace;
                    Schedule_Early(inputIns);
                }
            }else if(pinnedNode->inst->Opcode == Return){
                //寻找到是否有lhs
                int numOfOperand = pinnedNode->inst->user.value.NumUserOperands;
                if(numOfOperand == 1){
                    Value *returnValue = ins_get_lhs(pinnedNode->inst);
                    if(!isImm(returnValue) && !isParam(returnValue,paramNum)){
                        Instruction *inputIns = (Instruction *)returnValue;
                        Schedule_Early(inputIns);
                    }
                }
            }else if(pinnedNode->inst->Opcode == br_i1){
                Value *cond = ins_get_lhs(pinnedNode->inst);
                Instruction *inputIns = (Instruction *)cond;
                Schedule_Early(inputIns);
            }else if(isCompareOperator(pinnedNode)){
                Value *lhs = ins_get_lhs(pinnedNode->inst);

                Value *rhs = ins_get_rhs(pinnedNode->inst);

                if(!isImm(lhs) && !isParam(lhs,paramNum)){
                    Instruction *intputIns = (Instruction *)lhs;
                    Schedule_Early(intputIns);
                }

                if(!isImm(rhs) && !isParam(rhs,paramNum)){
                    Instruction *inputIns = (Instruction *)rhs;
                    Schedule_Early(inputIns);
                }
            }else if(pinnedNode->inst->Opcode == Mod || pinnedNode->inst->Opcode == Div){
                Value *lhs = ins_get_lhs(pinnedNode->inst);

                Value *rhs = ins_get_rhs(pinnedNode->inst);

                if(!isImm(lhs) && !isParam(lhs,paramNum)){
                    Instruction *intputIns = (Instruction *)lhs;
                    Schedule_Early(intputIns);
                }

                if(!isImm(rhs) && !isParam(rhs,paramNum)){
                    Instruction *inputIns = (Instruction *)rhs;
                    Schedule_Early(inputIns);
                }
            }
        }
        pinnedNode = get_next_inst(pinnedNode);
    }
}

DomTreeNode *Find_LCA(DomTreeNode *lca, DomTreeNode *use){
    assert(use != NULL);
    if(lca == NULL){
        //printf("use %d\n",use->block->id);
        return use;
    }
    //printf("lca %d use %d\n",lca->block->id,use->block->id);
    while(lca->depth > use->depth){
        lca = lca->parent->domTreeNode;
    }

    while(use->depth > lca->depth){
        use = use->parent->domTreeNode;
    }

    //
    while(use != lca){
        use = use->parent->domTreeNode;
        lca = lca->parent->domTreeNode;
    }

    assert(use == lca);
    return use;
}

void Schedule_Late(Instruction *ins){
    //printf("ins %d\n",ins->i);
    if(ins->visited == true){
        return;
    }
    //printf("now ins %d\n",ins->i);
    BasicBlock *block = ins->Parent;
    Function *function = block->Parent;

    assert(block != NULL);
    assert(function != NULL);

    //being visited now
    ins->visited = true;

    //start the lca empty
    DomTreeNode *lca = NULL;

    DomTreeNode *use = NULL;
    //for all uses y of ins (Schedule all uses first)
    Value *insDest = ins_get_dest(ins);

    Use *iUses = insDest->use_list;
    while(iUses != NULL){
        User *y = iUses->Parent;
        Instruction *yIns = (Instruction *)y;
        Schedule_Late(yIns);
        use = yIns->Parent->domTreeNode;

        assert(use != NULL);
        //it can't be phi so we just find the lca

        lca = Find_LCA(lca,use);

        assert(lca != NULL);
        iUses = iUses->Next;
    }

    BasicBlock *entry = function->entry;
    BasicBlock *tail = function->tail;

    //find the phi's uses
    InstNode *phiNode = entry->head_node;
    InstNode *funcTail = tail->tail_node;
    while(phiNode != funcTail){
        if(phiNode->inst->Opcode == Phi){
            bool hasUse = false;
            HashSet *phiSet = phiNode->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                Value *define = phiInfo->define;
                if(define == insDest){
                    //
                    use = phiInfo->from->domTreeNode;
                    lca = Find_LCA(lca,use);
                }
            }
        }
        phiNode = get_next_inst(phiNode);
    }

    //只有不是pinned的insNode值得我们去移动，但是pinnedNode 的uses我们还是希望能够Schedule Late
    InstNode *insNode = findNode(ins->Parent,ins);
    //printf("insNode %d\n",insNode->inst->i);
    if(!isPinnedIns(insNode)){

        if(lca == NULL) return;

        //selecting the best block for this instruction
        //also we need to count loopAnalysis depth nest first;
        //
        DomTreeNode *best = lca; // indicates the best place at the lca
        //printf("lca %d\n",lca->block->id);
        while(lca != ins->Parent->domTreeNode){ // not to the earliest
            if(lca->loopNest < best->loopNest){
                best = lca;
            }
            lca = lca->parent->domTreeNode;
        }
        //printf("best %d\n",best->block->id);
        if(best != ins->Parent->domTreeNode){
            //insert at front but we need to insert

            removeIns(insNode);

            BasicBlock *bestBlock = best->block;

            InstNode *blockHead = get_next_inst(bestBlock->head_node);
            InstNode *blockTail = bestBlock->tail_node;
            //not to all insert at the beginning
            //if current Block have use -> insert just before the use
            //if current Block have no use -> insert just before the tail

            //because the phi's use is considered at the predecessor
            //so current block's phi must have no use!!

            bool HaveUse = false;

            while(blockHead != blockTail){
                int numOfOperands = blockHead->inst->user.value.NumUserOperands;
                Value *lhs = NULL;
                Value *rhs = NULL;
                if(numOfOperands == 1){
                    lhs = ins_get_lhs(blockHead->inst);
                }else if(numOfOperands == 2){
                    lhs = ins_get_lhs(blockHead->inst);
                    rhs = ins_get_rhs(blockHead->inst);
                }

                if(lhs != NULL && lhs == insDest){
                    break;
                }
                if(rhs != NULL && rhs == insDest){
                    break;
                }

                blockHead = get_next_inst(blockHead);
            }

            ins_insert_before(insNode,blockHead);

            insNode->inst->Parent = bestBlock;
        }
    }
}



void ScheduleLate(Function *function){
    BasicBlock *entry = function->entry;
    BasicBlock *tail = function->tail;
    //for all instructions i do
    InstNode *pinnedNode = entry->head_node;
    InstNode *funcTail = tail->tail_node;

    while(pinnedNode != funcTail){
        if(isPinnedIns(pinnedNode)){
            pinnedNode->inst->visited = true;

            //for uses y of i do:
            Value *pinnedDest = ins_get_dest(pinnedNode->inst);

            //not count for phi
            Use *uses = pinnedDest->use_list;

            while(uses != NULL){
                User *yUser = uses->Parent;

                Instruction *yIns = (Instruction *)yUser;

                Schedule_Late(yIns);

                uses = uses->Next;
            }


            //special count for phi
            InstNode *tempNode = entry->head_node;
            while(tempNode != funcTail){
                if(tempNode->inst->Opcode == Phi){
                    HashSet *phiSet = tempNode->inst->user.value.pdata->pairSet;
                    bool usedInPhi = false;
                    HashSetFirst(phiSet);
                    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                        Value *define = phiInfo->define;
                        if(define != NULL && define == pinnedDest){
                            //
                            usedInPhi = true;
                        }
                    }

                    if(usedInPhi){
                        Schedule_Late(tempNode->inst);
                    }
                }
                tempNode = get_next_inst(tempNode);
            }
        }
        pinnedNode = get_next_inst(pinnedNode);
    }

    //Schedule late the rest instructions -> avoid
    pinnedNode = entry->head_node;
    while(pinnedNode != funcTail){
        if(pinnedNode->inst->visited == false && pinnedNode->inst->Opcode != Alloca){
            Schedule_Late(pinnedNode->inst);
        }
        pinnedNode = get_next_inst(pinnedNode);
    }
}

void clearInsVisited(Function *function){
    BasicBlock *entry = function->entry;
    BasicBlock *tail = function->tail;

    InstNode *funcHead = entry->head_node;
    InstNode *funcTail = tail->tail_node;

    //从头到尾
    InstNode *tempNode = funcHead;
    while(tempNode != funcTail){
        tempNode->inst->visited = false;
        tempNode = get_next_inst(tempNode);
    }
}

void GCM(Function *currentFunction){
    dominanceAnalysis(currentFunction);

    markDominanceDepth(currentFunction);

    markLoopNest(currentFunction);

    ScheduleEarly(currentFunction);

    DVNT(currentFunction);

    clearInsVisited(currentFunction);

    ScheduleLate(currentFunction);
}