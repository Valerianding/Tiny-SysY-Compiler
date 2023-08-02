//
// Created by Valerian on 2023/5/14.
//

#include "inscomb.h"

//This pass will do inst combine
InstNode *cur_;
InstNode *result_;

const Opcode possibleOpcodes[] = {Add,Sub,Mul,Div,Mod};
bool isPossibleOperator(InstNode *inst){
    int n = sizeof(possibleOpcodes) / sizeof(Opcode);
    for(int i = 0; i < n; i++){
        if(inst->inst->Opcode == possibleOpcodes[i]){
            return true;
        }
    }
    return false;
}

bool isDeadInst(InstNode *instNode){
    //if the use_list is null we simply return true
    if(isCriticalOperator(instNode)){
        //why you put a critical operation here since we only do inst comb for calculations
        assert(false);
    }
    Value *dest = ins_get_dest(instNode->inst);
    if(dest->use_list == NULL){
        return true;
    }
    return false;
}


//get a complexity or rank of Values
//0->constant 1->Argument 2->Instruction
//TODO is it OK to not consider unary or binary???
unsigned GetComplexity(Value *val, int paramNum){
    if(isImm(val)) return 0;
    if(isParam(val,paramNum)) return 1;
    return 2;
}

//only used for add / mul
void SwapOperand(InstNode *instNode){
    int numOfOperand = instNode->inst->user.value.NumUserOperands;

    assert(instNode->inst->Opcode == Mul || instNode->inst->Opcode == Add);

    assert(numOfOperand == 2);
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);
    Use *use1 = user_get_operand_use(&instNode->inst->user,0);
    Use *use2 = user_get_operand_use(&instNode->inst->user,1);
    use_set_value(use1,rhs);
    use_set_value(use2,lhs);
}

//only consider calculation operation first!
bool isAssociative(Opcode op){
    switch (op) {
        case Add:case Mul:return true;
        default: return false;
    }
}


//because we don't want to mess up with Types
//so we'd better check out type for the combine instruction
bool checkType(Instruction *ins1, Instruction *ins2){
    assert(ins1 != NULL && ins2 != NULL);
    Value *ins1Lhs = ins_get_lhs(ins1);
    Value *ins1Rhs = ins_get_rhs(ins1);
    Value *ins1Dest = ins_get_dest(ins1);

    Value *ins2Lhs = ins_get_lhs(ins2);
    Value *ins2Rhs = ins_get_rhs(ins2);
    Value *ins2Dest = ins_get_dest(ins2);


}

bool SimplifyCommutative(InstNode *instNode){
    bool changed = false;
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);

    BasicBlock *block = instNode->inst->Parent;
    Function *func = block->Parent;

    int paramNum = func->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;
    if(GetComplexity(lhs,paramNum) < GetComplexity(rhs,paramNum)){
        //swap operand -> make rhs simpler
        SwapOperand(instNode);
        changed = true;
    }

    //only for add & mul we can do more -> because of the rounding of C language
    if(!isAssociative(instNode->inst->Opcode)) return changed;

    Opcode op = instNode->inst->Opcode;

    //
    Value *curLhs = ins_get_lhs(instNode->inst);
    Value *curRhs = ins_get_rhs(instNode->inst);


    Instruction *lhsIns = NULL;
    Instruction *rhsIns = NULL;
    if(!isImm(curLhs) && !isParam(curLhs,paramNum)){
        lhsIns = (Instruction *)curLhs;
    }

    if(!isImm(curRhs) && !isParam(curRhs,paramNum)){
        rhsIns = (Instruction *)curRhs;
    }

    Value *lhsInsRhs = ins_get_rhs(lhsIns);


    //check the type due to crazy design of IR
    if(!checkType(lhsIns,instNode->inst)) return changed;


    //same operation
    if(lhsIns->Opcode == op && isImm(lhsInsRhs)){
        //%1 = .. + C0
        //.. = %1 + C1

        //=> .. = .. + (C0 + C1)
        if(isImm(rhs)){

        }
    }
}

bool IsAssociative(InstNode *instNode){
    switch (instNode->inst->Opcode) {
         case Add: case Sub: case Mul:
         case Div: case Mod: return true;
        default: return false;
    }
}

InstNode *RunOnAdd(InstNode *instNode){
    bool changed = SimplifyCommutative(instNode);
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);
    return NULL;
}

InstNode *RunOnSub(InstNode *instNode){

    return NULL;
}

InstNode *RunOnMul(InstNode *instNode){


    return NULL;
}

InstNode *RunOnDiv(InstNode *instNode){


    return NULL;
}

InstNode *RunOnMod(InstNode *instNode){

    return NULL;
}



void RunPass(InstNode *instNode){
    switch (instNode->inst->Opcode) {
        case Add:{
            result_ = RunOnAdd(instNode);
            return;
        }
        case Sub:{
            result_ = RunOnSub(instNode);
            return;
        }
        case Mul:{
            result_ = RunOnMul(instNode);
            return;
        }
        case Div:{
            result_ = RunOnDiv(instNode);
            return;
        }
        case Mod:{
            result_ = RunOnMod(instNode);
            return;
        }
        default:{
            assert(false);
        }
    }
}

bool InstCombine(Function *currentFunction){
    bool changed = false;


    BasicBlock *entry = currentFunction->entry;
    clear_visited_flag(entry);

    int paramNum = entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;
    //printf("paramNum %d\n",paramNum);

    //insert all possible instructions to workList
    HashSet *ALL = HashSetInit();
    HashSet *workList = HashSetInit();
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        block->visited = true;
        HashSetRemove(workList,block);
        HashSetAdd(ALL,block);
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }
        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList, block->false_block);
        }
    }

    //
    printf("all size %d\n", HashSetSize(ALL));
    HashSetClean(workList); // now we need to collect InstNode*

    HashSetFirst(ALL);
    for(BasicBlock *block = HashSetNext(ALL); block != NULL; block = HashSetNext(ALL)){
        InstNode *blockHead = block->head_node;
        InstNode *blockTail = block->tail_node;
        while(blockHead != blockTail){
            if(isPossibleOperator(blockHead)){
                HashSetAdd(workList,blockHead);
            }
            blockHead = get_next_inst(blockHead);
        }
    }

    //run on workList
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        InstNode *node = HashSetNext(workList);
        HashSetRemove(workList,node);

        //if is dead inst
        if(isDeadInst(node)){
            //add operands to the workList
            //int numOfOperand = node->inst->user.value.NumUserOperands;

            Value *lhs = ins_get_lhs(node->inst);
            Value *rhs = ins_get_rhs(node->inst);
            if(!isImm(lhs) && !isParam(lhs,paramNum)){
                Instruction *lhsIns = (Instruction *)lhs;
                InstNode *lhsNode = findNode(lhsIns->Parent,lhsIns);
                HashSetAdd(workList,lhsNode);
            }

            if(!isImm(rhs) && !isParam(rhs,paramNum)){
                Instruction *rhsIns = (Instruction *)rhs;
                InstNode *rhsNode = findNode(rhsIns->Parent,rhsIns);
                HashSetAdd(workList,rhsNode);
            }

            //remove this instruction from parent block
            deleteIns(node);
            continue;
        }

        //if we can const propagate it


        //try to combine current instruction
        cur_ = node;
        result_ = NULL;
        RunPass(node);
        if(result_){
            //check if instruction can be replaced
            if(result_ != node){
                //insert new instruction into block

            }
        }
    }
}