//
// Created by Valerian on 2023/5/28.
//

#include "memlvn.h"


//TODO 还没有考虑传递是参数数组的情况！！
Array *arrayCreate(Value *array, unsigned int index){
    Array *arr = (Array*)malloc(sizeof(Array));
    arr->array = array;
    arr->index = index;
    return arr;
}

StoreInfo *storeInfoCreate(InstNode *instNode, Value *stored){
    StoreInfo *storeInfo = (StoreInfo *) malloc(sizeof(StoreInfo));
    storeInfo->storeInstruction = instNode;
    storeInfo->storedValue = stored;
    return storeInfo;
}

void memlvn(Function *current){
    BasicBlock *entry = current->entry;

    HashSet *arrays = HashSetInit();
    InstNode *entryHeadNode = entry->head_node;
    InstNode *entryTailNode = entry->tail_node;
    InstNode *entryCurrNode = entryHeadNode;
    while(entryCurrNode != entryTailNode){
        if(entryCurrNode->inst->Opcode == Alloca){
            Value *dest = ins_get_dest(entryCurrNode->inst);
            printf("arrays add %s\n",dest->name);
            HashSetAdd(arrays,dest);
        }
        entryCurrNode = get_next_inst(entryCurrNode);
    }

    clear_visited_flag(entry);

    HashSet *workList = HashSetInit();
    HashSetAdd(workList,entry);
    BasicBlock *nextBlock = NULL;
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        nextBlock = HashSetNext(workList);
        HashSetRemove(workList,nextBlock);
        assert(nextBlock->visited == false);
        nextBlock->visited = true;

        //
        mem_lvn(nextBlock,arrays,current);
        if(nextBlock->true_block && nextBlock->true_block->visited == false){
            HashSetAdd(workList,nextBlock->true_block);
        }
        if(nextBlock->false_block && nextBlock->false_block->visited == false){
            HashSetAdd(workList,nextBlock->false_block);
        }
    }
}

