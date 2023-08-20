//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
// 出现在IR里面的我们都要进行分析了 除了alloca
// 对于alloca后端是自行建立了一个表去查找位置
const Opcode invalidOpcodes[] = {FunBegin, Label, ALLBEGIN, Alloca, tmp, zext, MEMCPY, zeroinitializer, GLOBAL_VAR, FunEnd};
const Opcode compareOpcodes[] = {EQ,NOTEQ,LESS, LESSEQ,GREAT,GREATEQ};
const Opcode hasNoDestOpcodes[] = {br,br_i1,br_i1_true,br_i1_false,Store,Return,Label,GIVE_PARAM};
const Opcode CriticalOpcodes[] = {Return,Call,Store,GIVE_PARAM,MEMCPY,MEMSET,SysYMemset,SysYMemcpy};
const Opcode CalculationOpcodes[] = {Add,Sub,Mul,Mod,Div,GEP};
const Opcode simpleOpcodes[] = {Add, Sub, Mul, Div, Mod,GEP};
//TODO 解决全局的公共子表达式 解决其对于Phi函数可能的破坏
const char* InputSySYFunctions[] = {"getint","getfloat","getch","getarray","getfarray"};
const char* OutputSySYFunctions[] = {"putint","putch","putarray","putfloat","putfarray","putf"};
const char* TimeSySYFunctions[] = {"_sysy_starttime","_sysy_stoptime"};
bool isValidOperator(InstNode *insNode){
    for (int i = 0; i < sizeof(invalidOpcodes) / sizeof(Opcode); i++) {
        if (insNode->inst->Opcode == invalidOpcodes[i]) {
            return false;
        }
    }
    return true;
}


bool isCompareOperator(InstNode *insNode){
    for(int i = 0; i < sizeof(compareOpcodes) / sizeof(Opcode); i++){
        if(insNode->inst->Opcode == compareOpcodes[i]){
            return true;
        }
    }
    return false;
}

bool isCriticalOperator(InstNode *insNode){
    for(int i = 0; i < sizeof(CriticalOpcodes) / sizeof(Opcode); i++){
        if(insNode->inst->Opcode == CriticalOpcodes[i]) {
            return true;
        }
    }
    return false;
}


bool hasNoDestOperator(InstNode *insNode){
    for(int i = 0; i < sizeof(hasNoDestOpcodes) / sizeof(Opcode); i++){
        if(insNode->inst->Opcode == hasNoDestOpcodes[i]){
            return true;
        }
    }
    return false;
}

void clear_visited_flag(BasicBlock *startBlock){
    InstNode *currNode = startBlock->head_node;

    while(currNode != NULL){
        BasicBlock *currNodeParent = currNode->inst->Parent;
        if(currNodeParent->visited == true){
            currNodeParent->visited = false;
        }
        currNode = get_next_inst(currNode);
    }
}

// alloca i32 **的话我们也可以mem2reg
void correctType(Function *currentFunction){
    InstNode *headNode = currentFunction->entry->head_node;
    while(headNode != get_next_inst(currentFunction->tail->tail_node)){
        Value *insValue = ins_get_dest(headNode->inst);
        if(headNode->inst->Opcode == Alloca && insValue->VTy->ID == AddressTyID){
            insValue->VTy->ID = Var_INT;
        }
        headNode = get_next_inst(headNode);
    }
}

// 都返回精度最高的类型 获得的一定是立即数
float getOperandValue(Value *operand){
    if(isImmInt(operand)){
        return (float)operand->pdata->var_pdata.iVal;
    }else if(isImmFloat(operand)){
        return operand->pdata->var_pdata.fVal;
    }
    return 0;
}

