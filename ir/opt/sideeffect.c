//
// Created by Valerian on 2023/5/31.
//

#include "sideeffect.h"

//需要构建调用关系图
//一个全局的map Value* -> GraphNode*
//理论上来说应该是一个树的结构
HashMap *callGraph;
HashSet *visitedCall;


//当全局数组传递参数的时候存在问题
//那我们默认如果传递了数组 并且使用了那就是算对内存有写操作不能优化
//修改visitedObject语意是我们 在本次函数以及子函数里面访问的全局变量
//意味着我们将不考虑参数的情况
//通过contain memoryOperations来进行判断
void sideEffect(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    InstNode *funcHead = entry->head_node;
    InstNode *funcTail = tail->tail_node;

    Value *function = funcHead->inst->user.use_list[0].Val;
    printf("funcName is %s\n",function->name);
    int paramNum = funcHead->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    Type *paramTypes = funcHead->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_type_lists;
    printf("param Num is %d!\n",paramNum);
    for(int i = 0; i < paramNum; i++){
        if(paramTypes[i].ID == AddressTyID){
            function->containMemoryOperations = true;
        }
    }

    while(funcHead != funcTail){
        if(funcHead->inst->Opcode == Load){
            //如果Load的是globalVar
            Value *loadPlace = ins_get_lhs(funcHead->inst);
            if(isGlobalVar(loadPlace)){
                HashSetAdd(function->visitedObjects,loadPlace);
            }
        }else if(funcHead->inst->Opcode == Store){
            Value *storedPalace = ins_get_rhs(funcHead->inst);
            if(isGlobalVar(storedPalace)){
                HashSetAdd(function->visitedObjects,storedPalace);
            }
        }else if(funcHead->inst->Opcode == GEP){
            Value *gepValue = ins_get_lhs(funcHead->inst);
            if(isGlobalArray(gepValue)){
                HashSetAdd(function->visitedObjects,gepValue);
            }
        }else if(funcHead->inst->Opcode == Call){
            //同时我们构建callGraphNode



            //看看call的是哪个function
            Value *calledFunction = ins_get_lhs(funcHead->inst);
            //if is the same as its self
            if(calledFunction == function){
                //do nothing

                //递归函数
                //child是自己 parent是否需要是自己呢？
//                CallGraphNode *callGraphNode = HashMapGet(callGraph,function);
//                HashSetAdd(callGraphNode->children,callGraphNode);

            }else if(isInputFunction(calledFunction)) {
                //如果是库函数
                function->containInput = true;
            }else if(isOutputFunction(calledFunction)) {
                function->containOutput = true;
            }else if(isTimeFunction(calledFunction)){
                function->containTime = true;
            }else{
                //如果不是input / output / time
                HashSetFirst(calledFunction->visitedObjects);
                for(Value *visitedObject = HashSetNext(calledFunction->visitedObjects); visitedObject != NULL; visitedObject = HashSetNext(calledFunction->visitedObjects)){
                    HashSetAdd(function->visitedObjects,visitedObject);
                }
                function->containOutput |= calledFunction->containOutput;
                function->containInput |= calledFunction->containInput;


                printf("called function name is %s\n",calledFunction->name);

                //callGraph
                CallGraphNode *callGraphNode = HashMapGet(callGraph,function);
                CallGraphNode *calledGraphNode = HashMapGet(callGraph,calledFunction);

                HashSetAdd(callGraphNode->children,calledGraphNode);
                HashSetAdd(calledGraphNode->parents,callGraphNode);
            }
        }
        funcHead = get_next_inst(funcHead);
    }

    //
    printf("\33[1;31mvisited object contain:\33[0m ");
    HashSetFirst(function->visitedObjects);
    for(Value *visitedObject = HashSetNext(function->visitedObjects); visitedObject != NULL; visitedObject = HashSetNext(function->visitedObjects)){
        printf("%s ",visitedObject->name);
    }
    printf("\n");

    //memoryOperations 对应了全局变量 和 传递的数组
    if(HashSetSize(function->visitedObjects) > 0){
        function->containMemoryOperations = true;
    }

    printf("function contain input: %d",function->containInput);
    printf(" contain output: %d",function->containOutput);
    printf(" contain memoryOperations: %d\n",function->containMemoryOperations);
}

