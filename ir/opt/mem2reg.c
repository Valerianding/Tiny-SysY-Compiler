//
// Created by Valerian on 2023/1/15.
//

#include "mem2reg.h"

void CleanObject(void* obj){
    //(Value*)obj;
    free((Value*)obj);
}

InstNode* new_phi(Value *val){
    Instruction *phiIns = ins_new(0);
    phiIns->Opcode = Phi;
    InstNode *phiNode = new_inst_node(phiIns);
    //做一个映射 记录现在对应的是哪个alloca
    phiNode->inst->user.value.alias = val;
    // 添加一个默认的名字
    phiIns->user.value.name = (char *)malloc(sizeof(char) * 4);
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

void insertPhiInfo(InstNode *ins,pair *phiInfo){
    if(ins->inst->user.value.pdata->pairSet == nullptr){
        ins->inst->user.value.pdata->pairSet = HashSetInit();
    }
    HashSetAdd(ins->inst->user.value.pdata->pairSet,phiInfo);
}

void mem2reg(Function *currentFunction){
    //对于现在的Function初始化
    currentFunction->loadSet = HashMapInit();
    currentFunction->storeSet = HashMapInit();

    //HashSet *allocas = HashSetInit();

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
            InstNode *next = get_next_inst(curNode);
            delete_inst(curNode);
            curNode = next;
        }else if(curNode->inst->Opcode == Store && !HashMapContain(currentFunction->loadSet, ins_get_rhs(curNode->inst))) {
            InstNode *next = get_next_inst(curNode);
            delete_inst(curNode);
            curNode = next;
        }else{
            curNode = get_next_inst(curNode);
        }
    }

    //TODO 做一些剪枝来保证phi函数插入更加简介

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



    //变量重新命名
    // DomTreeNode *root = currentFunction->root;
    assert(entry->domTreeNode == currentFunction->root);
    DomTreeNode *root = entry->domTreeNode;

    //利用一个HashMap alloca的value*  -> stack*

    //(a) A hash table of IncomingVals which is a map from a alloca to its most recent name is created
    // Most recent name of each alloca is an undef value to start with
    HashMap *IncomingVals = HashMapInit();

    curNode = entry->head_node;
    //printf("entry id : %d",curNode->inst->i);
    //printf("tail id : %d",entry->tail_node->inst->i);
    while(curNode != get_next_inst(entry->tail_node)){
        //printf("curNode id is : %d",curNode->inst->i);
        if(curNode->inst->Opcode == Alloca){
            //每一个alloca都对应一个stack
            stack *allocaStack = stackInit();
            //stackSetClean(allocaStack,CleanObject);
            assert(allocaStack != nullptr);
            //printf("name : %s created stack\n",curNode->inst->user.value.name);
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

    // delete load 和 store
    deleteLoadStore(currentFunction);


    // 让LLVM IR符合标准
    //renameVariabels(currentFunction);


    // OK 记得释放内存哦
    //先释放栈的内存再释放HashMap的内存
    HashMapFirst(IncomingVals);
    for(Pair *pair = HashMapNext(IncomingVals); pair != NULL; pair = HashMapNext(IncomingVals)){
        stack *allocStack = pair->value;
        stackDeinit(allocStack);
    }
    HashMapDeinit(IncomingVals);
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

    // 变量重命名
    while(curr != get_next_inst(tail)){
        switch(curr->inst->Opcode) {
            case Load: {
                // 需要被替换的
                Value *value = ins_get_value(curr->inst);
                // 对应的allocas
                Value *alloca = ins_get_lhs(curr->inst);
                Value *replace = nullptr;
                //找到栈
                stack *allocStack = HashMapGet(IncomingVals, alloca);
                assert(allocStack != nullptr);
                //去里面找
                stackTop(allocStack, (void *) &replace);
                assert(replace != nullptr);

                value_replaceAll(value, replace);
                break;
            }
            case Store: {
                //更新变量的使用与
                //对应的{}
                Value *data = ins_get_lhs(curr->inst);
                Value *store = ins_get_rhs(curr->inst); // 对应的allocas
                //可以直接这样更新
                stack *allocStack = (stack *) HashMapGet(IncomingVals, store);
                assert(allocStack != nullptr);
                stackPush(allocStack, data);

                //记录define的次数
                int *defineTime = HashMapGet(countDefine,store);
                assert(defineTime != NULL);
                *(defineTime) = *(defineTime) + 1;
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

    if(tail->inst->Opcode == br){
        //label的编号是
        int labelId = tail->inst->user.value.pdata->instruction_pdata.true_goto_location;
        //从function的头节点开始向后寻找
        InstNode *funcHead = ins_get_funcHead(tail);
        BasicBlock *nextBlock = search_ins_label(funcHead,labelId)->inst->Parent;

        //维护这个block内的phiNode
        InstNode *nextBlockHead = nextBlock->head_node;
        InstNode *nextBlockCurr = get_next_inst(nextBlockHead);
        while(nextBlockCurr->inst->Opcode == Phi){
            //对应的是哪个
            Value *alias = nextBlockCurr->inst->user.value.alias;

            //去找对应需要更新的
            stack *allocStack = HashMapGet(IncomingVals,alias);
            assert(allocStack != NULL);

            //从中去取目前到达的定义
            Value *pairValue;
            stackTop(allocStack,(void *)&pairValue);

            //填充信息
            pair *phiInfo = createPhiInfo(block,pairValue);
            insertPhiInfo(nextBlockCurr,phiInfo);
            nextBlockCurr = get_next_inst(nextBlockCurr);
        }
    }

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
            assert(allocStack != NULL);

            Value *pairValue = NULL;
            stackTop(allocStack,(void *)&pairValue);

            pair *phiInfo  = createPhiInfo(block,pairValue);
            insertPhiInfo(falseBlockCurr,phiInfo);
            falseBlockCurr = get_next_inst(falseBlockCurr);
        }
    }

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
        //去找压栈的次数
        int *defineTime = HashMapGet(countDefine,alloc);
        int actualTime = *(defineTime);
        printf("block : %d alloc : %s defineTimes : %d\n",block->id,alloc->name,actualTime);
        for(int i = 0; i < actualTime; i++){
            stackPop(allocStack);
        }
    }
}

void deleteLoadStore(Function *currentFunction){
    // 删除这个函数内的load store

    BasicBlock *entry = currentFunction->head;
    BasicBlock *end = currentFunction->tail;

    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    InstNode *curr = head;
    while(curr != get_next_inst(tail)){
        switch(curr->inst->Opcode){
            case Load:{
                InstNode *next = get_next_inst(curr);
                delete_inst(curr);
                curr = next;
                break;
            }
            case Store:{
                InstNode *next = get_next_inst(curr);
                delete_inst(curr);
                curr = next;
                break;
            }
            default:{
                curr = get_next_inst(curr);
                break;
            }
        }
    }
}

//TODO 草，对不起，这算法太丑了，但是能跑，若有时间，想想有没有什么别的方法
void renameVariabels(Function *currentFunction){
    bool hava_param=false;
    BasicBlock *entry = currentFunction->head;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;
    //currNode的第一条是FunBegin,判断一下是否有参
    Value *v_func=currNode->inst->user.use_list->Val;
    if(v_func->pdata->symtab_func_pdata.param_num>0)
        hava_param=true;

    //开始时候为1或__
    int countVariable=1;
    if(hava_param)                //TODO 有参的情况看丁老师具体想怎么处理
        countVariable=v_func->pdata->symtab_func_pdata.param_num+1;


    ///先遍历两次，搭一个label与br语句的映射关系（对不起，感觉好丑），本来想过在遍历语法树的时候就建上关系，但是看了一下，短路的话，逻辑有点复杂，不太好加
    HashMap* hashMap=HashMapInit(); //<key,value>-----> <Value*,HashSet<Value*>>，key是label的value,value是br或br_i1或phi的value set

    //先装入label的value，作为全部key
    while(currNode != get_next_inst(end->tail_node)){
        if(currNode->inst->Opcode==Label)
        {
           HashSet *set=HashSetInit();
           HashMapPut(hashMap,(void*)&currNode->inst->user.value,(void*)set);
        }
        currNode = get_next_inst(currNode);
    }

    //将br,br_i1,phi装到对应key的value set中
    //br:设置v_br的alias为NULL，肯定是它的true_location对应label的值
    //br_i1:设置v_br的alias为false_location对应的那个label的value，判断要装入的究竟是true的还是false的
    currNode=entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        if(currNode->inst->Opcode==br)
        {
            int true_location=currNode->inst->user.value.pdata->instruction_pdata.true_goto_location;
            HashMapFirst(hashMap);
            for(Pair *pair = HashMapNext(hashMap); pair != NULL; pair = HashMapNext(hashMap)){
                Value *v=pair->key;
                if(v->pdata->instruction_pdata.true_goto_location==true_location)
                {
                    //拿出那个set,加入这个br的value
                    HashSet *set=pair->value;
                    Value *v_br=&currNode->inst->user.value;
                    v_br->alias=NULL;
                    HashSetAdd(set,v_br);
                    break;
                }
            }
        }
        else if(currNode->inst->Opcode==br_i1)
        {
            int true_location=currNode->inst->user.value.pdata->instruction_pdata.true_goto_location;
            int false_location=currNode->inst->user.value.pdata->instruction_pdata.false_goto_location;

            HashMapFirst(hashMap);
            for(Pair *pair = HashMapNext(hashMap); pair != NULL; pair = HashMapNext(hashMap)){
                Value *v=pair->key;
                if(v->pdata->instruction_pdata.true_goto_location==true_location)
                {
                    //拿出那个set,加入这个br_i1的value
                    HashSet *set=pair->value;
                    Value *v_br=&currNode->inst->user.value;
                    HashSetAdd(set,v_br);
                }
                else if(v->pdata->instruction_pdata.true_goto_location==false_location)
                {
                    //拿出那个set,加入这个br_i1的value
                    HashSet *set=pair->value;
                    Value *v_br=&currNode->inst->user.value;
                    //这个br i1的alias设置为这个label value
                    v_br->alias=v;
                    HashSetAdd(set,v_br);
                }
            }
        }
        else if(currNode->inst->Opcode==Phi)
        {
            //1.取出hashSet
            HashSet *phiSet=currNode->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                BasicBlock *from = phiInfo->from;
                HashMapFirst(hashMap);
                for(Pair *pair = HashMapNext(hashMap); pair != NULL; pair = HashMapNext(hashMap)){
                    Value *v=pair->key;
                    if(v->pdata->instruction_pdata.true_goto_location==from->id)
                    {
                        //拿出那个set,加入这个value
                        HashSet *set=pair->value;
                        Value *v_phi=&currNode->inst->user.value;
                        HashSetAdd(set,v_phi);
                    }
                }
            }
        }
        currNode = get_next_inst(currNode);
    }

    currNode=entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){

        // 只用考虑修改左边的并且我们暂时不修改alloc指令
        if(currNode->inst->Opcode != Alloca && currNode->inst->Opcode!=br && currNode->inst->Opcode!=br_i1){
            if(currNode->inst->Opcode == Label){
                HashSet *hashset=hashMap->get(hashMap,&currNode->inst->user.value);

                HashSetFirst(hashset);
                for(Value *v_br = HashSetNext(hashset); v_br != NULL; v_br = HashSetNext(hashset)){
                    if(v_br->name!=NULL)
                    {
                        //是phi
                        HashSet *phiSet=v_br->pdata->pairSet;
                        HashSetFirst(phiSet);
                        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                            BasicBlock *from = phiInfo->from;
                            if(currNode->inst->user.value.pdata->instruction_pdata.true_goto_location==from->id)
                            {
                                //修改该修改的对应id
                                phiInfo->from->id=countVariable;
                            }
                        }
                    }
                    else
                    {
                        if(v_br->alias==NULL)        //br的情况可完全确定
                            v_br->pdata->instruction_pdata.true_goto_location=countVariable;
                        else                         //br_i1的情况
                        {
                            if(v_br->alias->pdata->instruction_pdata.true_goto_location==currNode->inst->user.value.pdata->instruction_pdata.true_goto_location)
                                v_br->pdata->instruction_pdata.false_goto_location=countVariable;          //如果是false_location的位置，与label有映射
                            else
                                v_br->pdata->instruction_pdata.true_goto_location=countVariable;
                        }
                    }
                }
                currNode->inst->user.value.pdata->instruction_pdata.true_goto_location = countVariable;
                countVariable++;
            }
            else {

                //普通的instruction语句
                char *insName = currNode->inst->user.value.name;

                //如果不为空那我们可以进行重命名
                if(insName != NULL && insName[0] == '%'){
                    char newName[5];
                    clear_tmp(insName);
                    newName[0]='%';
                    int index = 1;
                    int rep_count=countVariable;
                    while(rep_count){
                        newName[index] = (rep_count % 10) + '0';
                        rep_count /= 10;
                        index++;
                    }
                    for(int i = 1; i < index; i++){
                        insName[i] = newName[i];
                    }
                    countVariable++;
                }
            }
        }

        currNode = get_next_inst(currNode);
    }

    //内存释放
    HashMapFirst(hashMap);
    for(Pair *pair = HashMapNext(hashMap); pair != NULL; pair = HashMapNext(hashMap)){
        HashSet *set=pair->value;
        HashSetDeinit(set);
    }
    HashMapDeinit(hashMap);
}