void renameVariables(Function *currentFunction) {
    bool haveParam = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;

    //currNode的第一条是FunBegin,判断一下是否有参
    Value *funcValue = currNode->inst->user.use_list->Val;
    if (funcValue->pdata->symtab_func_pdata.param_num > 0)
        haveParam = true;

    //开始时候为1或__
    int countVariable = 0;
    if (haveParam){
        //更新第一个基本块
        countVariable += funcValue->pdata->symtab_func_pdata.param_num;
        currNode->inst->Parent->id = countVariable;
    }
    countVariable++;

    currNode = get_next_inst(currNode);  // skip for FuncBegin



    HashSet *copyNumSet = HashSetInit(); // set for copy operation we don't use the number here
    InstNode *tailNode = get_next_inst(end->tail_node);
    //we don't want to use the CopyOperation's same id
    InstNode *copyNode = currNode;
    while(copyNode != tailNode){
        if(copyNode->inst->Opcode == CopyOperation){
            Value *copyDest = ins_get_dest(copyNode->inst)->alias;
            char *copyNum = &copyDest->name[1];
            //printf("string %s\n",copyNum);
            int num = atoi(copyNum);
            //printf("num %d\n",num);
            HashSetAdd(copyNumSet,(void *)num);
        }
        copyNode = get_next_inst(copyNode);
    }

    while (currNode != tailNode) {
        if (currNode->inst->Opcode != br && currNode->inst->Opcode != br_i1 && currNode->inst->Opcode != CopyOperation) {
            while(HashSetFind(copyNumSet,(void *)countVariable)){
                //printf("countVariable %d\n",countVariable);
                countVariable++;
            }
            if (currNode->inst->Opcode == Label) {
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
                    char newName[10];
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
                    insName[j] = '\0';
                    countVariable++;
                }
            }
        }
        currNode = get_next_inst(currNode);
    }

    clear_visited_flag(entry);
    //通过true false block的方式设置
    // 跳过funcBegin
    BasicBlock *tail = currentFunction->tail;
    currNode = get_next_inst( entry->head_node);
    InstNode *endNode = get_next_inst(end->tail_node);
    while(currNode != endNode){
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
            //printf("block %d\n",block->id);
            block->visited = true;
            InstNode *blockTail = block->tail_node;
            if(block == tail){
                blockTail = get_prev_inst(blockTail);
            }
            Value *blockTailInsValue = ins_get_dest(blockTail->inst);
            if(block->true_block){
                blockTailInsValue->pdata->instruction_pdata.true_goto_location = block->true_block->id;
            }
            if(block->false_block){
                blockTailInsValue->pdata->instruction_pdata.false_goto_location = block->false_block->id;
            }
        }
        currNode = get_next_inst(currNode);
    }
}

void showInstructionInfo(InstNode *instruction_list){
    InstNode *temp2 = instruction_list;

    /* 测试所有instruction list */
    for(;temp2 != NULL;temp2 = get_next_inst(temp2)){
        print_one_ins_info(temp2);
    }
    printf("---------- after print ins info  ---------\n");
}

void showBlockInfo(InstNode *instruction_list){
    InstNode *temp = get_next_inst(instruction_list);
    BasicBlock *block = temp->inst->Parent;
    //assert(block != NULL);
    clear_visited_flag(block);
    print_block_info(block);
    printf("--------- after print block info ---------\n");
}

void HashSetClean(HashSet *set){
    if(set == NULL){
        return;
    }
    //assert(set != NULL);
    HashSetFirst(set);
    for(void *key = HashSetNext(set); key != NULL; key = HashSetNext(set)){
        HashSetRemove(set,key);
    }
}

BasicBlock *newBlock(HashSet *prevBlocks,BasicBlock *suc){
    BasicBlock *newBlock = bb_create();
    HashSetFirst(prevBlocks);
    BasicBlock *prevBlock = NULL;
    for(prevBlock = HashSetNext(prevBlocks); prevBlock != NULL; prevBlock = HashSetNext(prevBlocks)){
        if(prevBlock->true_block == suc){
            prevBlock->true_block = newBlock;
        }else if(prevBlock->false_block == suc){
            prevBlock->false_block = newBlock;
        }
        HashSetAdd(newBlock->preBlocks,prevBlock);
    }

    newBlock->true_block = suc;
    //
    HashSetClean(suc->preBlocks);

    //然后block添加唯一的一个前驱为newBloc
    HashSetAdd(suc->preBlocks,newBlock);


    InstNode *prevTail = get_prev_inst(suc->head_node);
    InstNode *prevNext = suc->head_node;

    // 创建两个语句
    Instruction *newBlockLabel = ins_new_zero_operator(Label);
    Instruction *newBlockBr = ins_new_zero_operator(br);
    InstNode *newBlockLabelNode = new_inst_node(newBlockLabel);
    InstNode *newBlockBrNode = new_inst_node(newBlockBr);

    // 进行一些链接
    ins_insert_after(newBlockLabelNode,prevTail);
    ins_insert_after(newBlockBrNode,newBlockLabelNode);

    newBlockBrNode->list.next = &prevNext->list;
    prevNext->list.prev = &newBlockBrNode->list;

    bb_set_block(newBlock,newBlockLabelNode,newBlockBrNode);
    return newBlock;
}

