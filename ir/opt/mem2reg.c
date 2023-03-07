//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"

InstNode* new_phi(Value *val){
    Instruction *phiIns = ins_new(0);
    phiIns->Opcode = Phi;

    InstNode *phiNode = new_inst_node(phiIns);
    return phiNode;
}

void insert_phi(BasicBlock *block,Value *val){
    //头指令
    InstNode *instNode = block->head_node;

    //注意第一条语句是label 所以我们一定要插入到label的后面
    //

    // 这个好像不存在这种情况
    //如果当前的基本块内已经存在了对应phi指令的话那我们就不需要再次添加了


    //插入空的phi函数
    InstNode *phiInstNode = new_phi(val);
    ins_insert_after(phiInstNode,instNode);

    // 让这个语句属于Basicblock
    phiInstNode->inst->Parent = block;
}

void mem2reg(Function *currentFunction){
    //对于现在的Function初始化
    currentFunction->loadSet = HashMapInit();
    currentFunction->storeSet = HashMapInit();

    HashSet *allocas = HashSetInit();

    BasicBlock *entry = currentFunction->head;
    BasicBlock *end = currentFunction->tail;


    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    InstNode *curNode = head;
    // 第一次循环去扫描一遍 记录loadSet 和 storeSet
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

        if(curNode->inst->Opcode == Alloca){
            //需要存一下
            HashSetAdd();
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

//

    curNode = head;

    //做一些基本的优化

    // 删除无用的alloca
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

    //mem2reg的主要过程
    //如果只有一个store的话 被store支配的load转换
    //Function里面去找
    HashMapFirst(currentFunction->storeSet);
    //对里面所有的variable进行查找
    for(Pair *pair = HashMapNext(currentFunction->storeSet); pair != NULL; pair = HashMapNext(currentFunction->storeSet)){
        //先看一下我们的这个是否是正确的
        Value *val = pair->key;
        HashSet *storeSet = pair->value;  //这个value对应的所有defBlocks
        HashSetFirst(storeSet);

        printf("value : %s store(defBlocks) : ", val->name);
        for(BasicBlock *key = HashSetNext(storeSet); key != NULL; key = HashSetNext(storeSet)){
            printf("b%d ",key->id);
        }

        printf("\n");

        //place phi

        HashSet *phiBlocks = HashSetInit();
        while(HashSetSize(storeSet) != 0){
            HashSetFirst(storeSet);
            BasicBlock *block = HashSetNext(storeSet);
            HashSetRemove(storeSet,block);
            assert(block != nullptr);
            HashSet *df = block->df;
            HashSetFirst(df);
            for(BasicBlock *key = HashSetNext(df); key != nullptr; key = HashSetNext(df)){
                if(!HashSetFind(phiBlocks,key)){
                    //在key上面放置phi函数
                    insert_phi(key,NULL);
                    HashSetAdd(phiBlocks,key);
                    if(!HashSetFind(storeSet,key)){
                        HashSetAdd(storeSet,key);
                    }
                }
            }
            printf("1");
        }
        HashSetDeinit(phiBlocks);
    }



    //变量重新命名
    DomTreeNode *root = currentFunction->root;
    //foreach v : Variable do
    // v.reachingDef <- Undefined

    //利用一个HashMap alloca的value*  ->  到的这里的value*


    // 还需要栈吗？？
    //(a) A hash table of IncomingVals which is a map from a alloca to its most recent name is created// Most recent name of each alloca is an undef value to start with
    HashMap *IncomingVals = HashMapInit();

    //

    // 对树做一个DFS
    //dfsTravelDomTree(root)


    // OK 记得释放内存哦
}

void dfsTravelDomTree(DomTreeNode *node){
    if(HashSetSize(node->children) == 0){
        return;
    }

    // 先根处理
    BasicBlock *block = node->block;
    InstNode *head = block->head_node;
    InstNode *tail = block->tail_node;

    InstNode *curr = head;

    while(curr != get_next_inst(tail)){




        curr = get_next_inst(curr);
    }





    // 递归遍历
    HashSetFirst(node->children);
    for(DomTreeNode *key = HashSetNext(node->children); key != nullptr; key = HashSetNext(node->children)){
        dfsTravelDomTree(key);
    }

    //
}