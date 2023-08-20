//
// Created by Valerian on 2023/5/25.
//


#include "other.h"

//RunOnLoop

//if any loop match the pattern

//check if the preHeader has Store

//dirty pattern
bool checkMemOptLoop(Loop *loop){
    BasicBlock *head = loop->head;
    if(HashSetSize(head->preBlocks) != 2) return false;

    if(HashSetSize(loop->loopBody) != 2) return false;

    //need the cmp to be a lst
    if(!loop->end_cond) return false;

    if(!loop->modifier) return false;

    Instruction *icmp = (Instruction *)loop->end_cond;

    Value *icmpRhs = ins_get_rhs(icmp);

    if(!isImmInt(icmpRhs)) return false;

    if(!loop->body_block) return false;

    if(!loop->initValue) return false;

    //the initValue must be zero
    if(!isImmInt(loop->initValue) || loop->initValue->pdata->var_pdata.iVal != 0) return false;

    //dirty pattern
    //gep load add add
    int index = 0;

    BasicBlock *bodyBlock = loop->body_block;
    InstNode *bodyHead = bodyBlock->head_node;
    InstNode *bodyTail = bodyBlock->tail_node;
    InstNode *nextNode = get_next_inst(bodyTail);
    bodyHead = get_next_inst(bodyHead);

    int numParam = bodyBlock->Parent->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    Value *expectedLoad = NULL;
    Value *load = NULL;
    while(bodyHead != nextNode){
        switch (index) {
            case 0: {
                if(bodyHead->inst->Opcode != GEP){
                    return false;
                }

                //gep's index must be induction variable
                Value *gepIndex = ins_get_rhs(bodyHead->inst);
                if(gepIndex != loop->inductionVariable){
                    return false;
                }

                expectedLoad = ins_get_dest(bodyHead->inst);
                break;
            }
            case 1:{
                if(bodyHead->inst->Opcode != Load){
                    return false;
                }

                Value *actLoad = ins_get_lhs(bodyHead->inst);
                if(actLoad != expectedLoad){
                    return false;
                }

                load = ins_get_dest(bodyHead->inst);
                //this load must only have one use
                int countUse = 0;
                Use *loadUses = load->use_list;
                while(loadUses != NULL){
                    countUse++;
                    loadUses = loadUses->Next;
                }

                if(countUse != 1){
                    return false;
                }

                break;
            }
            case 2:{
                if(bodyHead->inst->Opcode != Add){
                    return false;
                }

                Value *addLhs = ins_get_lhs(bodyHead->inst);
                Value *addRhs = ins_get_rhs(bodyHead->inst);

                if(addLhs == loop->inductionVariable){
                    if(!isImmInt(addRhs) || addRhs->pdata->var_pdata.iVal != 1){
                        return false;
                    }
                }else{
                    //if not
                    //must be add phi, (load)
                    if(isImm(addLhs) || isParam(addLhs,numParam)){
                        return false;
                    }

                    Instruction *expectedPhi = (Instruction *)addLhs;
                    if(expectedPhi->Opcode != Phi){
                        return false;
                    }

                    //rhs must come from load
                    if(addRhs != load){
                        return false;
                    }
                }
                break;
            }
            case 3:{
                if(bodyHead->inst->Opcode != Add){
                    return false;
                }

                Value *addLhs = ins_get_lhs(bodyHead->inst);
                Value *addRhs = ins_get_rhs(bodyHead->inst);

                if(addLhs == loop->inductionVariable){
                    if(!isImmInt(addRhs) || addRhs->pdata->var_pdata.iVal != 1){
                        return false;
                    }
                }else{
                    //if not
                    //must be add phi, (load)
                    if(isImm(addLhs) || isParam(addLhs,numParam)){
                        return false;
                    }

                    Instruction *expectedPhi = (Instruction *)addLhs;
                    if(expectedPhi->Opcode != Phi){
                        return false;
                    }

                    //rhs must come from load
                    if(addRhs != load){
                        return false;
                    }

                }
                break;
            }
            case 4:{
                if(bodyHead->inst->Opcode != br){
                    return false;
                }
                break;
            }
            default: {
                return false;
            }
        }
        index++;
        bodyHead = get_next_inst(bodyHead);
    }
    return true;
}

void MemOptOnLoop(Loop *loop){
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        MemOptOnLoop(child);
    }

    if(checkMemOptLoop(loop)){
        //get the tripCount of the loop
        Instruction *icmp = (Instruction *)loop->end_cond;

        Value *tripCount = ins_get_rhs(icmp);

        assert(isImmInt(tripCount));

        int countStore = tripCount->pdata->var_pdata.iVal;


        //see if the loop preHeader has tripCount number's Store
        BasicBlock *preHeader = NULL;
        BasicBlock *loopEntry = loop->head;

        HashSetFirst(loopEntry->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(loopEntry->preBlocks); preBlock != NULL; preBlock = HashSetNext(loopEntry->preBlocks)){
            if(!HashSetFind(loop->loopBody,preBlock)){
                preHeader = preBlock;
            }
        }

        //check if have same number of store
        InstNode *preHead = preHeader->head_node;
        InstNode *preTail = preHeader->tail_node;

        printf("%d\n",countStore);

        int addNum = 0;
        int nowStore = 1;
        while(preHead != preTail){
            if(preHead->inst->Opcode == Store){
                //and the store must be constant
                Value *stored = ins_get_lhs(preHead->inst);
                if(!isImmInt(stored)){
                    return;
                }

                addNum += stored->pdata->var_pdata.iVal;
                nowStore++;
            }

            //store is to the same one dimension array
            preHead = get_next_inst(preHead);
        }


        //find the load
        InstNode *bodyHead = loop->body_block->head_node;
        InstNode *bodyTail = loop->body_block->tail_node;
        while(bodyHead != bodyTail){

            bodyHead = get_next_inst(bodyHead);
        }

        if(nowStore == countStore){
            printf("stored %d\n",addNum);
            //now we know that all the store are useless
            assert(false);

            //get the phi's intiValue


            //add the initValue with all the constants
        }
    }
}

void memOpt(Function *currentFunction){
    HashSet *loops = currentFunction->loops;
    HashSetFirst(loops);
    for(Loop *root = HashSetNext(loops); root != NULL; root = HashSetNext(loops)){
        MemOptOnLoop(root);
    }
}