InstNode *findNode(BasicBlock *block,Instruction *inst){
    InstNode *currNode = block->head_node;
    while(currNode != get_next_inst(block->tail_node)){
        if(currNode->inst == inst){
            return currNode;
        }
        currNode = get_next_inst(currNode);
    }
    return NULL;
}

bool isCalculationOperator(InstNode *instNode){
    for(int i = 0; i < sizeof(CalculationOpcodes) / sizeof(Opcode); i++){
        if(instNode->inst->Opcode == CalculationOpcodes[i]){
            return true;
        }
    }
    return false;
}

bool HashSetDifferent(HashSet *lhs,HashSet *rhs){
    //左边是小的 右边是大的
    unsigned int leftSize = HashSetSize(lhs);
    unsigned int rightSize = HashSetSize(rhs);
    if(leftSize != rightSize) return true;
    HashSetFirst(lhs);
    for(void *key = HashSetNext(lhs); key != NULL; key = HashSetNext(lhs)){
        if(!HashSetFind(rhs,key)) return true;
    }
    return false;
}


void calculateNonLocals(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = entry->head_node;
    InstNode *tailNode = end->tail_node;

    clear_visited_flag(entry);

    currentFunction->nonLocals = HashSetInit();
    HashSet *nonLocals = currentFunction->nonLocals;
    HashSet *killed = HashSetInit();
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
    printf("here!\n");
    HashSetDeinit(killed);
    clear_visited_flag(entry);
}


void valueReplaceAll(Value *oldValue, Value *newValue, Function *currentFunction){
    value_replaceAll(oldValue,newValue);
    InstNode *currNode = currentFunction->entry->head_node;
    InstNode *tailNode = currentFunction->tail->tail_node;
    while(currNode != tailNode){
        if(currNode->inst->Opcode == Phi){
            HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
            if(phiSet != NULL){
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    if(phiInfo->define == oldValue){
                        phiInfo->define = newValue;
                    }
                }
            }
        }
        currNode = get_next_inst(currNode);
    }
}

//valueReplaceAll but do not replace if it's User is cur
void valueReplaceWithout(Value *oldValue, Value *newValue, Value *cur, Function *currentFunction){
    // 使用两个Use的原因是需要维护Next的正确性
    if(oldValue->use_list != NULL){
        Use *use1 = oldValue->use_list;
        Use *use2 = use1->Next;
        while(use1 != NULL){
            Value *user = (Value *)use1->Parent;
            if(user != cur){
                value_add_use(newValue,use1);
                use1->Val = newValue;
            }
            use1 = use2;
            use2 = (use2 == NULL ? NULL : use2->Next);
        }
    }
    oldValue->use_list = NULL;
    InstNode *currNode = currentFunction->entry->head_node;
    InstNode *tailNode = currentFunction->tail->tail_node;
    while(currNode != tailNode){
        if(currNode->inst->Opcode == Phi){
            HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                if(phiInfo->define == oldValue){
                    phiInfo->define = newValue;
                }
            }
        }
        currNode = get_next_inst(currNode);
    }

}

bool isParam(Value *val, int paramNum){
    char *name = val->name;
    name++;
    int num = atoi(name);
    if(num <= paramNum - 1){
        return true;
    }
    return false;
}

void HashSetCopy(HashSet *dest,HashSet *src){
    HashSetFirst(src);
    for(void *key = HashSetNext(src); key != NULL; key = HashSetNext(src)){
        if(!HashSetFind(dest,key)){
            HashSetAdd(dest,key);
        }
    }
}

