//
// Created by Valerian on 2023/5/31.
//

#include "sideeffect.h"
void sideEffect(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    InstNode *funcHead = entry->head_node;
    InstNode *funcTail = tail->tail_node;

    Value *function = funcHead->inst->user.use_list[0].Val;
    printf("funcName is %s\n",function->name);
    int paramNum = funcHead->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;
    printf("param Num is %d!\n",paramNum);
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
            //看看call的是哪个function
            Value *calledFunction = ins_get_lhs(funcHead->inst);
            //if is the same as its self
            if(calledFunction == function){
                //do nothing
            }else{
                //如果是库函数是否需要额外考虑 -> 懒得考虑了
                //if is not SySY function call do follows
                HashSetFirst(calledFunction->visitedObjects);
                for(Value *visitedObject = HashSetNext(calledFunction->visitedObjects); visitedObject != NULL; visitedObject = HashSetNext(calledFunction->visitedObjects)){
                    //copy visited objects

                    //TODO 现在是简单的全部拷贝过去了但是存在问题！！ copy 的时候也需要判断是不是子函数 visited有咩有涉及到自己传递的局部变量
                    HashSetAdd(function->visitedObjects,visitedObject);
                }
                function->containOutput |= calledFunction->containOutput;
                function->containInput |= calledFunction->containInput;
            }
        }
        funcHead = get_next_inst(funcHead);
    }

    //
    printf("visited object contain: ");
    HashSetFirst(function->visitedObjects);
    for(Value *visitedObject = HashSetNext(function->visitedObjects); visitedObject != NULL; visitedObject = HashSetNext(function->visitedObjects)){
        printf("%s ",visitedObject->name);
    }
    printf("\n");
}