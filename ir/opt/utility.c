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
                            insValue->VTy->ID = Var_INT;
                        }else if(isFloatType(lhs->VTy)){
                            insValue->VTy->ID = Var_FLOAT;
                        }
                    }
                }else{
                    // 如果alloca的是NULL那么说明应该是保存返回值的
                    // 我们也简单的设置成为Int的Type
                    Value *insValue = ins_get_value(currNode->inst);
                    insValue->VTy->ID = Int;
                }
                break;
            }
            case GEP:{
                Value *insValue = ins_get_value(currNode->inst);
                //设置这个value的type
                insValue->VTy->ID = ArrayTyID;
                break;
            }
            case bitcast:{
                //
                Value *insValue = ins_get_value(currNode->inst);
                insValue->VTy->ID = ArrayTyID;
                Value *lhs = ins_get_lhs(currNode->inst);
                lhs->VTy->ID = ArrayTyID;
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
        case bitcast:
            return false;
        case GEP:
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
        case Phi:
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