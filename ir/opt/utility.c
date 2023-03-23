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
                }else{
                    // 如果alloca的是NULL那么说明应该是保存返回值的
                    // 我们也简单的设置成为Int的Type
                    Value *insValue = ins_get_value(currNode->inst);
                    insValue->VTy->ID = Int;
                }
                break;
            }
            case GMP:{
                Value *insValue = ins_get_value(currNode->inst);
                //设置这个value的type
                insValue->VTy->ID = ArrayTyID;
                break;
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
        case GMP:
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
    Function *parent = block->Parent;

    assert(parent->entry == block);
    BasicBlock *tail = parent->tail;
    InstNode *currNode = block->head_node;
    InstNode *tailNode = tail->tail_node;

    BasicBlock *prevBlock = NULL;
    while(currNode != get_next_inst(tailNode)){
        BasicBlock *currNodeParent = currNode->inst->Parent;
        if(currNodeParent != prevBlock){
            currNodeParent->visited = false;
            prevBlock = currNodeParent;
        }
        currNode = get_next_inst(currNode);
    }
}