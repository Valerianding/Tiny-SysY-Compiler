//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"

void mem2reg(Function *currentFunction){
    //对于现在的Function初始化
    currentFunction->loadSet = HashMapInit();
    currentFunction->storeSet = HashMapInit();

    BasicBlock *entry = currentFunction->head;
    BasicBlock *end = currentFunction->tail;


    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    InstNode *curNode = head;
    //(Value*)instruction -> HashSet
    while(curNode != get_next_inst(tail)) {
        BasicBlock *parent = curNode->inst->Parent;

        if (curNode->inst->Opcode == Load) {
            Value *loadValue = curNode->inst->user.use_list[0].Val;
            if (HashMapContain(currentFunction->loadSet, loadValue)) {
                //不是第一次加入
                HashSet *loadSet = HashMapGet(currentFunction->loadSet, loadValue);
                //那么就将现在的BasicBlock
                if (!HashSetFind(loadSet, parent)) {
                    //如果存在那么就意味着存在两次load 我们目前仅保留一次 如果不存在我们再加入进去
                    HashSetAdd(loadSet, parent);
                }
            } else {
                HashSet *loadSet = HashSetInit();
                HashSetAdd(loadSet, parent);
                HashMapPut(currentFunction->loadSet, loadValue, loadSet);
            }
        }


        if (curNode->inst->Opcode == Store) {
            //一定对应的是第二个
            Value *storeValue = curNode->inst->user.use_list[1].Val;
            if (HashMapContain(currentFunction->storeSet, storeValue)) {
                //如果存在的话那么之前的HashSet也一定存在
                HashSet *storeSet = HashMapGet(currentFunction->storeSet, storeValue);
                if (!HashSetFind(storeSet, parent)) {
                    HashSetAdd(storeSet, parent);
                }
            } else {
                HashSet *storeSet = HashSetInit();
                HashSetAdd(storeSet, parent);
                HashMapPut(currentFunction->storeSet, storeValue, storeSet);
            }
        }
        curNode = get_next_inst(curNode);
    }
    curNode = head;

    //做一些基本的优化

    while(curNode != get_next_inst(tail)){
        //如果alloca没有load就删除这个alloca
        //首先找到对应的value
        if(curNode->inst->Opcode == Alloca && !HashMapContain(currentFunction->loadSet,(Value*)curNode->inst)){
            //不存在loadSet之中不存在这个value
            //删除这个instruction
            InstNode *prev = get_prev_inst(curNode);
            InstNode *next = get_next_inst(curNode);

            //直接删除alloca
            prev->list.next = &next->list;
            next->list.prev = &prev->list;
        }
        curNode = get_next_inst(curNode);
    }


    //如果只有一个store的话 被store支配的load转换
    HashMapFirst(currentFunction->storeSet);
    for(Pair *pair = HashMapNext(currentFunction->storeSet); pair != NULL; pair = HashMapNext(currentFunction->storeSet)){
        Value* value = pair->value;
        Instruction *ins = (Instruction*)value;
        //将value转换成instruction去找对应的
        Value* storeValue = (Value*)(ins->user.use_list[1].Val);
        HashSet *storeSet = pair->value;
        if(HashSetSize(storeSet) == 1){
            //找到对应的基本块 求出其对应的
            HashSetFirst(storeSet);
            BasicBlock *block = HashSetNext(storeSet);
            //找到对应支配基本块
            HashSet *dom = block->dom;
            HashSetFirst(dom);
            for(BasicBlock *key = HashSetNext(dom); key != NULL; key = HashSetNext(dom)){
                //如果这个基本块有load
                InstNode *keyBlockHead = key->head_node;
                InstNode *keyBlockTail = key->tail_node;

                InstNode *keyBlockCur = keyBlockHead;
                while(keyBlockCur != keyBlockTail){
                    //找到对应的load了
                    if(keyBlockCur->inst->Opcode == Load && (Value*)keyBlockCur->inst == value){
                        //OK现在我要将load直接变成赋值 注意这里需要做添加value的use_list
                        //并且注意我们不能直接new一个instruction 所以目前我们先暂时的更改Opcode和value就行
                        //应该错误
                        value_replace(&keyBlockCur->inst->user.use_list[0].Val,storeValue,&keyBlockCur->inst->user.use_list[0]);
                    }
                    keyBlockCur = get_next_inst(keyBlockCur);
                }
            }
            //如果支配基本块中有相应的load的话

            //替换
        }
    }
    //对于每一个store 支配的load 替换！！

    //去hashmap中找对应的store语句

    //如果只有一个store 那么将所有store支配的load直接换成对应的值

    //如果支配的话那么就写上去

    //如果不支配的话就
}