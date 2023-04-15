//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
// 出现在IR里面的我们都要进行分析了 除了alloca
// 对于alloca后端是自行建立了一个表去查找位置
const Opcode invalidOpcodes[] = {FunBegin, Label, ALLBEGIN, Alloca, tmp, zext, MEMCPY, zeroinitializer, GLOBAL_VAR, FunEnd};
const Opcode simpleOpcodes[] = {Add,Sub,Mul,Div,Module};
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