//TODO 利用Hash库实现
//TODO
unsigned hash_values(Vector *valueVector) {
    unsigned count = VectorSize(valueVector);
    unsigned volatile *Memory = (unsigned *)malloc(sizeof(unsigned ) * count);
    Value *value = NULL;
    for(int i = 0; i < count; i++) {
        VectorGet(valueVector,i,(void *)&value);
        //assert(value != NULL);
        if(isImmInt(value)){
            Memory[i] = (unsigned )value->pdata->var_pdata.iVal;
        }else if(isImmFloat(value)){
            Memory[i] = (unsigned )value->pdata->var_pdata.fVal;
        }else{
            Memory[i] = (unsigned )value;
        }
    }
    return HashMurMur32((void *)Memory,count);
}

bool isSySYFunction(Value *function){
    if(isInputFunction(function) || isOutputFunction(function) || isTimeFunction(function)){
        return true;
    }
    return false;
}

bool isTimeFunction(Value *function){
    int n = sizeof(TimeSySYFunctions) / sizeof(char *);
    for(int i = 0; i < n; i++){
        if(strcmp(function->name,TimeSySYFunctions[i]) == 0){
            return true;
        }
    }
    return false;
}

bool isInputFunction(Value *function){
    int n = sizeof(InputSySYFunctions) / sizeof(char *);
    for(int i = 0; i < n; i++){
        if(strcmp(function->name,InputSySYFunctions[i]) == 0){
            return true;
        }
    }
    return false;
}

bool isOutputFunction(Value *function){
    int n = sizeof(OutputSySYFunctions) / sizeof(char *);
    for(int i = 0; i < n; i++){
        if(strcmp(function->name,OutputSySYFunctions[i]) == 0){
            return true;
        }
    }
    return false;
}


//must satisfy the form:
//a   =  icmp ... ...
//b =  a xor treu
//c = zext b
bool JudgeXor(InstNode *insList){
    InstNode *tempNode = insList;
    while(tempNode != NULL){
        if(tempNode->inst->Opcode == XOR){
            Value *curLhs  = ins_get_lhs(tempNode->inst);
            Value *curDest = ins_get_dest(tempNode->inst);

            //see if satisfy the case
            InstNode *prevNode = get_prev_inst(tempNode);
            Value *prevDest = ins_get_dest(prevNode->inst);
            //前面这个要是0

            //
            Value *prevRhs = ins_get_rhs(prevNode->inst);
            //assert(prevRhs->VTy->ID == Int && prevRhs->pdata->var_pdata.iVal == 0);
            //assert(prevNode->inst->Opcode == NOTEQ || prevNode->inst->Opcode == EQ);
            //assert(prevDest == curLhs);

            InstNode *nextNode = get_next_inst(tempNode);
            Value *nextLhs = ins_get_lhs(nextNode->inst);
            //assert(nextNode->inst->Opcode == zext);
            //assert(nextLhs == curDest);

            //also we need to adjust the dest to be var_int
            curDest->VTy->ID = Var_INT;
        }
        tempNode = get_next_inst(tempNode);
    }
}

void combineZext(InstNode *insList){
    InstNode *tempNode = insList;
    while(tempNode != NULL){
        if(tempNode->inst->Opcode == zext){
            Value *tempDest = ins_get_dest(tempNode->inst);
            Value *tempLhs = ins_get_lhs(tempNode->inst);

            //TODO may occur in Phi???
            value_replaceAll(tempDest,tempLhs);

            //delete curNode
            InstNode *nextNode = get_next_inst(tempNode);
            deleteIns(tempNode);
            tempNode = nextNode;
        }else{
            tempNode = get_next_inst(tempNode);
        }
    }
}

bool JudgeIcmp(InstNode *icmp){
    Value *icmpDest = ins_get_dest(icmp->inst);
    Use *uses = icmpDest->use_list;
    while(uses != NULL){
        User *user = uses->Parent;
        Instruction *ins = (Instruction *)user;
        if(ins->Opcode != br && ins->Opcode != br_i1){
            return true;
        }
        uses = uses->Next;
    }
    return false;
}


bool isSimpleOperator(InstNode *instNode){
    for (int i = 0; i < sizeof(simpleOpcodes) / sizeof(Opcode); i++){
        if (instNode->inst->Opcode == simpleOpcodes[i]){
            return true;
        }
    }
    return false;
}

