//
// Created by Valerian on 2023/5/25.
//


#include "other.h"


//for integer-divide-optimization only
//we should analyze algorithm identical
const Opcode calculationOpcodes[] = {Add, Sub, Mul, Div, Mod};
const Opcode lowerOpcodes[] = {Add,Sub};
const Opcode higherOpcodes[] = {Mod, Div, Mul};

bool isLowerOpcode(Instruction *ins){
    int n = sizeof(lowerOpcodes) / sizeof(Opcode);
    for(int i = 0; i < n; i++){
        if(ins->Opcode == lowerOpcodes[i]){
            return true;
        }
    }
    return false;
}


bool isHigherOpcodes(Instruction *ins){
    int n = sizeof(higherOpcodes) / sizeof(Opcode);
    for(int i = 0; i < n; i++){
        if(ins->Opcode == higherOpcodes[i]){
            return true;
        }
    }
    return false;
}


Value *sameOperation(Instruction *ins1, Instruction *ins2) {
    //the two opcode already is the same
    //check if the instruction have same operation
    //that means only one operand is different and the other is same
    //and also we should consider the Opcode

    //if true it returns the same Operand
    //if not it returns NULL
    Value *ins1Lhs = ins_get_lhs(ins1);
    Value *ins1Rhs = ins_get_rhs(ins1);

    Value *ins2Lhs = ins_get_lhs(ins2);
    Value *ins2Rhs = ins_get_rhs(ins2);

    //check if there is same
    if(isSame(ins1Lhs,ins2Lhs)|| isSame(ins1Rhs,ins2Rhs)) {
        return ins1Lhs == ins2Lhs ? ins1Lhs : ins1Rhs;
    }

    if(isSame(ins1Lhs,ins2Rhs)|| isSame(ins1Rhs,ins2Lhs)){
        //on this condition we need the opcode to be add or mul
        if(ins1->Opcode == Add || ins1->Opcode == Mul){
            return ins1Lhs == ins2Rhs ? ins1Lhs : ins1Rhs;
        }
    }

    //
    return NULL;
}


Value *checkValid(InstNode *instNode){
    Value *lhs = ins_get_lhs(instNode->inst);

    Value *rhs = ins_get_rhs(instNode->inst);

    Value *dest = ins_get_dest(instNode->inst);

    //check if the instructions
    Instruction *lhsIns = (Instruction *)lhs;
    Instruction *rhsIns = (Instruction *)rhs;

    //后面的Opcode必须是lower的 前面的必须是Higher的才能满足结合律
    if(!isLowerOpcode(instNode->inst) || !isHigherOpcodes(rhsIns) || !isHigherOpcodes(lhsIns)) return NULL;

    //same type
    if((lhs->VTy->ID != rhs->VTy->ID) || (lhs->VTy->ID != dest->VTy->ID))  return NULL;

    //because we don't want type to cause any trouble it must satisfy the same type for all
    Value *lhsLhs = ins_get_lhs(lhsIns);
    Value *lhsRhs = ins_get_rhs(lhsIns);
    Value *rhsLhs = ins_get_lhs(rhsIns);
    Value *rhsRhs = ins_get_rhs(rhsIns);

    if(!isSameLargeType(lhsLhs,lhsRhs) || !isSameLargeType(lhsLhs,lhs)) return NULL;
    if(!isSameLargeType(rhsLhs,rhsRhs) || !isSameLargeType(rhsLhs,rhs)) return NULL;

    //same Opcode
    if(lhsIns->Opcode != rhsIns->Opcode) return NULL;

    //same block
    if(lhsIns->Parent != rhsIns->Parent || instNode->inst->Parent != lhsIns->Parent){
        return NULL;
    }


    bool valid = true;

    Use *lhsUses = lhs->use_list;
    while(lhsUses != NULL){
        User *lhsUsers = lhsUses->Parent;
        Instruction *lhsUserIns = (Instruction *)lhsUsers;
        if(lhsUserIns != instNode->inst){
            valid = false;
        }
        lhsUses = lhsUses->Next;
    }


    Use *rhsUses = rhs->use_list;
    while(rhsUses != NULL){
        User *rhsUsers = rhsUses->Parent;
        Instruction *rhsUserIns = (Instruction *)rhsUsers;
        if(rhsUserIns != instNode->inst){
            valid = false;
        }
        rhsUses = rhsUses->Next;
    }

    if(!valid) return NULL;

    //check if both have same operand and the operand
    Value *sameOperand = sameOperation(lhsIns,rhsIns);

    if(sameOperand == NULL) return false;

    //OK now this means
    return sameOperand;
}


