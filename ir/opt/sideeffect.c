//
// Created by Valerian on 2023/5/31.
//

#include "sideeffect.h"

//需要构建调用关系图
//一个全局的map Value* -> GraphNode*
//理论上来说应该是一个树的结构
HashMap *callGraph;



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
            }else{
                //如果不是库函数
                HashSetFirst(calledFunction->visitedObjects);
                for(Value *visitedObject = HashSetNext(calledFunction->visitedObjects); visitedObject != NULL; visitedObject = HashSetNext(calledFunction->visitedObjects)){
                    HashSetAdd(function->visitedObjects,visitedObject);
                }
                function->containOutput |= calledFunction->containOutput;
                function->containInput |= calledFunction->containInput;


                //callGraph
//                CallGraphNode *callGraphNode = HashMapGet(callGraph,function);
//                CallGraphNode *calledGraphNode = HashMapGet(callGraph,calledFunction);
//
//                HashSetAdd(callGraphNode->children,calledGraphNode);
//                HashSetAdd(calledGraphNode->parents,callGraphNode);
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



CallGraphNode* createCallGraph(Value *function){
    CallGraphNode *callGraphNode = (CallGraphNode *)malloc(sizeof(CallGraphNode));
    callGraphNode->function = function;
    callGraphNode->visited = false;
    callGraphNode->children = HashSetInit();
    callGraphNode->parents = HashSetInit();
}

void buildCallGraphNode(Function *currentFunction){
    //找到function 的Value *然后去构建CallGraphNode
    BasicBlock *entry = currentFunction->entry;

    InstNode *funcHead = entry->head_node;
    Value *function = funcHead->inst->user.use_list[0].Val;
    CallGraphNode *callGraphNode = createCallGraph(function);

    HashMapPut(callGraph,function,callGraphNode);
}


void traversal(CallGraphNode *root){

}