bool isSame(Value *left, Value *right){
    if(left == NULL || right == NULL) return false;
    if(isImmInt(left) && isImmInt(right) && (left->pdata->var_pdata.iVal == right->pdata->var_pdata.iVal)){
        return true;
    }else if(isImmFloat(left) && isImmFloat(right) && (left->pdata->var_pdata.fVal == right->pdata->var_pdata.fVal)){
        return true;
    }else if(left == right){
        return true;
    }
    return false;
}

bool isSameLargeType(Value *left, Value *right){
    if(isInt(left) && isInt(right)){
        return true;
    }else if(isFloat(left) && isFloat(right)){
        return true;
    }
    return false;
}


//TODO Think carefully
//void SwapOperand(InstNode *instNode){
//    //swap Operand regardless of the correctness
//    User *user = &instNode->inst->user;
//
//    //
//}


Function *ReconstructFunction(InstNode *inst_list){
    //
    InstNode *tempNode = inst_list;
    Function *start = NULL;
    Function *prev = NULL;
    Function *curr = NULL;

    //找到第一个funcHead
    while(tempNode->inst->Opcode != FunBegin){
        tempNode = get_next_inst(tempNode);
    }

    //assert(tempNode->inst->Opcode == FunBegin);


    start = tempNode->inst->Parent->Parent;
    prev = start;

    tempNode = get_next_inst(tempNode);
    while(tempNode != NULL){
        if(tempNode->inst->Opcode == FunBegin){
            curr = tempNode->inst->Parent->Parent;
            prev->Next = curr;
            prev = curr;
        }
        tempNode = get_next_inst(tempNode);
    }

    return start;
}

bool isLoopInvariant(Loop *loop,Value *var){
    BasicBlock *loopEntry = loop->head;
    Function *function = loopEntry->Parent;
    BasicBlock *entry = function->entry;
    int paraNum = entry->head_node->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;

    printf("paramNum is %d\n",paraNum);

    if(isParam(var,paraNum)){
        return true;
    }else{
        Instruction *ins = (Instruction *)var;
        BasicBlock *block = ins->Parent;
        if(!HashSetFind(loop->loopBody,block)){
            return true;
        }else{
            return false;
        }
    }
}

bool returnValueNotUsed(InstNode *instNode){
    //assert(instNode->inst->Opcode == Call);

    Value *returnValue = ins_get_dest(instNode->inst);
    if(returnValue->use_list == NULL){
        return true;
    }
    return false;
}

//block dominated target or not?
//if block dominates target return true
bool isDominated(BasicBlock *block, BasicBlock *target){
    if(HashSetFind(target->dom,block)){
        return true;
    }
    return false;
}

//被pos支配的对old的use全部替换为对new的使用
bool specialValueReplace(Value *old, Value *new, BasicBlock *pos){
    Use *oldUses = old->use_list;
    while(oldUses != NULL){
        Use *tempNext = oldUses->Next;
        //see the user of current
        Instruction *curIns = (Instruction *)oldUses->Parent;
        BasicBlock *block = curIns->Parent;
        if(block->dom!=NULL && isDominated(pos,block)){
            value_add_use(new,oldUses);
            oldUses->Val = new;
        }
        oldUses = tempNext;
    }

    //还需要判断后续的phi函数是否是被支配的
    Function *func = pos->Parent;
    InstNode *phiNode = func->entry->head_node;
    InstNode *funcTail = func->tail->tail_node;

    while(phiNode != funcTail){
        if(phiNode->inst->Opcode == Phi){
            BasicBlock *phiBlock = phiNode->inst->Parent;
            if((phiBlock->dom!=NULL && isDominated(pos,phiBlock) && phiBlock != pos) || HashSetFind(phiBlock->preBlocks, pos)){
                HashSet *phiSet = phiNode->inst->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    Value *define = phiInfo->define;
                    if(define == old){
                        phiInfo->define = new;
                    }
                }
            }
        }
        phiNode = get_next_inst(phiNode);
    }
}

void dfsTravelCfg(Vector *vector, BasicBlock *block){
    block->visited = true;
    VectorPushBack(vector,block);
    if(block->true_block && block->true_block->visited == false){
        dfsTravelCfg(vector,block->true_block);
    }
    if(block->false_block && block->false_block->visited == false){
        dfsTravelCfg(vector,block->false_block);
    }
}

