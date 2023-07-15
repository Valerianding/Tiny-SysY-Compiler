//
// Created by Valerian on 2023/5/25.
//


#include "other.h"


//for integer-divide-optimization only
//we should analyze algorithm identical

bool checkValid(Value *lhs, Value *rhs, InstNode *instNode){
    //check if the instructions
    Instruction *lhsIns = (Instruction *)lhs;
    Instruction *rhsIns = (Instruction *)rhs;

    //we need to prove them in the same block & only used here?

    //TODO 我们先让条件严格一点也就是说不仅要在一个block里面还需要满足只能使用
    Value *curValue = ins_get_dest(instNode->inst);

    bool valid = true;

    Use *lhsUses = lhs->use_list;
    while(lhsUses != NULL){
        User *lhsUsers = lhsUses->Next;
        Instruction *lhsUserIns = (Instruction *)lhsUsers;
        if(lhsUserIns != instNode->inst){
            valid = false;
        }
        lhsUses = lhsUses->Next;
    }


    Use *rhsUses = rhs->use_list;
    while(rhsUses != NULL){
        User *rhsUsers = rhsUses->Next;
        Instruction *rhsUserIns = (Instruction *)rhsUsers;
        if(rhsUserIns != instNode->inst){
            valid = false;
        }
        rhsUses = rhsUses->Next;
    }


}


bool AlgorithmIdentical(BasicBlock *block){
    //backwards

    //if two operand is produced by the same Opcode and the same right Operand
    //we can first
    InstNode *tailNode = block->tail_node;
    InstNode *headNode = block->head_node;

    while(tailNode != headNode){
        if(isCalculationOperator(tailNode)){
            //we only consider add, div, mod, sub, mul
            Value *lhs = ins_get_lhs(tailNode->inst);
            Value *rhs = ins_get_rhs(tailNode->inst);

            //OK let's find their instructions
            Instruction *lhsIns = (Instruction *)lhs;
            Instruction *rhsIns = (Instruction *)rhs;

            //we need to make sure that all the instructions all in the same block!!


        }
        tailNode = get_prev_inst(tailNode);
    }
}


