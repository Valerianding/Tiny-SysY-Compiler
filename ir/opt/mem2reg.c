//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"
HashSet *nonLocals = NULL;
HashSet *killed = NULL;
InstNode* new_phi(Value *val){
    Instruction *phiIns = ins_new(0);
    phiIns->Opcode = Phi;
    phiIns->user.value.pdata->pairSet = NULL;
    phiIns->user.value.IsPhi = true;
    phiIns->user.value.Useless = false;
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

    //设置当前phi函数的type
    Value *insValue = ins_get_dest(phiInstNode->inst);
    insValue->VTy->ID = val->VTy->ID;
    printf("%d--------",val->VTy->ID);
    printf("set PhiNode success !\n");
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
            if(isLocalVar(loadValue)){
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
            if(isLocalVar(storeValue)){
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
        //不是alloca的数组并且对这个alloca没有load的话
        Value *insValue = ins_get_dest(curNode->inst);
        if(curNode->inst->Opcode == Alloca && !isLocalArray(insValue) && !HashMapContain(currentFunction->loadSet,insValue)){
            //不存在loadSet之中不存在这个value
            //删除这个instruction
            InstNode *next = get_next_inst(curNode);
            deleteIns(curNode);
            curNode = next;
        }else if(curNode->inst->Opcode == Store) {
            Value *storeValue = ins_get_rhs(curNode->inst);
            if(isLocalVar(storeValue) && !HashMapContain(currentFunction->loadSet, storeValue)){
                InstNode *next = get_next_inst(curNode);
                deleteIns(curNode);
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

        // 只有non-Locals 我们才需要放置phi函数
        if(HashSetFind(nonLocals, val)){
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
        printf("---------------------------------------------fuck--------------------------------------- \n");
        dfsTravelDomTree(root,IncomingVals);
    }

    printf("after rename !\n");

    prunePhi(currentFunction);

    correctPhiNode(currentFunction);

    printf("after correct phiNode\n");

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

// 针对的是需要拷贝回原来的
void insertCopies(BasicBlock *block,Value *dest,Value *src){
    InstNode *tailIns = block->tail_node;
    assert(tailIns != NULL);
    InstNode *copyIns = newCopyOperation(src);
    assert(copyIns != NULL);
    copyIns->inst->Parent = block;
    Value *insValue = ins_get_dest(copyIns->inst);
    insValue->alias = dest;

    //应该不会超过10位数
    // 针对这个情况好像没什么用
    insValue->name = (char*)malloc(sizeof(char) * 10);
    strcpy(insValue->name,"%copy");
    ins_insert_before(copyIns,tailIns);

    // 由于是CopyOperation 所以phi的type我们已经是设置好了的
}


void dfsTravelDomTree(DomTreeNode *node,HashMap *IncomingVals){

    printf("in rename phrase : block : %d\n", node->block->id);
    assert(HashMapSize(IncomingVals) != 0);
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

    printf("11111 \n");
    // 变量重命名
    while(curr != get_next_inst(tail)){
        switch(curr->inst->Opcode) {
            case Load: {
                Value *alloc = ins_get_lhs(curr->inst);
                if(isLocalVar(alloc)){
                    // 对应的alloc
                    printf("what name : %s\n",alloc->name);
                    // 需要被替换的
                    Value *value = ins_get_dest(curr->inst);
                    Value *replace = nullptr;
                    //找到栈
                    stack *allocStack = HashMapGet(IncomingVals, alloc);
                    //如果是nullptr
                    assert(allocStack != nullptr);
                    //去里面找
                    stackTop(allocStack, (void *) &replace);

                    //replace 必然不能是nullptr否则会出现未定义错误
                    assert(replace != nullptr);
                    value_replaceAll(value, replace);
                }
                break;
            }
            case Store: {
                Value *store = ins_get_rhs(curr->inst); // 对应的allocas
                if(isLocalVar(store)){
                    //更新变量的使用
                    //对应的{}
                    Value *data = ins_get_lhs(curr->inst);
                    //可以直接这样更新
                    stack *allocStack = HashMapGet(IncomingVals, store);
                    assert(allocStack != nullptr);
                    printf("store %s\n",data->name);
                    stackPush(allocStack, data);
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

    // 如果当前的末尾是funcEnd的话我们需要回退一个instNode
    if(tail->inst->Opcode == FunEnd){
        tail = get_prev_inst(tail);
    }

    // 维护该基本块所有的后继基本块中的phi指令 修改phi函数中的参数
    if(tail->inst->Opcode == br){
        //label的编号是
        int labelId = tail->inst->user.value.pdata->instruction_pdata.true_goto_location;
        printf("br %d\n",labelId);
        //从function的头节点开始向后寻找
        InstNode *funcHead = ins_get_funcHead(tail);
        BasicBlock *nextBlock = search_ins_label(funcHead,labelId)->inst->Parent;

        //维护这个block内的phiNode 跳过Label
        InstNode *nextBlockCurr = get_next_inst(nextBlock->head_node);

        while(nextBlockCurr->inst->Opcode == Phi){
            //对应的是哪个
            Value *alias = nextBlockCurr->inst->user.value.alias;
            //去找对应需要更新的
            stack *allocStack = HashMapGet(IncomingVals,alias);
            assert(allocStack != NULL);

            //从中去取目前到达的定义
            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);
            //填充信息

            // TODO 如果是空那么应该填充Undefine
            if(pairValue != NULL){
                pair *phiInfo = createPhiInfo(block,pairValue);
                insertPhiInfo(nextBlockCurr,phiInfo);
                printf("insert a phi info\n");
            }else{
                // 如果是NULL的话就是Undefine
                pair *phiInfo = createPhiInfo(block,pairValue);
                insertPhiInfo(nextBlockCurr,phiInfo);
            }
            nextBlockCurr = get_next_inst(nextBlockCurr);
        }
    }

    if(tail->inst->Opcode == br_i1){
        int labelId1 = tail->inst->user.value.pdata->instruction_pdata.true_goto_location;
        int labelId2 = tail->inst->user.value.pdata->instruction_pdata.false_goto_location;
        InstNode *funcHead = ins_get_funcHead(tail);

        BasicBlock *trueBlock = search_ins_label(funcHead,labelId1)->inst->Parent;
        BasicBlock *falseBlock = search_ins_label(funcHead,labelId2)->inst->Parent;


        //跳回第一个基本块没有意义 所以我们都去跳过Label
        InstNode *trueBlockCurr = get_next_inst(trueBlock->head_node);
        InstNode *falseBlockCurr = get_next_inst(falseBlock->head_node);

        //分别去维护信息
        while(trueBlockCurr->inst->Opcode == Phi){
            // 对应的是哪个alias
            Value *alias = trueBlockCurr->inst->user.value.alias;

            //取栈里面找那个对应的Value
            stack *allocStack = HashMapGet(IncomingVals,alias);
            assert(allocStack != NULL);

            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);

            // TODO 还是需要
            if(pairValue != NULL){
                pair *phiInfo = createPhiInfo(block,pairValue);
                insertPhiInfo(trueBlockCurr,phiInfo);
            }else{
                pair *phiInfo = createPhiInfo(block,pairValue);
                insertPhiInfo(trueBlockCurr,phiInfo);
            }
            trueBlockCurr = get_next_inst(trueBlockCurr);
        }

        while(falseBlockCurr->inst->Opcode == Phi){
            Value *alias = falseBlockCurr->inst->user.value.alias;

            stack *allocStack = HashMapGet(IncomingVals,alias);
            assert(allocStack != NULL);

            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);

            // TODO pairValue 是NULL的时候需要是Undefine
            if(pairValue != NULL){
                pair *phiInfo  = createPhiInfo(block,pairValue);
                insertPhiInfo(falseBlockCurr,phiInfo);
            }else{
                pair *phiInfo  = createPhiInfo(block,pairValue);
                insertPhiInfo(falseBlockCurr,phiInfo);
            }
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
        assert(allocStack != NULL);
        if(allocStack != NULL){
            //去找压栈的次数
            assert(allocStack != NULL);
            int *defineTime = HashMapGet(countDefine,alloc);
            int actualTime = *(defineTime);
            printf("block : %d alloc : %s defineTimes : %d\n",block->id,alloc->name,actualTime);
            for(int i = 0; i < actualTime; i++){
                stackPop(allocStack);
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
                Value *insValue = ins_get_dest(curr->inst);
                if(isLocalVar(insValue)){
                    InstNode *next = get_next_inst(curr);
                    deleteIns(curr);
                    curr = next;
                }else{
                    curr = get_next_inst(curr);
                }
                break;
            }
            case Load:{
                Value *loadValue = ins_get_lhs(curr->inst);
                if(isLocalVar(loadValue)){
                    InstNode *next = get_next_inst(curr);
                    deleteIns(curr);
                    curr = next;
                }else{
                    curr = get_next_inst(curr);
                }
                break;
            }
            case Store:{
                Value *storeValue = ins_get_rhs(curr->inst);
                if(isLocalVar(storeValue)){
                    InstNode *next = get_next_inst(curr);
                    deleteIns(curr);
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

InstNode *newCopyOperation(Value *src){
    // 此时挂载了
    Instruction *copyIns = ins_new_unary_operator(CopyOperation,src);
    assert(copyIns != NULL);
    copyIns->Opcode = CopyOperation;
    InstNode *copyInsNode = new_inst_node(copyIns);
    assert(copyInsNode != NULL);
    return copyInsNode;
}


void calculateNonLocals(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;
    InstNode *tailNode = end->tail_node;

    clear_visited_flag(entry);

    nonLocals = HashSetInit();
    killed = HashSetInit();
    while(currNode != get_next_inst(tailNode)){
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            // 每次都要重新计算killed
            HashSetClean(killed);
        }
        if(currNode->inst->Opcode == Load){
            Value *lhs = ins_get_lhs(currNode->inst);
            if(lhs != NULL && isLocalVar(lhs) && !HashSetFind(killed,lhs)){
                HashSetAdd(nonLocals,lhs);
            }
        }

        if(currNode->inst->Opcode == Store){
            Value *rhs = ins_get_rhs(currNode->inst);
            if(rhs != NULL && isLocalVar(rhs) && !HashSetFind(killed,rhs)){
                HashSetAdd(killed, rhs);
            }
        }
        currNode = get_next_inst(currNode);
    }

    HashSetFirst(nonLocals);
    printf("nonLocals: ");
    for(Value *nonLocalValue = HashSetNext(nonLocals); nonLocalValue != NULL; nonLocalValue = HashSetNext(nonLocals)){
        printf("%s ", nonLocalValue->name);
    }
    HashSetDeinit(killed);
    clear_visited_flag(entry);
}

void correctPhiNode(Function *currentFunction){

    bool changed = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;

    currNode = entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        if(currNode->inst->Opcode == Phi){
            HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;

            if(HashSetSize(phiSet) == 1){
                changed = true;
                HashSetFirst(phiSet);
                pair *phiInfo = HashSetNext(phiSet);
                Value *define = phiInfo->define;
                Value *insValue = ins_get_dest(currNode->inst);
                value_replaceAll(insValue,define);


                // 还不对因为有可能在另一个phi函数里面引用到了这个变量所以我们还得替换！
                // 从头到尾去判断
                InstNode *tempNode = entry->head_node;
                while(tempNode != get_next_inst(end->tail_node)){
                    if(tempNode->inst->Opcode == Phi){
                        HashSet *pairSet = tempNode->inst->user.value.pdata->pairSet;
                        HashSetFirst(pairSet);
                        for(pair *phiInfo1 = HashSetNext(pairSet); phiInfo1 != NULL; phiInfo1 = HashSetNext(pairSet)){
                            if(phiInfo1->define == insValue){
                                phiInfo1->define = define;
                            }
                        }
                    }
                    tempNode = get_next_inst(tempNode);
                }

                InstNode *nextNode = get_next_inst(currNode);
                deleteIns(currNode);
                currNode = nextNode;
            }else if(HashSetSize(phiSet) == 0){
                changed = true;
                InstNode *nextNode = get_next_inst(currNode);
                deleteIns(currNode);
                currNode = nextNode;
            }else{
              currNode = get_next_inst(currNode);
            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }


    //再解决
    currNode = entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        if(currNode->inst->Opcode == Phi){
            Value *insValue = ins_get_dest(currNode->inst);
            HashSet *pairSet = currNode->inst->user.value.pdata->pairSet;
            HashSetFirst(pairSet);
            unsigned int phiSize = HashSetSize(pairSet);
            unsigned int repeatSize = 0;
            for(pair *phiInfo = HashSetNext(pairSet); phiInfo != NULL; phiInfo = HashSetNext(pairSet)){
                if(phiInfo->define == insValue){
                    repeatSize++;
                }
            }
            if(phiSize - repeatSize == 1){
                changed = true;
                HashSetFirst(pairSet);
                for(pair *phiInfo = HashSetNext(pairSet); phiInfo != NULL; phiInfo = HashSetNext(pairSet)){
                    if(phiInfo->define == insValue){
                        HashSetRemove(pairSet,phiInfo);
                    }
                }
            }
        }
        currNode = get_next_inst(currNode);
    }
    if(changed) correctPhiNode(currentFunction);
}

void SSADeconstruction(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    clear_visited_flag(entry);
    InstNode *currNode = entry->head_node;

    // split the edge
    while(currNode != end->tail_node){
        // 每个基本块只遍历一次
        BasicBlock *block = currNode->inst->Parent;
        if(!block->visited){
            block->visited = true;
            //跳过Label
            InstNode *blockNode = get_next_inst(block->head_node);
            // 如果当前基本块上存在phi函数
            if(blockNode->inst->Opcode == Phi) {
                HashSetFirst(block->preBlocks);
                for(BasicBlock *prevBlock = HashSetNext(block->preBlocks);
                     prevBlock != NULL; prevBlock = HashSetNext(block->preBlocks)) {
                    if (prevBlock->true_block && prevBlock->false_block) {// 是关键边
                        printf("critical edge splitting!!!!!!!!!\n");
                        // 分割当前关键边
                        BasicBlock *newBlock = bb_create();
                        printf("prev block :%d \n",prevBlock->id);
                        InstNode *prevTail = prevBlock->tail_node;
                        InstNode *prevTailNext = get_next_inst(prevBlock->tail_node);

                        Instruction *newBlockLabel = ins_new_zero_operator(Label);
                        InstNode *newBlockLabelNode = new_inst_node(newBlockLabel);

                        //直接跳转语句
                        Instruction *newBlockBr = ins_new_zero_operator(br);
                        InstNode *newBlockBrNode = new_inst_node(newBlockBr);


                        ins_insert_after(newBlockLabelNode, prevTail);
                        ins_insert_after(newBlockBrNode, newBlockLabelNode);

                        //;
                        newBlockBrNode->list.next = &prevTailNext->list;
                        prevTailNext->list.prev = &newBlockBrNode->list;


                        // 维护这个基本块中的信息 TODO 没有维护支配信息等
                        bb_set_block(newBlock,newBlockLabelNode, newBlockBrNode);
                        newBlock->Parent = currentFunction;
                        newBlock->id = -2; // 如果是-2的话 表示是新的节点
                        // 前一个基本块到底是true 还是 false 是block
                        if (prevBlock->true_block == block) {
                            prevBlock->true_block = newBlock;
                        } else if (prevBlock->false_block == block) {
                            prevBlock->false_block = newBlock;
                        }
                        HashSetAdd(newBlock->preBlocks, prevBlock);
                        newBlock->true_block = block;
                        //移除原来那个边
                        HashSetRemove(block->preBlocks, prevBlock);
                        HashSetAdd(block->preBlocks, newBlock);
                        // 修改phiInfo里面的信息
                        InstNode *correctNode = get_next_inst(block->head_node);
                        while(correctNode->inst->Opcode == Phi){
                            Value *insValue = ins_get_dest(correctNode->inst);
                            HashSet *phiSet = insValue->pdata->pairSet;
                            HashSetFirst(phiSet);
                            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                                if(phiInfo->from == prevBlock){
                                    //原来是来自prevBlock的更改为来自新的Block
                                    phiInfo->from = newBlock;
                                }
                            }
                            correctNode = get_next_inst(correctNode);
                        }
                    }else{
                        //不动
                    }
                }
            }
            //进行拷贝操作
            InstNode* phiNode = get_next_inst(block->head_node);
            while(phiNode->inst->Opcode == Phi){
                Value *insValue = ins_get_dest(phiNode->inst);
                HashSet *phiSet = insValue->pdata->pairSet;
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    Value *src = phiInfo->define;
                    // TODO 这里粗浅的解决了自引用的问题
                    if(src != NULL && src != insValue){
                        BasicBlock *from = phiInfo->from;
                        insertCopies(from,insValue,src);
                    }
                }
                InstNode *nextNode = get_next_inst(phiNode);
                deleteIns(phiNode);
                phiNode = nextNode;
            }
        }
        currNode = get_next_inst(currNode);
    }

    renameVariabels(currentFunction);

    sequentialCopy(currentFunction);
}

void prunePhi(Function *currentFunction){
    Queue *workList = QueueInit();
    HashSet *phiStack = HashSetInit();

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;
    QueuePush(workList,entry);

    while(QueueSize(workList) != 0){
        BasicBlock *block = NULL;
        QueueFront(workList,(void *)&block);
        QueuePop(workList);
        assert(block != NULL);
        InstNode *blockCurr = block->head_node;
        InstNode *blockTail = block->tail_node;
        while(blockCurr != get_next_inst(blockTail)){
            if(blockCurr->inst->Opcode == Phi){
                Value *phiValue = ins_get_dest(blockCurr->inst);
                phiValue->Useless = true;
            }else{
                Value *lhs = ins_get_lhs(blockCurr->inst);
                if(lhs != NULL && lhs->IsPhi == true){
                    lhs->Useless = false;
                    HashSetAdd(phiStack,lhs);
                }
                Value *rhs = ins_get_rhs(blockCurr->inst);
                if(rhs != NULL && rhs->IsPhi == true){
                    rhs->Useless = false;
                    HashSetAdd(phiStack,rhs);
                }
            }
            blockCurr = get_next_inst(blockCurr);
        }

        //
        DomTreeNode *domTreeNode = block->domTreeNode;
        HashSetFirst(domTreeNode->children);
        for(DomTreeNode *childNode = HashSetNext(domTreeNode->children); childNode != NULL; childNode = HashSetNext(domTreeNode->children)){
            BasicBlock *childBlock = childNode->block;
            printf("push child %d\n",childBlock->id);
            QueuePush(workList,childBlock);
        }
    }
    //
    printf("second while!\n");

    printf("stack size : %d \n",HashSetSize(phiStack));


    while(HashSetSize(phiStack) != 0){
        HashSetFirst(phiStack);
        Value *phiValue = HashSetNext(phiStack);
        HashSetRemove(phiStack,phiValue);
        Instruction *phiIns = (Instruction*)phiValue;
        HashSet *phiSet = phiValue->pdata->pairSet;
        HashSetFirst(phiSet);
        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
            Value *src = phiInfo->define;
            if(src != NULL && src->IsPhi == true && src->Useless == true){
                src->Useless = false;
                HashSetAdd(phiStack,src);
            }
        }
    }

    //
    InstNode *currNode = entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        if(currNode->inst->Opcode == Phi){
            Value *phiValue = ins_get_dest(currNode->inst);
            if(phiValue->IsPhi == true && phiValue->Useless == true){
                InstNode *nextNode = get_next_inst(currNode);
                printf("delete a phiNode! -------\n");
                deleteIns(currNode);
                currNode = nextNode;
            }else{
                currNode = get_next_inst(currNode);
            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
}

void sequentialCopy(Function *currentFunction){
    //针对的是每一个BasicBlock上去做的
    BasicBlock *entry = currentFunction->entry;
    //
    clear_visited_flag(entry);

    Queue *workList = QueueInit();
    QueuePush(workList,entry);
    while(QueueSize(workList) != 0){
        BasicBlock *block = NULL;
        QueueFront(workList,(void *)&block);
        QueuePop(workList);
        assert(block != NULL);
        if(block->visited == false){
            block->visited = true;
            printf("current block is %d\n",block->id);
            HashSet *pCopy = HashSetInit(); // copyPair *
            Queue *seq = QueueInit();

            //对于每一个block我们去收集一下
            InstNode *copyIns = block->head_node;
            while(copyIns != block->tail_node){
                if(copyIns->inst->Opcode == CopyOperation){
                    Value *src = ins_get_lhs(copyIns->inst);
                    Value *dest = ins_get_dest(copyIns->inst)->alias;
                    CopyPair *copyPair = createCopyPair(src,dest);
                    HashSetAdd(pCopy,copyPair);
                    //找到src的use_list
                    use_remove_from_list(copyIns->inst->user.use_list);
                    // 删除parallelCopy语句 再seq的时候我们再进行插入
                    InstNode *nextIns = get_next_inst(copyIns);
                    deleteIns(copyIns);
                    copyIns = nextIns;
                }else{
                    copyIns = get_next_inst(copyIns);
                }
            }

            printf("out !\n");
            printf("pCopy size : %d\n", HashSetSize(pCopy));
            HashSetFirst(pCopy);
            for(CopyPair *copyPair = HashSetNext(pCopy); copyPair != NULL; copyPair = HashSetNext(pCopy)){
                printf("%s <- %s ",copyPair->dest->name,copyPair->src->name);
            }
            printf("\n");

            while(HashSetSize(pCopy) != 0){
                bool exist = true;
                while(exist && HashSetSize(pCopy) != 0) {
                    printf("pCopy size : %d\n", HashSetSize(pCopy));
                    HashSet *tempSet = HashSetInit();
                    HashSetCopyPair(tempSet,pCopy);
                    exist = false;
                    printf("contain : ");
                    HashSetFirst(pCopy);
                    for (CopyPair *copyPair = HashSetNext(pCopy); copyPair != NULL; copyPair = HashSetNext(pCopy)) {
                        printf("%s <- %s",copyPair->dest->name,copyPair->src->name);

                        Value *dest = copyPair->dest;
                        bool usedByOther = false;
                        HashSetFirst(tempSet);
                        for(CopyPair *otherPair = HashSetNext(tempSet); otherPair != NULL; otherPair = HashSetNext(tempSet)) {
                            if(otherPair->src == dest){
                                usedByOther = true;
                            }
                        }

                        if (usedByOther == false) {
                            printf("pushed a copy %s <- %s!\n", copyPair->dest->name, copyPair->src->name);
                            bool res = HashSetRemove(pCopy, copyPair);
                            QueuePush(seq, copyPair);
                            assert(res == true);
                            exist = true;
                        }
                    }
                    printf("\n");
                    HashSetDeinit(tempSet);
                }
                printf("out !\n");

                HashSetFirst(pCopy);
                CopyPair *copyPair = HashSetNext(pCopy);

                if(copyPair != NULL) {
                    printf("change %s <- %s\n", copyPair->dest->name,copyPair->src->name);
                    // used by other
                    // createnewValue;
                    Value *src = copyPair->src;
                    Value *tempValue = (Value*)malloc(sizeof(Value));
                    value_init(tempValue);
                    tempValue->name = (char*)malloc(sizeof(char) * 4);
                    strcpy(tempValue->name,"%temp");
                    // 自己为自己的alias
                    tempValue->alias = tempValue;

                    // create a new temp
                    CopyPair *tempPair = createCopyPair(src,tempValue);
                    QueuePush(seq,tempPair);
                    //replace pCopy b <- a into b <- a'src = tempValue;
                    copyPair->src = tempValue;
                }
            }
            CopyPair *seqCopy = NULL;
            while(QueueSize(seq) != 0) {
                // 从seq里面取出CopyPair进行还原
                // 从头去取
                QueueFront(seq,(void*)&seqCopy);
                QueuePop(seq);
                assert(seqCopy != NULL);
                insertCopies(block,seqCopy->dest,seqCopy->src);
            }
            //销毁内存
            HashSetDeinit(pCopy);
            QueueDeinit(seq);

            if(block->true_block && block->true_block->visited == false) QueuePush(workList,block->true_block);
            if(block->false_block && block->false_block->visited == false) QueuePush(workList,block->false_block);
        }
    }
    printf("after all!\n");
}

CopyPair *createCopyPair(Value *src, Value *dest){
    CopyPair *copyPair = (CopyPair*)malloc(sizeof(CopyPair));
    memset(copyPair,0,sizeof(CopyPair));
    copyPair->src = src;
    copyPair->dest = dest;
    return copyPair;
}

void HashSetCopyPair(HashSet *dest, HashSet *src){
    HashSetFirst(src);
    for(CopyPair *copyPair = HashSetNext(src); copyPair != NULL; copyPair = HashSetNext(src)){
        HashSetAdd(dest,copyPair);
    }
}