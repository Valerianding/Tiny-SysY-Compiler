//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"

InstNode* new_phi(Value *val){
    Instruction *phiIns = ins_new(0);
    phiIns->Opcode = Phi;
    phiIns->user.value.pdata->pairSet = NULL;
    InstNode *phiNode = new_inst_node(phiIns);
    //做一个映射 记录现在对应的是哪个alloca
    phiNode->inst->user.value.alias = val;
    // 添加一个默认的名字
    phiIns->user.value.name = (char *)malloc(sizeof(char) * 7);
    strcpy(phiIns->user.value.name,"%phi");
    return phiNode;
}

pair *createPhiInfo(BasicBlock *prev, Value *val){
    pair *phiInfo = (pair*)malloc(sizeof(pair));
    phiInfo->define = val;
    phiInfo->from = prev;
    return phiInfo;
}

void insert_phi(BasicBlock *block,Value *val){
    //头指令
    InstNode *instNode = block->head_node;
    //插入空的phi函数
    InstNode *phiInstNode = new_phi(val);
    ins_insert_after(phiInstNode,instNode);
    // 让这个语句属于BasicBlock
    phiInstNode->inst->Parent = block;
}

void insertPhiInfo(InstNode *ins,pair *phiInfo){
    if(ins->inst->user.value.pdata->pairSet == nullptr){
        ins->inst->user.value.pdata->pairSet = HashSetInit();
    }
    assert(ins->inst->user.value.pdata->pairSet != NULL);
    assert(phiInfo != NULL);
    HashSetAdd(ins->inst->user.value.pdata->pairSet,phiInfo);
}

void mem2reg(Function *currentFunction){
    //对于现在的Function初始化
    currentFunction->loadSet = HashMapInit();
    currentFunction->storeSet = HashMapInit();

    BasicBlock *entry = currentFunction->entry;
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
            if(!isArray(loadValue) && !isGlobalVar(loadValue)){
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
        }

        if (curNode->inst->Opcode == Store) {
            //一定对应的是第二个
            Value *storeValue = curNode->inst->user.use_list[1].Val;
            if(!isGlobalVar(storeValue) && !isArray(storeValue)){
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
        }
        curNode = get_next_inst(curNode);
    }

    // 打印一下现在HashSet
    HashMapFirst(currentFunction->loadSet);
    for(Pair *pair = HashMapNext(currentFunction->loadSet); pair != NULL; pair = HashMapNext(currentFunction->loadSet)){
        Value *val = pair->key;
        BasicBlock *block = pair->value;
        printf("value %s : load in block : b%d\n", val->name,block->id);
    }
    curNode = head;
    //做一些基本的优化
    // 删除无用的alloca
    while(curNode != get_next_inst(tail)){
        //如果alloca没有load就删除这个alloca
        //首先找到对应的value
        printf("assert alloca ing \n");
        if(curNode->inst->Opcode == Alloca && curNode->inst->user.value.use_list == NULL){
            //不存在loadSet之中不存在这个value
            //删除这个instruction
            InstNode *next = get_next_inst(curNode);
            delete_inst(curNode);
            curNode = next;
        }else if(curNode->inst->Opcode == Store) {
            Value *storeValue = ins_get_rhs(curNode->inst);
            if(!isGlobalVar(storeValue) && !isArray(storeValue) && !HashMapContain(currentFunction->loadSet, storeValue)){
                InstNode *next = get_next_inst(curNode);
                delete_inst(curNode);
                curNode = next;
            }else{
                curNode = get_next_inst(curNode);
            }
        }else{
            curNode = get_next_inst(curNode);
        }
    }

    //TODO 做一些剪枝来保证phi函数插入更加简介

    //mem2reg的主要过程
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
            for(BasicBlock *key = HashSetNext(df); key != nullptr; key = HashSetNext(df)) {
                if (!HashSetFind(phiBlocks, key)) {
                    //在key上面放置phi函数
                    insert_phi(key, val);
                    HashSetAdd(phiBlocks, key);
                    if (!HashSetFind(storeSet, key)) {
                        HashSetAdd(storeSet, key);
                    }
                }
            }
        }
        HashSetDeinit(phiBlocks);
    }



    printf("after insert phi function!\n");
    //变量重新命名
    // DomTreeNode *root = currentFunction->root;
    assert(entry->domTreeNode == currentFunction->root);
    DomTreeNode *root = entry->domTreeNode;

    HashMap *IncomingVals = HashMapInit();

    curNode = entry->head_node;
    while(curNode != get_next_inst(entry->tail_node)){
        //printf("curNode id is : %d",curNode->inst->i);
        if(curNode->inst->Opcode == Alloca){
            //每一个alloca都对应一个stack
            stack *allocaStack = stackInit();
            assert(allocaStack != nullptr);
            HashMapPut(IncomingVals,&(curNode->inst->user.value),allocaStack);
            assert(HashMapContain(IncomingVals,&(curNode->inst->user.value)));
        }
        curNode = get_next_inst(curNode);
    }

    printf("in rename pass!\n");

    //变量重命名 如果都没有alloc那么就不需要了
    if(HashMapSize(IncomingVals) != 0){
        dfsTravelDomTree(root,IncomingVals);
    }

    printf("after rename !\n");
    // delete load 和 store
    deleteLoadStore(currentFunction);


    printf("after delete alloca load store\n");
    // 让LLVM IR符合标准
    renameVariabels(currentFunction);


    // OK 记得释放内存哦
    //先释放栈的内存再释放HashMap的内存
    HashMapFirst(IncomingVals);
    for(Pair *pair = HashMapNext(IncomingVals); pair != NULL; pair = HashMapNext(IncomingVals)){
        stack *allocStack = pair->value;
        stackDeinit(allocStack);
    }
    HashMapDeinit(IncomingVals);

    // Function中的HashSet也不需要了
    HashMapDeinit(currentFunction->storeSet);
    HashMapDeinit(currentFunction->loadSet);
}

