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
const Opcode CalculationOpcodes[] = {Add,Sub,Mul,Mod,Div};
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
        if(headNode->inst->Opcode == GEP){
            // get 出来的这个变量
          insValue->IsFromArray = 1;
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

void renameVariabels(Function *currentFunction) {
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

    currNode = get_next_inst(currNode);
    while (currNode != get_next_inst(end->tail_node)) {
        if (currNode->inst->Opcode != br && currNode->inst->Opcode != br_i1 && currNode->inst->Opcode != CopyOperation) {
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
    while(currNode != get_next_inst(end->tail_node)){
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
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