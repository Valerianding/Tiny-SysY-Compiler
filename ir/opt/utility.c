//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
// 出现在IR里面的我们都要进行分析了 除了alloca
// 对于alloca后端是自行建立了一个表去查找位置
const Opcode invalidOpcodes[] = {FunBegin, Label, ALLBEGIN, Alloca, tmp, zext, MEMCPY, zeroinitializer, GLOBAL_VAR, FunEnd};
const Opcode compareOpcodes[] = {EQ,NOTEQ,LESS, LESSEQ,GREAT,GREATEQ};
const Opcode hasNoDestOpcodes[] = {br,br_i1,br_i1_true,br_i1_false,Store,Return,Label,GIVE_PARAM};
const Opcode CriticalOpcodes[] = {Return,Call,Store,br,GIVE_PARAM,MEMCPY,MEMSET};
const Opcode CalculationOpcodes[] = {Add,Sub,Mul,Mod,Div,GEP};
const char* InputSySYFunctions[] = {"getint","getfloat","getch","getarray","getfarray"};
const char* OutputSySYFunctions[] = {"putint","putch","putarray","putfloat","putfarray","putf"};
const char* TimeSySYFunctions[] = {"starttime","stoptime"};
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

//        // TODO 还有很多function我们没有设置Type
//        if(headNode->inst->Opcode == Call){
//            Value *function = ins_get_lhs(headNode->inst);
//            if(strcmp(function->name,"getint") == 0){
//                insValue->VTy->ID = Var_INT;
//            }else if(strcmp(function->name,"getfloat") == 0){
//                insValue->VTy->ID = Var_FLOAT;
//            }
//        }
        //我们还需要对到底是从数组还是局部变量定义的进行区分
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
            printf("string %s\n",copyNum);
            int num = atoi(copyNum);
            printf("num %d\n",num);
            HashSetAdd(copyNumSet,(void *)num);
        }
        copyNode = get_next_inst(copyNode);
    }

    while (currNode != tailNode) {
        if (currNode->inst->Opcode != br && currNode->inst->Opcode != br_i1 && currNode->inst->Opcode != CopyOperation) {
            while(HashSetFind(copyNumSet,(void *)countVariable)){
                printf("countVariable %d\n",countVariable);
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
            printf("block %d\n",block->id);
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
    assert(block != NULL);
    clear_visited_flag(block);
    print_block_info(block);
    printf("--------- after print block info ---------\n");
}

void HashSetClean(HashSet *set){
    if(set == NULL){
        return;
    }
    assert(set != NULL);
    HashSetFirst(set);
    for(void *key = HashSetNext(set); key != NULL; key = HashSetNext(set)){
        HashSetRemove(set,key);
    }
}

BasicBlock *newBlock(HashSet *prevBlocks,BasicBlock *block){
    BasicBlock *newBlock = bb_create();
    HashSetFirst(prevBlocks);
    BasicBlock *prevBlock = NULL;
    for(prevBlock = HashSetNext(prevBlocks); prevBlock != NULL; prevBlock = HashSetNext(prevBlocks)){
        if(prevBlock->true_block == block){
            prevBlock->true_block = newBlock;
        }else if(prevBlock->false_block == block){
            prevBlock->false_block = newBlock;
        }
        HashSetAdd(newBlock->preBlocks,prevBlock);
    }

    newBlock->true_block = block;
    //
    HashSetClean(block->preBlocks);

    //然后block添加唯一的一个前驱为newBloc
    HashSetAdd(block->preBlocks,newBlock);


    InstNode *prevTail = get_prev_inst(block->head_node);
    InstNode *prevNext = block->head_node;

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

unsigned long int hash_values(Vector *valueVector) {
    int count = VectorSize(valueVector);
    unsigned int seed = 0x9747b28c;
    unsigned int m = 0x5bd1e995;
    unsigned int r = 24;
    unsigned int h = seed ^ (count * 4);

    Value *val = NULL;
    for(int i = 0; i < count; i++) {
        VectorGet(valueVector,i,(void *)&val);
        assert(val != NULL);
        unsigned int k;
        //尝试在hash里面解决掉这个立即数的问题
        if(isImm(val)){
            if(isImmInt(val)){
                k = val->pdata->var_pdata.iVal;
            }else{
                k = val->pdata->var_pdata.fVal;
            }
        }else{
            k = *((unsigned int *)val);
        }

        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (unsigned long int)h;
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
            assert(prevRhs->VTy->ID == Int && prevRhs->pdata->var_pdata.iVal == 0);
            assert(prevNode->inst->Opcode == NOTEQ || prevNode->inst->Opcode == EQ);
            assert(prevDest == curLhs);

            InstNode *nextNode = get_next_inst(tempNode);
            Value *nextLhs = ins_get_lhs(nextNode->inst);
            assert(nextNode->inst->Opcode == zext);
            assert(nextLhs == curDest);

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