void mem_lvn(BasicBlock *block, HashSet *arrays,Function *currentFunction){
    InstNode *funcHead = currentFunction->entry->head_node;
    assert(funcHead->inst->Opcode == FunBegin);
    int paramNum = funcHead->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    printf("paramNum is %d\n",paramNum);
    HashMap *memory = HashMapInit(); //Array -> StoreInfo

    InstNode *head = block->head_node;
    InstNode *tail = block->tail_node;

    InstNode *currNode = head;
    while(currNode != tail){
        if(currNode->inst->Opcode == Load){
            printf("case load!\n");
            //找到第二个value
            Value *loadPlace = ins_get_lhs(currNode->inst);
            //TODO 如果是数组就走这边 如果不是数组我们需要单独处理
            if(!isGlobalVar(loadPlace)){
                Instruction *gepInstruction = (Instruction*)loadPlace;
                if(gepInstruction->Opcode == bitcast){

                }
                Vector *indexVector = VectorInit(10);
                //lhs 是位置 rhs是偏移量
                Value *gepValue = ins_get_lhs(gepInstruction);
                Value *gepIndex = ins_get_rhs(gepInstruction);
                printf("gepValue %s gepIndex %s\n",gepValue->name,gepIndex->name);
                //倒序压入栈当中
                VectorPushBack(indexVector,gepIndex);

                //TODO 这里是第一个gepValue
                while(!HashSetFind(arrays,gepValue) && !isGlobalArray(gepValue) && !isParam(gepValue,paramNum)){
                    gepInstruction = (Instruction*)gepValue;
                    if(gepInstruction->Opcode == bitcast){
                       gepValue = ins_get_lhs(gepInstruction);
                    }else{
                        gepIndex = ins_get_rhs(gepInstruction);
                        VectorPushBack(indexVector,gepIndex);
                        gepValue = ins_get_lhs(gepInstruction);
                    }
                }

                unsigned long int hash_value = hash_values(indexVector);
                printf("load %s index is %d\n",gepValue->name,hash_value);

                Value *fromArray = gepValue;
                Value *loadValue = ins_get_dest(currNode->inst);

                bool exist = false;
                HashMapFirst(memory);
                for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                    //看看存不存在key
                    Array *array = pair->key;
                    if(array->array == fromArray && array->index == hash_value){
                        //存在 我们可以直接找到这个load出来的是多少了！！
                        exist = true;
                        StoreInfo *storeInfo = pair->value;
                        Value *replace = storeInfo->storedValue;
                        assert(replace != NULL);
                        valueReplaceAll(loadValue,replace,currentFunction);
                        printf("replace !\n");
                        break;
                    }
                }
                if(exist){
                    InstNode *nextNode = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = nextNode;
                }else{
                    currNode = get_next_inst(currNode);
                }
                VectorDeinit(indexVector);
            }else{
                //如果是那么同样的只是我们不需要去找了 storeInfo 里面index设置为0就可以了
                Value *loadValue = ins_get_dest(currNode->inst);
                bool exist = false;
                HashMapFirst(memory);
                for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                    Array *array = pair->key;
                    if(array->array == loadPlace && array->index == 0){
                        //存在load
                        //取出store
                        exist = true;
                        StoreInfo *storeInfo = pair->value;
                        Value *replace = storeInfo->storedValue;
                        assert(replace != NULL);
                        valueReplaceAll(loadValue,replace,currentFunction);
                        printf("replace !\n");
                        break;
                    }
                }
                if(exist){
                    InstNode *nextNode = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = nextNode;
                }else{
                    currNode = get_next_inst(currNode);
                }
            }
        }else if(currNode->inst->Opcode == Store){
            printf("case store!\n");
            Value *stored = ins_get_lhs(currNode->inst);
            Value *storePlace = ins_get_rhs(currNode->inst);
            if(!isGlobalVar(storePlace)){
                Vector *indexVector = VectorInit(10);
                Instruction *gepInstruction = (Instruction*)storePlace;
                Value *gepValue = ins_get_lhs(gepInstruction);
                Value *gepIndex = ins_get_rhs(gepInstruction);
                VectorPushBack(indexVector,gepIndex);

                printf("gepValue %s gepIndex %s\n",gepValue->name,gepIndex->name);
                while(!HashSetFind(arrays,gepValue) && !isGlobalArray(gepValue) && !isParam(gepValue,paramNum)){
                    gepInstruction = (Instruction*)gepValue;
                    //如果遇到了bitcast语句怎么办
                    if(gepInstruction->Opcode == bitcast){
                        //需要找到LHS就行并且gepindex为NULL;
                        //actually it is bitcast instruction
                        gepValue = ins_get_lhs(gepInstruction);
                    }else{
                        gepValue = ins_get_lhs(gepInstruction);
                        gepIndex = ins_get_rhs(gepInstruction);
                        VectorPushBack(indexVector,gepIndex);
                        printf("gepValue %s gepIndex %s\n",gepValue->name,gepIndex->name);
                    }
                }

                printf("indexVectorSize is %d\n", VectorSize(indexVector));
                unsigned long int hash_value = hash_values(indexVector);
                printf("store %s index is %d\n",gepValue->name,hash_value);
                VectorDeinit(indexVector);


                Value *fromArray = gepValue;
                //
                bool exist = false;
                HashMapFirst(memory);
                for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                    Array *array = pair->key;
                    if(array->array == fromArray && array->index == hash_value){
                        //之前存在
                        exist = true;
                        //remove
                        StoreInfo *storeInfo = pair->value;
                        deleteIns(storeInfo->storeInstruction);
                        free(storeInfo);

                        StoreInfo *newInfo = storeInfoCreate(currNode,stored);
                        pair->value = newInfo;
                    }
                }

                if(!exist){
                    Array *array = arrayCreate(fromArray,hash_value);
                    //如果不存在就new
                    StoreInfo *newInfo = storeInfoCreate(currNode,stored);
                    HashMapPut(memory,array,newInfo);
                }
                currNode = get_next_inst(currNode);
            }else{
                bool exist = false;
                HashMapFirst(memory);
                for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                    Array *array = pair->key;
                    if(array->array == storePlace && array->index == 0){
                        exist = true;
                        StoreInfo *storeInfo = pair->value;
                        deleteIns(storeInfo->storeInstruction);

                        StoreInfo *newInfo = storeInfoCreate(currNode,stored);
                        pair->value = newInfo;
                    }
                }
                if(!exist){
                    Array *array = arrayCreate(storePlace,0);
                    StoreInfo *newInfo = storeInfoCreate(currNode,stored);
                    HashMapPut(memory,array,newInfo);
                }
                currNode = get_next_inst(currNode);
            }
        }else if(currNode->inst->Opcode == Call){
            //assert(false);

            Value *functionValue = ins_get_lhs(currNode->inst);

            //存在两种情况
            //第一种情况是调用的函数（及其再下层的被调用的函数）和现在的函数访问了相同的全局变量，
            //那么我们需要清除对于全局变量的存储信息
            //第二种情况是给调用函数传递了参数那么默认被调函数修改了数组，我们需要对传递的数组进行清除

            //case 1:
            HashSetFirst(functionValue->visitedObjects);
            for(Value *visited = HashSetNext(functionValue->visitedObjects); visited != NULL; visited = HashSetNext(functionValue->visitedObjects)){
                printf("remove %s info\n",visited->name);
                HashMapFirst(memory);
                for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                    Array *array = pair->key;
                    StoreInfo *storeInfo = pair->value;
                    if(array->array == visited){
                        //this storeInfo in memory is useless
                        HashMapRemove(memory,array);
                        free(array);
                        free(storeInfo);
                    }
                }
            }


            //case 2:
            int calledParamNum = functionValue->pdata->symtab_func_pdata.param_num;
            InstNode *paramNode = currNode;
            Value *paramValue = NULL;
            //往前找
            while(calledParamNum--){
                paramNode = get_prev_inst(paramNode);
                assert(paramNode->inst->Opcode == GIVE_PARAM);
                paramValue = ins_get_lhs(paramNode->inst);
                //看看give Param的Value是啥！
                //如果是地址的话 找到对应的数组 清除memory里面对应数组的信息
                //不是数组我们都可以不用管吧
                Instruction *ins = NULL;
                if(isAddress(paramValue)){
                    while(!HashSetFind(arrays,paramValue) && !isGlobalArray(paramValue) && !isParam(paramValue,paramNum)){
                        ins = (Instruction*)paramValue;
                        paramValue = ins_get_lhs(ins);
                    }

                    printf("find paramValue array %s",paramValue->name);
                    HashMapFirst(memory);
                    for(Pair *pair = HashMapNext(memory); pair != NULL; pair = HashMapNext(memory)){
                        Array *array = pair->key;
                        StoreInfo *storeInfo = pair->value;
                        if(array->array == paramValue){
                            HashMapRemove(memory,array);
                            free(array);
                            free(storeInfo);
                        }
                    }
                }
            }
            currNode = get_next_inst(currNode);
        }else{
            currNode = get_next_inst(currNode);
        }
    }
}