void insertCopies(BasicBlock *block,Value *dest,Value *src){
    InstNode *tailIns = block->tail_node;
    assert(tailIns != NULL);
    InstNode *copyIns = newCopyOperation(dest,src);
    printf("new a copy Ins!\n");
    assert(copyIns != NULL);
    copyIns->inst->Parent = block;
    ins_insert_before(copyIns,tailIns);
}

void dfsTravelDomTree(DomTreeNode *node,HashMap *IncomingVals){

    printf("in rename phrase : block : %d\n", node->block->id);
    assert(HashMapSize(IncomingVals) != 0);

    printf("11111 \n");
    // 先根处理
    BasicBlock *block = node->block;
    InstNode *head = block->head_node;
    InstNode *tail = block->tail_node;

    InstNode *curr = head;


    //记录一下每一个alloc的压栈次数
    HashMap *countDefine = HashMapInit();
    HashMapFirst(IncomingVals);
    for(Pair *pair = HashMapNext(IncomingVals); pair != NULL; pair = HashMapNext(IncomingVals)){
        int *defineTimes = (int *)malloc(sizeof(int));
        //默认初始化为零
        *(defineTimes) = 0;
        Value *alloc = (Value*)pair->key;
        HashMapPut(countDefine,alloc,defineTimes);
    }

    if(GlobalIncomingVal != nullptr){
        // 同样的我们也要记录全局变量压栈的次数
        HashMapFirst(GlobalIncomingVal);
        for(Pair *pair = HashMapNext(GlobalIncomingVal); pair != NULL; pair = HashMapNext(GlobalIncomingVal)){
            int *globalDefineTimes = (int *)malloc(sizeof(int));
            *(globalDefineTimes) = 0;
            Value *alloc = (Value*)pair->key;
            HashMapPut(countDefine,alloc,globalDefineTimes);
        }
    }

    printf("11111 \n");
    // 变量重命名
    while(curr != get_next_inst(tail)){
        switch(curr->inst->Opcode) {
            case Load: {
                Value *alloc = ins_get_lhs(curr->inst);
                if(!isArray(alloc) && !isGlobalVar(alloc)){
                    // 需要被替换的
                    Value *value = ins_get_value(curr->inst);
                    // 对应的alloc
                    Value *replace = nullptr;
                    //找到栈
                    stack *allocStack = HashMapGet(IncomingVals, alloc);
                    printf("what name : %s\n",alloc->name);
                    //如果是nullptr
                    if(allocStack == nullptr){
                        //尝试去全局里面去取
                        stack *globalStack = HashMapGet(GlobalIncomingVal,alloc);
                        assert(globalStack != nullptr);
                        stackTop(globalStack,(void *)&replace);
                        assert(replace != nullptr);
                    }else{
                        assert(allocStack != nullptr);
                        //去里面找
                        stackTop(allocStack, (void *) &replace);
                        assert(replace != nullptr);
                    }
                    value_replaceAll(value, replace);
                }
                break;
            }
            case Store: {
                Value *store = ins_get_rhs(curr->inst); // 对应的allocas
                if(!isArray(store) && !isGlobalVar(store)){
                    //更新变量的使用
                    //对应的{}
                    Value *data = ins_get_lhs(curr->inst);
                    //可以直接这样更新
                    stack *allocStack = HashMapGet(IncomingVals, store);
                    if(allocStack == nullptr){
                        stack *globalStack = HashMapGet(GlobalIncomingVal,store);
                        assert(globalStack != nullptr);
                        stackPush(globalStack,data);
                    }else{
                        assert(allocStack != nullptr);
                        stackPush(allocStack, data);
                    }
                    //记录define的次数
                    int *defineTime = HashMapGet(countDefine,store);
                    //if(count
                    assert(defineTime != NULL);
                    *(defineTime) = *(defineTime) + 1;
                }
                break;
            }
            case Phi: {
                // 找到对应的allocas
                Value *alloc = curr->inst->user.value.alias;
                // 如果是phi的话我们就需要将现在的栈顶设置为phi指令
                Value *phi = &curr->inst->user.value;
                stack *allocStack = HashMapGet(IncomingVals, alloc);
                assert(allocStack != nullptr);
                stackPush(allocStack, phi);

                //
                int *defineTime = HashMapGet(countDefine,alloc);
                assert(defineTime != NULL);
                *(defineTime) = *(defineTime) + 1;
                break;
            }
        }
        curr = get_next_inst(curr);
    }

    // 维护该基本块所有的后继基本块中的phi指令 修改phi函数中的参数
    // 先找到它的后继基本块有两种可能

    printf("22222\n");
    if(tail->inst->Opcode == br){
        //label的编号是
        int labelId = tail->inst->user.value.pdata->instruction_pdata.true_goto_location;
        //从function的头节点开始向后寻找
        InstNode *funcHead = ins_get_funcHead(tail);
        BasicBlock *nextBlock = search_ins_label(funcHead,labelId)->inst->Parent;

        //维护这个block内的phiNode
        InstNode *nextBlockHead = nextBlock->head_node;
        //跳过Label
        InstNode *nextBlockCurr = get_next_inst(nextBlockHead);
        while(nextBlockCurr->inst->Opcode == Phi){
            //对应的是哪个
            Value *alias = nextBlockCurr->inst->user.value.alias;

            printf("fuck \n");
            //去找对应需要更新的
            stack *allocStack = HashMapGet(IncomingVals,alias);
            if(allocStack == NULL){
                allocStack = HashMapGet(GlobalIncomingVal,alias);
            }
            assert(allocStack != NULL);

            //从中去取目前到达的定义
            Value *pairValue;
            stackTop(allocStack,(void *)&pairValue);
            printf("fuck \n");
            //填充信息
            pair *phiInfo = createPhiInfo(block,pairValue);
            printf("fuck \n");
            insertPhiInfo(nextBlockCurr,phiInfo);
            printf("fuck \n");
            nextBlockCurr = get_next_inst(nextBlockCurr);
            printf("fuck \n");
        }
    }


    printf("44444\n");
    if(tail->inst->Opcode == br_i1){
        int labelId1 = tail->inst->user.value.pdata->instruction_pdata.true_goto_location;
        int labelId2 = tail->inst->user.value.pdata->instruction_pdata.false_goto_location;

        InstNode *funcHead = ins_get_funcHead(tail);

        BasicBlock *trueBlock = search_ins_label(funcHead,labelId1)->inst->Parent;
        BasicBlock *falseBlock = search_ins_label(funcHead,labelId2)->inst->Parent;

        //两次循环里面去维护
        //注意第一条语句一定是label
        InstNode *trueBlockCurr = get_next_inst(trueBlock->head_node);
        InstNode *falseBlockCurr = get_next_inst(falseBlock->head_node);

        //分别去维护信息
        while(trueBlockCurr->inst->Opcode == Phi){
            // 对应的是哪个alias
            Value *alias = trueBlockCurr->inst->user.value.alias;

            //取栈里面找那个对应的Value
            stack *allocStack = HashMapGet(IncomingVals,alias);
            if(allocStack == NULL){
                allocStack = HashMapGet(GlobalIncomingVal,alias);
            }
            assert(allocStack != NULL);

            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);

            pair *phiInfo = createPhiInfo(block,pairValue);
            insertPhiInfo(trueBlockCurr,phiInfo);
            trueBlockCurr = get_next_inst(trueBlockCurr);
        }

        while(falseBlockCurr->inst->Opcode == Phi){
            Value *alias = falseBlockCurr->inst->user.value.alias;

            stack *allocStack = HashMapGet(IncomingVals,alias);
            if(allocStack == NULL){
                allocStack = HashMapGet(GlobalIncomingVal,alias);
            }
            assert(allocStack != NULL);

            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);

            pair *phiInfo  = createPhiInfo(block,pairValue);
            insertPhiInfo(falseBlockCurr,phiInfo);
            falseBlockCurr = get_next_inst(falseBlockCurr);
        }
    }

    printf("333\n");
    // 递归遍历
    HashSetFirst(node->children);
    for(DomTreeNode *key = HashSetNext(node->children); key != nullptr; key = HashSetNext(node->children)){
        dfsTravelDomTree(key,IncomingVals);
    }

    // 弹栈
    HashMapFirst((IncomingVals));
    for(Pair *pair = HashMapNext(IncomingVals); pair != NULL; pair = HashMapNext(IncomingVals)){
        Value *alloc = pair->key;

        stack *allocStack = HashMapGet(IncomingVals,alloc);
        if(allocStack != NULL){
            //去找压栈的次数
            assert(allocStack != NULL);
            int *defineTime = HashMapGet(countDefine,alloc);
            int actualTime = *(defineTime);
            printf("block : %d alloc : %s defineTimes : %d\n",block->id,alloc->name,actualTime);
            for(int i = 0; i < actualTime; i++){
                stackPop(allocStack);
            }
        }else{
            // 全局变量里面去压栈操作
            stack *globalAllocStack = HashMapGet(GlobalIncomingVal,alloc);
            assert(globalAllocStack != NULL);
            int *globalDefineTime = HashMapGet(countDefine,alloc);
            int actualTime = *(globalDefineTime);
            for(int i = 0; i < actualTime; i++){
                stackPop(globalAllocStack);
            }
        }
    }

    //释放内存
    HashMapFirst(countDefine);
    for(Pair *pair = HashMapNext(countDefine); pair != NULL; pair = HashMapNext(countDefine)){
        int *countTime = pair->value;
        free(countTime);
    }
}

