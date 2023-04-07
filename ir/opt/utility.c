//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
// 出现在IR里面的我们都要进行分析了 除了alloca
// 对于alloca后端是自行建立了一个表去查找位置
bool isValidOperator(InstNode *insNode){
    switch (insNode->inst->Opcode) {
        case FunBegin:
            return false;
        case Label:
            return false;
        case ALLBEGIN:
            return false;
        case Alloca:
            return false;
        case tmp:
            return false;
        case zext:
            return false;
        case MEMCPY:
            return false;
        case zeroinitializer:
            return false;
        case GLOBAL_VAR:
            return false;
        case FunEnd:
            return false;
    }
    return true;
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

void correctType(Function *currentFunction){
    InstNode *headNode = currentFunction->entry->head_node;
    while(headNode != get_next_inst(currentFunction->tail->tail_node)){
        Value *insValue = ins_get_value(headNode->inst);
        if(headNode->inst->Opcode == Alloca && insValue->VTy->ID == AddressTyID){
            insValue->VTy->ID = Var_INT;
            printf("correct!!!!\n");
        }
        headNode = get_next_inst(headNode);
    }
}