CallGraphNode* createCallGraph(Function *function){
    CallGraphNode *callGraphNode = (CallGraphNode *)malloc(sizeof(CallGraphNode));
    callGraphNode->function = function;
    callGraphNode->visited = false;
    callGraphNode->children = HashSetInit();
    callGraphNode->parents = HashSetInit();
    return callGraphNode;
}

void buildCallGraphNode(Function *currentFunction){
    //找到function 的Value *然后去构建CallGraphNode
    BasicBlock *entry = currentFunction->entry;

    InstNode *funcHead = entry->head_node;
    Value *function = funcHead->inst->user.use_list[0].Val;
    CallGraphNode *callGraphNode = createCallGraph(currentFunction);

    HashMapPut(callGraph,function,callGraphNode);
}


bool isRemoveAble(Value *function){
    if(function->containOutput || function->containInput || function->containMemoryOperations || function->containTime){
        return false;
    }
    return true;
}


//logical wrong
//only for specific test cases (sad)
void traversal(CallGraphNode *node){
    //看看
    //判断里面是否存在useless call
    Function *func = node->function;
    BasicBlock *entry = func->entry;
    Value *curFunc = entry->head_node->inst->user.use_list[0].Val;

    BasicBlock *tail = func->tail;
    InstNode *callNode = entry->head_node;
    InstNode *tailNode = tail->tail_node;
    while(callNode != tailNode){
        if(callNode->inst->Opcode == Call){
            //

            Value *dest = ins_get_dest(callNode->inst);

            //SySY库函数不能进行简化
            //常见的eabi库、系统库函数不能简化
            Value *called = ins_get_lhs(callNode->inst);
            Type returnType = called->pdata->symtab_func_pdata.return_type;
            //and it can't be return void!!
            if(!isSySYFunction(called) && (returnType.ID != VoidTyID)){

                //OK let's see the dest uses
                //
                Use *uses = dest->use_list;
                if(uses == NULL) {
                    printf("function %s calls %s return value is not used\n", curFunc->name, called->name);

                    //OK we can remove the called function
                    //let's first get the function
                    CallGraphNode *calledGraphNode = HashMapGet(callGraph,called);

                    assert(calledGraphNode != NULL);

                    Function *calledFunction = calledGraphNode->function;

                    //find the return value of function

                    BasicBlock *calledTail = calledFunction->tail;

                    InstNode *calledTailNode = calledTail->tail_node;

                    while(calledTailNode->inst->Opcode != Return){

                        calledTailNode = get_prev_inst(calledTailNode);
                    }

                    //
                    assert(calledTailNode->inst->Opcode == Return);

                    Value *calledReturnValue = ins_get_lhs(calledTailNode->inst);
                    assert(calledReturnValue != NULL);

                    //we don't want it to be a imm

                    //according to the test cases if calledReturnValue is produced by another call
                    //and this another call doesn't have time / input / output / memory operation
                    //we can remove this call and remove

                    //TODO plz check this break
                    if(isImm(calledReturnValue)){
                        break;
                    }

                    Instruction *ins = (Instruction *)calledReturnValue;
                    BasicBlock *block = ins->Parent; // this block belongs to the mid call


                    //并且不能存在于已经删除了的函数函数调用里面
                    if(ins->Opcode == Call){
                        //only consider this situation
                        //remove this call and remove the function value's
                        Value *nextCall = ins_get_lhs(ins);
                        printf("nextCall %s\n",nextCall->name);

                        //由于相当于我们要修改中间的这个函数 我们需要保证调用这个函数都被ignored了
                        Use *midUses = called->use_list;

                        printf("midCall %s\n",called->name);


                        //ok we should make sure that all uses of mid is ignored
                        bool OK = true;
                        while(midUses != NULL){
                            User *midUsers = midUses->Parent;
                            //see if all not used
                            Value *midUsesValue = &midUsers->value;
                            //
                            if(midUsesValue->use_list != NULL){
                                OK = false;
                                break;
                            }
                            midUses = midUses->Next;
                        }

                        //next Call can't have side effect
                        //and we don't want to visit the same call twice
                        if(OK && isRemoveAble(nextCall) && !HashSetFind(visitedCall,called)){
                            HashSetAdd(visitedCall, called);

                            InstNode *curCall = findNode(block,ins);


                            //and we should also make sure that this call is only used in return value
                            Value *curCallDest = ins_get_dest(curCall->inst);


                            bool onlyUsed = true;
                            Use *curCallUses = curCallDest->use_list;
                            while(curCallUses != NULL){
                                User *user = curCallUses->Parent;

                                Instruction *curCallUserIns = (Instruction *) user;

                                if(curCallUserIns != calledTailNode->inst){
                                    printf("other id %d\n",curCallUserIns->i);
                                    //can be
                                    onlyUsed = false;
                                    break;
                                }
                                curCallUses = curCallUses->Next;
                            }

                            if(onlyUsed){
                                //remove this curCall
                                //change the return value to be 0
                                deleteIns(curCall);


                                //remove GiveParams for the next call
                                int paramNum = nextCall->pdata->symtab_func_pdata.param_num;

                                //
                                InstNode *giveParam = curCall;
                                while(paramNum >= 0){
                                    InstNode *tempNode = get_prev_inst(giveParam);
                                    deleteIns(giveParam);
                                    giveParam = tempNode;
                                    paramNum--;
                                }


                                //remove Give Param Nodes

                                //find all the give param
                                printf("param num : %d\n",paramNum);

                                printf("once !\n");

                                //change the return value
                                switch(calledReturnValue->VTy->ID){
                                    case Var_INT:
                                        calledReturnValue->VTy->ID = Int;
                                        calledReturnValue->pdata->var_pdata.iVal = 0;
                                        break;
                                    case Var_FLOAT:
                                        calledReturnValue->VTy->ID = Float;
                                        calledReturnValue->pdata->var_pdata.fVal = 0.0;
                                        break;
                                    default:
                                        assert(false);
                                }
                            }
                        }
                    }else{
                        //我们只需要检查return 的 value 是不是也只被return use了 如果是并且这个midFunction的User同时也是被所有的ignored
                        //ins->生成return value的ins
                        printf("another case called function is %s\n",called->name);


                        //check if all the called return value is ignored
                        Use *calledUses = called->use_list;

                        bool OK = true;
                        while(calledUses != NULL){
                            User *user = calledUses->Parent;
                            Value *userValue = &user->value;
                            if(userValue->use_list != NULL){
                                OK = false;
                            }
                            calledUses = calledUses->Next;
                        }
                        if(OK && !HashSetFind(visitedCall,called)){
                            HashSetAdd(visitedCall,called);

                            Value *insDest = ins_get_dest(ins);

                            Use *insDestUses = insDest->use_list;

                            bool onlyUse = true;
                            while(insDestUses != NULL){
                                User *insDestUser = insDestUses->Parent;
                                //only used by return
                                if(calledTailNode->inst != (Instruction *)insDestUser){
                                    onlyUse = false;
                                }
                                insDestUses = insDestUses->Next;
                            }



                            if(onlyUse){
                                InstNode *insNode = findNode(block,ins);
                                //if only used let's remove this node
                                deleteIns(insNode);

                                //let's modify the return value

                                switch (calledReturnValue->VTy->ID) {
                                    case Var_INT:
                                        calledReturnValue->VTy->ID = Int;
                                        calledReturnValue->pdata->var_pdata.iVal = 0;
                                        break;
                                    case Var_FLOAT:
                                        calledReturnValue->VTy->ID = Float;
                                        calledReturnValue->pdata->var_pdata.fVal = 0.0;
                                        break;
                                    default:
                                        assert(false);
                                }
                            }
                        }
                    }
                }
            }
        }
        callNode = get_next_inst(callNode);
    }

    //after this we‘d better call rename phrase

    renameVariables(func);

    HashSetFirst(node->children);
    for(CallGraphNode *child = HashSetNext(node->children); child != NULL; child = HashSetNext(node->children)){
        traversal(child);
    }
}



void travel(){
    //找到main函数
    HashMapFirst(callGraph);
    for(Pair *value2graph = HashMapNext(callGraph); value2graph != NULL; value2graph = HashMapNext(callGraph)){
        Value *function = value2graph->key;
        printf("function: %s\n",function->name);
        if(strcmp(function->name,"main") == 0){
            //do optimization pass
            CallGraphNode *main = value2graph->value;
            traversal(main);
            break;
        }
    }
}