void deleteLoadStore(Function *currentFunction){
    // 删除这个函数内的load store

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    InstNode *curr = head;
    while(curr != get_next_inst(tail)){
        switch(curr->inst->Opcode){
            case Alloca:{
                Value *insValue = ins_get_value(curr->inst);
                if(!isArray(insValue)){
                    InstNode *next = get_next_inst(curr);
                    delete_inst(curr);
                    curr = next;
                }else{
                    curr = get_next_inst(curr);
                }
                break;
            }
            case Load:{
                Value *loadValue = ins_get_lhs(curr->inst);
                if(!isArray(loadValue) && !isGlobalVar(loadValue)){
                    InstNode *next = get_next_inst(curr);
                    delete_inst(curr);
                    curr = next;
                }else{
                    curr = get_next_inst(curr);
                }
                break;
            }
            case Store:{
                Value *storeValue = ins_get_rhs(curr->inst);
                if(!isArray(storeValue) && !isGlobalVar(storeValue)){
                    InstNode *next = get_next_inst(curr);
                    delete_inst(curr);
                    curr = next;
                }else{
                    curr = get_next_inst(curr);
                }
                break;
            }
            default:{
                curr = get_next_inst(curr);
                break;
            }
        }
    }
}

void renameVariabels(Function *currentFunction) {
    bool haveParam = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;

    //默认无论有没有参数第一个基本块为0
    //entry->id = 0;
    //currNode的第一条是FunBegin,判断一下是否有参
    Value *funcValue = currNode->inst->user.use_list->Val;
    if (funcValue->pdata->symtab_func_pdata.param_num > 0)
        haveParam = true;

    //开始时候为1或__
    int countVariable = 1;
    if (haveParam)
        countVariable += funcValue->pdata->symtab_func_pdata.param_num;

    while (currNode != get_next_inst(end->tail_node)) {
        if (currNode->inst->Opcode != br && currNode->inst->Opcode != br_i1) {

            if (currNode->inst->Opcode == Label) {
                //所有有可能用到这个label的都需要更新 只扫描之前的 它一定大于更新了的
                int prevLabel = currNode->inst->user.value.pdata->instruction_pdata.true_goto_location;

                // 去上面区间里面找
                InstNode *tempNode = entry->head_node;
                while (tempNode != NULL) {

                    if (tempNode->inst->Opcode == br || tempNode->inst->Opcode == br_i1) {
                        if (tempNode->inst->user.value.pdata->instruction_pdata.true_goto_location == prevLabel) {

                            tempNode->inst->user.value.pdata->instruction_pdata.true_goto_location = countVariable;
                        } else if (tempNode->inst->user.value.pdata->instruction_pdata.false_goto_location ==
                                   prevLabel) {
                            // 直接更新
                            tempNode->inst->user.value.pdata->instruction_pdata.false_goto_location = countVariable;
                        }
                    }
                    tempNode = get_next_inst(tempNode);
                }

                //更新一下BasicBlock的ID 顺便就更新了phi
                BasicBlock *block = currNode->inst->Parent;
                block->id = countVariable;
                currNode->inst->user.value.pdata->instruction_pdata.true_goto_location = countVariable;
                countVariable++;
            } else {
                // 普通的instruction语句
                char *insName = currNode->inst->user.value.name;

                //如果不为空那我们可以进行重命名
                if (insName != NULL && insName[0] == '%') {
                    char newName[5];
                    clear_tmp(insName);
                    newName[0] = '%';
                    int index = 1;
                    int rep_count = countVariable;
                    while (rep_count) {
                        newName[index] = (rep_count % 10) + '0';
                        rep_count /= 10;
                        index++;
                    }
                    int j = 1;
                    for (int i = index - 1; i >= 1; i--) {
                        insName[j] = newName[i];
                        j++;
                    }
                    countVariable++;
                }
            }
        }
        currNode = get_next_inst(currNode);
    }
}