//reverse post order travel the cfg
//reverse post order == depth first
//void RPOCfg(Function *currentFunction){
//    BasicBlock *entry = currentFunction->entry;
//    clear_visited_flag(entry);
//    currentFunction->RPOBlocks = VectorInit(10);
//    dfsTravelCfg(currentFunction->RPOBlocks,entry);
//
//    //
//    int count = 0;
//    printf("rpo of %s:",currentFunction->name);
//    int n = VectorSize(currentFunction->RPOBlocks);
//    BasicBlock *block = NULL;
//    for(int i = 0; i < n; i++){
//        VectorGet(currentFunction->RPOBlocks,i,(void *)&block);
//        InstNode *headNode = block->head_node;
//        InstNode *tailNode = get_next_inst(block->tail_node);
//        while(headNode != tailNode){
//            headNode->inst->i = count;
//            count++;
//            headNode = get_next_inst(headNode);
//        }
//        //assert(block != NULL);
//        printf(" block %d",block->id);
//    }
//    printf("\n");
//}

//top sort
void topCfg(Function *currentFunction){
    //首先我们计算每一个BasicBlock的Indegree HashMap BasicBlock -> int
    //对当前block
    HashMap *indegree = HashMapInit();


    HashSet *workList = HashSetInit();
    BasicBlock *entry = currentFunction->entry;
    clear_visited_flag(entry);
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);
        block->visited = true;
        int n = HashSetSize(block->preBlocks);

        //计算indegree的时候不能计算回边
        HashSetFirst(block->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
            if(HashSetFind(preBlock->dom,block)){
                n = n - 1;
            }
        }

        HashMapPut(indegree,block,(void *)n);
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }
        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
    HashSetDeinit(workList);

    HashMapFirst(indegree);
    for(Pair *pair = HashMapNext(indegree); pair != NULL; pair = HashMapNext(indegree)){
        BasicBlock *block = pair->key;
        int in = (int)pair->value;
        //printf("b %d indegree %d\n",block->id, in);
    }

    currentFunction->ToPoBlocks = VectorInit(10);
    Vector *vector = currentFunction->ToPoBlocks;
    BasicBlock *exit = currentFunction->tail;
    HashMapRemove(indegree,exit);

    //只有可能是现在修改的后继
    HashSet *modified = HashSetInit();
    HashSetAdd(modified,entry);

    //
    while(HashMapSize(indegree) != 0){
        //printf("one time!\n");
        //find the
        HashSetFirst(modified);
        for(BasicBlock *modifiedBlock = HashSetNext(modified); modifiedBlock != NULL; modifiedBlock = HashSetNext(modified)){
            int in = (int)HashMapGet(indegree,modifiedBlock);
            //printf("modified block %d in %d\n",modifiedBlock->id,in);
            if(in == 0){
                //printf("block %d in degree is 0!!\n",modifiedBlock->id);
                VectorPushBack(vector,modifiedBlock);
                //OK now we remove this Block from HashMap
                HashMapRemove(indegree,modifiedBlock);
                HashSetRemove(modified,modifiedBlock);

                if(modifiedBlock->true_block)
                    //assert(modifiedBlock->true_block != modifiedBlock->false_block);

                //minus indegree for it's successors;
                if(modifiedBlock->true_block && modifiedBlock->true_block != exit && !HashSetFind(modifiedBlock->dom,modifiedBlock->true_block)){
                    int n = (int) HashMapGet(indegree,modifiedBlock->true_block);
                    HashMapRemove(indegree,modifiedBlock->true_block);
                    n = n - 1;
                    HashMapPut(indegree,modifiedBlock->true_block,(void *)n);
                    HashSetAdd(modified,modifiedBlock->true_block);
                    //printf("true is %d inde : %d\n",block->true_block->id,n);
                }

                if(modifiedBlock->false_block && modifiedBlock->false_block != exit && !HashSetFind(modifiedBlock->dom,modifiedBlock->false_block)){
                    int n = (int) HashMapGet(indegree,modifiedBlock->false_block);
                    HashMapRemove(indegree,modifiedBlock->false_block);
                    n = n - 1;
                    HashMapPut(indegree,modifiedBlock->false_block,(void *)n);
                    HashSetAdd(modified,modifiedBlock->false_block);
                }
                break;
            }
        }
    }

    HashSetDeinit(modified);
    HashMapDeinit(indegree);
    VectorPushBack(vector,exit);

    int n = VectorSize(vector);
    BasicBlock *block = NULL;
    printf("topo:");

    int count = 0;
    for(int i = 0; i < n; i++){
        VectorGet(vector,i,(void *)&block);

        InstNode *blockHead = block->head_node;
        InstNode *blockTail = block->tail_node;
        InstNode *nextNode = get_next_inst(blockTail);
        while(blockHead != nextNode){
            blockHead->inst->i = count;
            count++;
            blockHead = get_next_inst(blockHead);
        }

        printf(" %d",block->id);
    }
    printf("\n");
}

