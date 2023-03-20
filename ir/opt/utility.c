//
// Created by Valerian on 2023/3/19.
//

#include "utility.h"
void correctType(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *exit = currentFunction->tail;


    InstNode *currNode = entry->head_node;
    while(currNode != get_next_inst(exit->tail_node)){

        switch (currNode->inst->Opcode) {
            case Alloca:{
                Value *lhs = ins_get_lhs(currNode->inst);
                if(lhs != NULL){
                    if(isArray(lhs)){
                        //还没有判断到底是int数组还是float数组
                        Value *insValue = ins_get_value(currNode->inst);
                        insValue->VTy->ID = ArrayTyID;
                    }else if(isVar(lhs)){
                        Value *insValue = ins_get_value(currNode->inst);
                        if(isIntType(lhs->VTy)){
                            insValue->VTy->ID = Int;
                        }else if(isFloatType(lhs->VTy)){
                            insValue->VTy->ID = Float;
                        }
                    }
                }
            }
            case GMP:{
                Value *insValue = ins_get_value(currNode->inst);
                //设置这个value的type
                insValue->VTy->ID = ArrayTyID;
            }

        }
        currNode = get_next_inst(currNode);
    }
}

bool isValidOperator(InstNode *insNode){
    switch (insNode->inst->Opcode) {
        case FunBegin:
            return false;
        case Label:
            return false;
        case ALLBEGIN:
            return false;
    }
    return true;
}