void outOfSSA(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    printf("in out of ssa\n");
    InstNode *currNode = entry->head_node;

    while(currNode != get_next_inst(end->tail_node)){
        printf("hello !\n");
        if(currNode->inst->Opcode == Phi){
            assert(currNode->inst->user.value.pdata->pairSet != NULL);
            printf("recognize a phi!\n");
            HashSetFirst(currNode->inst->user.value.pdata->pairSet);
            for(pair *phiInfo = HashSetNext(currNode->inst->user.value.pdata->pairSet); phiInfo != NULL; phiInfo = HashSetNext(currNode->inst->user.value.pdata->pairSet)){
                BasicBlock *from = phiInfo->from;
                Value *src = phiInfo->define;
                assert(from != NULL);
                assert(src != NULL);
                Value *dest = ins_get_value(currNode->inst);
                printf("hashset first!\n");
                printf("insert copy of %s to b%d\n",src->name, from->id);
                insertCopies(from,dest,src);
                printf("insert success!\n");
            }
            printf("insert a phi\n");
            InstNode *nextNode = get_next_inst(currNode);
            delete_inst(currNode);
            currNode = nextNode;
        }else{
            currNode = get_next_inst(currNode);
        }
    }
}


InstNode *newCopyOperation(Value *dest, Value *src){
    // 此时挂载了
    printf("copy ins created!\n");
    Instruction *copyIns = ins_new_unary_operator(CopyOperation,src);
    printf("copy ins created!\n");
    assert(copyIns != NULL);
    copyIns->Opcode = CopyOperation;
    copyIns->user.value.alias = dest;
    InstNode *copyInsNode = new_inst_node(copyIns);
    assert(copyInsNode != NULL);
    return copyInsNode;
}