bool AlgorithmIdentical(BasicBlock *block) {

    //
    bool effective = false;
    //backwards

    //if two operand is produced by the same Opcode and the same right Operand
    //we can first
    InstNode *tailNode = block->tail_node;
    InstNode *headNode = block->head_node;

    while (tailNode != headNode) {
        if (isCalculationOperator(tailNode)) {
            //we only consider add, div, mod, sub, mul
            Value *sameOperand = checkValid(tailNode);
            if (sameOperand != NULL) {
                Function *parent = block->Parent;
                printf("same operand %s in function %s\n", sameOperand->name, parent->name);


                //需要判断是 * 还是 / ;
                //%1 = a / 2;
                //%2 = b / 2;
                //%3 = %1 + %2

                //
                //%3 = a + b
                //OK now we find same operand
                //first find the different operand of each instruction
                Value *lhs = ins_get_lhs(tailNode->inst);
                Instruction *lhsIns = (Instruction *) lhs;

                Value *rhs = ins_get_rhs(tailNode->inst);
                Instruction *rhsIns = (Instruction *) rhs;


                assert(lhsIns->Opcode == rhsIns->Opcode);
                if(lhsIns->Opcode == Div){
                    return false;
                }else if(lhsIns->Opcode == Mul){
                    Value *lhsOther = ins_get_lhs(lhsIns) == sameOperand ? ins_get_rhs(lhsIns) : ins_get_lhs(lhsIns);
                    Value *rhsOther = ins_get_lhs(rhsIns) == sameOperand ? ins_get_rhs(rhsIns) : ins_get_lhs(rhsIns);

                    //replace use of the dest with other
                    Value *lhsDest = ins_get_dest(lhsIns);
                    Value *rhsDest = ins_get_dest(rhsIns);


                    //
                    valueReplaceAll(lhsDest, lhsOther, block->Parent);
                    valueReplaceAll(rhsDest, rhsOther, block->Parent);

                    //OK and this two instruction is not needed
                    InstNode *lhsNode = findNode(lhsIns->Parent, lhsIns);
                    InstNode *rhsNode = findNode(rhsIns->Parent, rhsIns);

                    deleteIns(lhsNode);
                    deleteIns(rhsNode);


                    assert(lhsIns->Opcode == rhsIns->Opcode);

                    //new a node right after the currentNode

                    //Value1 is currNode's dest
                    Value *curDest = ins_get_dest(tailNode->inst);
                    Instruction *binaryInstruction = ins_new_binary_operator(lhsIns->Opcode, curDest, sameOperand);
                    InstNode *newNode = new_inst_node(binaryInstruction);
                    newNode->inst->Parent = tailNode->inst->Parent;

                    //set name so that we can rename this instruction
                    newNode->inst->user.value.name = (char *) malloc(sizeof(char) * 10);
                    strcpy(newNode->inst->user.value.name, "%new");

                    //attach
                    ins_insert_after(newNode, tailNode);

                    effective = true;
                }else{
                    assert(false);
                }
            }
        }
        tailNode = get_prev_inst(tailNode);
    }
    return effective;
}


bool AlgorithmEliminate(Function *function){
    //

    HashSet *workList = HashSetInit();
    BasicBlock *entry = function->entry;

    clear_visited_flag(entry);

    HashSetAdd(workList, entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);

        block->visited = true;
        HashSetRemove(workList, block);

        bool effective = true;
        while(effective){
            effective = AlgorithmIdentical(block);
        }

        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList, block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
}