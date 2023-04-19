//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
// 出现在IR里面的我们都要进行分析了 除了alloca
// 对于alloca后端是自行建立了一个表去查找位置
const Opcode invalidOpcodes[] = {FunBegin, Label, ALLBEGIN, Alloca, tmp, zext, MEMCPY, zeroinitializer, GLOBAL_VAR, FunEnd};
const Opcode simpleOpcodes[] = {Add, Sub, Mul, Div, Mod};
const Opcode compareOpcodes[] = {EQ,NOTEQ,LESS, LESSEQ,GREAT,GREATEQ};
bool isValidOperator(InstNode *insNode){
    for (int i = 0; i < sizeof(invalidOpcodes) / sizeof(Opcode); i++) {
        if (insNode->inst->Opcode == invalidOpcodes[i]) {
            return false;
        }
    }
    return true;
}

bool isCalculationOperator(InstNode *inst){
    for (int i = 0; i < sizeof(simpleOpcodes) / sizeof(Opcode); i++){
        if (inst->inst->Opcode == simpleOpcodes[i]){
            return true;
        }
    }
    return false;
}

bool isCompareOperator(InstNode *insNode){
    for(int i = 0; i < sizeof(compareOpcodes) / sizeof(Opcode); i++){
        if(insNode->inst->Opcode == compareOpcodes[i]){
            return true;
        }
    }
    return false;
}

void clear_visited_flag(BasicBlock *block){
    InstNode *currNode = block->head_node;

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

        // TODO 还有很多function我们没有设置Type
        if(headNode->inst->Opcode == Call){
            Value *function = ins_get_lhs(headNode->inst);
            if(strcmp(function->name,"getint") == 0){
                insValue->VTy->ID = Var_INT;
            }else if(strcmp(function->name,"getfloat") == 0){
                insValue->VTy->ID = Var_FLOAT;
            }
        }
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
        if (currNode->inst->Opcode != Br && currNode->inst->Opcode != Br_i1) {

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


    //
    InstNode *prevTail = get_prev_inst(block->head_node);
    InstNode *prevNext = block->head_node;

    // 创建两个语句
    Instruction *newBlockLabel = ins_new_zero_operator(Label);
    Instruction *newBlockBr = ins_new_zero_operator(Br);
    InstNode *newBlockLabelNode = new_inst_node(newBlockLabel);
    InstNode *newBlockBrNode = new_inst_node(newBlockBr);

    // 进行一些链接
    ins_insert_after(newBlockLabelNode,prevTail);
    ins_insert_after(newBlockBrNode,newBlockLabelNode);

    newBlockBrNode->list.next = &prevNext->list;
    prevNext->list.prev = &newBlockBrNode->list;

    return newBlock;
}