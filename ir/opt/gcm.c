//
// Created by Valerian on 2023/7/22.
//

#include "gcm.h"

//load 明显是pinned 同时也代表乐store必须是pinned
//call 和 give_param 也是pinned是因为我们想避免不必要的讨论了


//当前还没有考虑memset 和 sysymemset
const Opcode pinnedOperations[] = {Load,Store, Phi,br,br_i1, Call, Return,GIVE_PARAM};

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

    //for all inputs x to i do
    if(ins->Opcode == Phi){
        HashSet *phiSet = ins->user.value.pdata->pairSet;
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)) {
            Value *define = phiInfo->define;
            Instruction *inputIns = NULL;
            if (define != NULL && !isParam(define, paramNum) && !isImm(define)) {
                //now this define must be produced by an instruction in the function
                inputIns = (Instruction *) define;
                Schedule_Early(inputIns);
            }

            if(inputIns != NULL){
                DomTreeNode *xNode = inputIns->Parent->domTreeNode;
                //if i.block.dom_depth < x.block.dom_depth
                if(iNode->depth < xNode->depth){
                    iNode = xNode;
                }
            }
        }
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

    }


    //after all we can schedule this instruction to the iNode -> block tail ?
}


//we should follow a dfs travel over the pinned instructions ?
void ScheduleEarly(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    InstNode *pinnedNode = entry->head_node;
    InstNode *tailNode = tail->tail_node;

    int paramNum = currentFunction->entry->head_node->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    printf("function %s has %d param!\n",currentFunction->name,paramNum);

    //for all instructions i
    while(pinnedNode != tailNode){
        //if i is pinned
        if(isPinnedIns(pinnedNode)){
            //i.visited = true
            pinnedNode->inst->visited = true;

            //for all inputs x to i
            if(pinnedNode->inst->Opcode == Phi){
                HashSet *phiSet = pinnedNode->inst->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    Value *define = phiInfo->define;
                    if(define != NULL && !isParam(define,paramNum) && !isImm(define)){
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
            }
        }
        pinnedNode = get_next_inst(pinnedNode);
    }
}