//only clean the use_list and the CFG is NOT CHANGING!!!
void cleanBlock(BasicBlock *block){
    InstNode *blockHead = block->head_node;
    InstNode *blockTail = block->tail_node;
    InstNode *blockNext = get_next_inst(blockTail);

    while(blockHead != blockNext){
        InstNode *tempNode = get_next_inst(blockHead);
        deleteIns(blockHead);
        blockHead = tempNode;
    }
}

bool containFloat(InstNode *node){
    while(node != NULL){
        Value *dest = ins_get_dest(node->inst);
        if(isLocalVarFloat(dest)){
            return true;
        }
        node = get_next_inst(node);
    }
    return false;
}

bool usedInPhi(Value *value, Function *function){
    //assert(value != NULL && function != NULL);
    BasicBlock *entry = function->entry;
    BasicBlock *tail = function->tail;
    InstNode *funcHead = entry->head_node;
    InstNode *funcTail = tail->tail_node;
    while(funcHead != funcTail){
        if(funcHead->inst->Opcode == Phi){
            HashSet *phiSet = funcHead->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                Value *define = phiInfo->define;
                if(define == value){
                    return true;
                }
            }
        }
        funcHead = get_next_inst(funcHead);
    }
    return false;
}

//TODO could have Bug???
bool isRegionalConstant(Value *value,struct Loop *loop){
    //assert(value != NULL);

    //assert(loop != NULL);


    Function *currentFunction = loop->head->Parent;

    int paramNum = currentFunction->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    if(isImmInt(value) || isParam(value,paramNum)) return true;

    //must be an int
    if(!isInt(value)) return false;

    //find the instruction
    Instruction *ins = (Instruction *)value;

    //must come from + - * / mod phi
    switch (ins->Opcode) {
     case Add:case Sub:case Mul:case Div:
     case Mod:case Phi: break;
     default: return false;
    }

    //if the value comes from
    if(ins->Opcode == Phi){

    }
    //must be defined outside the loop
    BasicBlock *insParent = ins->Parent;

    if(HashSetFind(loop->loopBody,insParent)){
        return false;
    }

    //
    return true;
}

bool hasSingleExit(Function *currentFunction){
    //收集一下All BasicBlocks
    HashSet *blocks = HashSetInit();
    HashSet *workList = HashSetInit();

    BasicBlock *entry = currentFunction->entry;
    clear_visited_flag(entry);

    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);
        block->visited = true;
        HashSetAdd(blocks,block);
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }
        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }


    //从exit回去能否找到所有的block
    InstNode *funcTail = currentFunction->tail->tail_node;
    while(funcTail->inst->Opcode != Return){
        funcTail = get_prev_inst(funcTail);
    }

    clear_visited_flag(entry);

    BasicBlock *exit = funcTail->inst->Parent;
    HashSetClean(workList);
    HashSetAdd(workList,exit);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);
        block->visited = true;
        HashSetFirst(block->preBlocks);
        for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
            if(preBlock->visited == false){
                HashSetAdd(workList,preBlock);
            }
        }
    }

    bool hasSingleExit = true;
    //check now if the blocks all visited;
    HashSetFirst(blocks);
    for(BasicBlock *block = HashSetNext(blocks); block != NULL; block = HashSetNext(blocks)){
        if(block->visited == false){
            hasSingleExit = false;
        }
    }

    HashSetDeinit(blocks);
    HashSetDeinit(workList);
    return hasSingleExit;
}