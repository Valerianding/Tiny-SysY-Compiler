//
// Created by Valerian on 2023/8/16.
//

#include "memoryAnalysis.h"

//This pass will analysis memory operation
//1. for each BasicBlock
//2. for each Loop -> load,store外提

//same load -> can be DVNT!!
//TODO 意义不大 -> 可以比赛期间做
void LoopMemoryAnalysis(Function *currentFunction){
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){

    }
}

//This pass will remove redundant call
//This pass needs to be performed before inline
//target -> sort

//Within one Block

//if some function:
// 1. do not have array operation
// 2. do not have global operation
// 3. do not have any other function call
// 4. ALL params are the same!

//=> replace use of call

bool isTargetFunction(Value *function){
    if(function->containMemoryOperations || function->containTime || function->containInput || function->containOutput){
        return false;
    }
    return true;
}

void RedundantCallElimination(Function *currentFunction) {
    BasicBlock *entry = currentFunction->entry;

    clear_visited_flag(entry);

    HashSet *workList = HashSetInit();

    HashSetAdd(workList,entry);

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        block->visited = true;
        HashSetRemove(workList,block);

        HashMap *funcMap = HashMapInit(); //func -> hashmap(num(calculated by params) -> call node);

        InstNode *blockHead = block->head_node;
        InstNode *blockTail = block->tail_node;

        while(blockHead != blockTail){
            if(blockHead->inst->Opcode == Call){
                Value *calledFunction = ins_get_lhs(blockHead->inst);
                if(isSySYFunction(calledFunction)){
                    blockHead = get_next_inst(blockHead);
                    continue;
                }
                int paramNum = calledFunction->pdata->symtab_func_pdata.param_num;
                Type *paramTypes = calledFunction->pdata->symtab_func_pdata.param_type_lists;
                bool containAddress = false;
                for(int i = 0; i < paramNum; i++){
                    if(paramTypes[i].ID == AddressTyID){
                        containAddress = true;
                    }
                }

                //OK see if we have the same param with same function
                if(!containAddress && isTargetFunction(calledFunction)){
                    Vector *paramVector = VectorInit(paramNum);
                    InstNode *paramNode = get_prev_inst(blockHead);
                    while(paramNum > 0){
                        assert(paramNode->inst->Opcode == GIVE_PARAM);
                        Value *lhs = ins_get_lhs(paramNode->inst);
                        VectorPushBack(paramVector,lhs);
                        paramNum--;
                    }

                    unsigned paramValues = hash_values(paramVector);

                    //
                    HashMap *num2Node = NULL;
                    InstNode *preCall = NULL;
                    if(HashMapGet(funcMap, calledFunction) != NULL){
                        num2Node = HashMapGet(funcMap,calledFunction);
                        if(HashMapGet(num2Node,(void *)paramValues)){
                            preCall = HashMapGet(num2Node,(void *)paramValues);
                            Value *dest = ins_get_dest(blockHead->inst);
                            Value *oldCall = ins_get_dest(preCall->inst);
                            valueReplaceAll(dest,oldCall,currentFunction);

                            //delete This call & it's give params
                            InstNode *nextNode = get_next_inst(blockHead);
                            paramNum = calledFunction->pdata->symtab_func_pdata.param_num;
                            while(paramNum >= 0){
                                InstNode *prevNode = get_prev_inst(blockHead);
                                deleteIns(blockHead);
                                blockHead = prevNode;
                                paramNum--;
                            }
                            blockHead = nextNode;
                        }else{
                            HashMapPut(num2Node,(void *)paramValues,blockHead);
                            blockHead = get_next_inst(blockHead);
                        }
                    }else{
                        num2Node = HashMapInit();
                        HashMapPut(funcMap,calledFunction,num2Node);
                        HashMapPut(num2Node,(void *)paramValues,blockHead);
                        blockHead = get_next_inst(blockHead);
                    }
                }else{
                    blockHead = get_next_inst(blockHead);
                }
            }else{
                blockHead = get_next_inst(blockHead);
            }
        }

        HashMapFirst(funcMap);
        for(Pair *pair = HashMapNext(funcMap); pair != NULL; pair = HashMapNext(funcMap)){
            HashMap *restMap = pair->value;
            HashMapDeinit(restMap);
        }
        HashMapDeinit(funcMap);

        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
}