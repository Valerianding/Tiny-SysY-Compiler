//
// Created by Valerian on 2023/5/14.
//
//

//This pass will do inst combine
#include "inscomb.h"
InstNode *cur_;
InstNode *result_;
Vector *workList;

const Opcode possibleOpcodes[] = {Add,Sub,Mul,Div,Mod};
bool isPossibleOperator(Instruction *inst){
    int n = sizeof(possibleOpcodes) / sizeof(Opcode);
    for(int i = 0; i < n; i++){
        if(inst->Opcode == possibleOpcodes[i]){
            return true;
        }
    }
    return false;
}

void RemoveFromWorklist(Vector *workList,InstNode *instNode){
    unsigned i = 0;
    while(i < VectorSize(workList)){
        InstNode *sameNode = NULL;
        VectorGet(workList,i,(void *)&sameNode);
        if(sameNode == instNode){
            VectorRemove(workList,i);
        }else{
            i = i + 1;
        }
    }
}

bool isDeadInst(InstNode *instNode){
    //if the use_list is null we simply return true
    if(isCriticalOperator(instNode)){
        //why you put a critical operation here since we only do inst comb for calculations
        //-> because we don't know the calculation operation's input is from what
        return false;
    }
    //also we must taking phi's uses into consideration
    bool Dead = true;
    Value *dest = ins_get_dest(instNode->inst);
    if(dest->use_list != NULL){
        Dead = false;
    }
    BasicBlock *block = instNode->inst->Parent;
    Function *func = block->Parent;
    if(usedInPhi(dest,func)){
        Dead = false;
    }
    return Dead;
}

//add all users of current instruction to workList
//also we only want to consider the possible operations
void AddUsesToWorklist(InstNode *inst){
    //printf("%d\n",inst->inst->i);
    assert(checkType(inst->inst));
    Value *dest = ins_get_dest(inst->inst);
    Use *uses = dest->use_list;
    while(uses != NULL){
        Value *user= uses->Val;
        Instruction *userIns = (Instruction *)user;
        if(isPossibleOperator(userIns) && user->use_list != NULL){
            InstNode *userNode = findNode(userIns->Parent,userIns);
            VectorPushBack(workList,userNode);
        }
        uses = uses->Next;
    }
}

InstNode *ReplaceInstUsesWith(InstNode *inst, Value *newValue){
    AddUsesToWorklist(inst);
    Value *dest = ins_get_dest(inst->inst);
    BasicBlock *block = inst->inst->Parent;
    Function *func = block->Parent;
    valueReplaceAll(dest,newValue,func);
    return inst;
}



//check if the dest of the operation is only used once
bool isOnlyUse(Instruction *ins){
    Value *dest = ins_get_dest(ins);
    int count = 0;
    Use *destUses = dest->use_list;
    while(destUses != NULL){
        count++;
        destUses = destUses->Next;
    }
    if(count == 1) return true;
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
//So we'd better check out type for the combine instruction
//also we don't want to mess up with non-binary operations
//So let's make it a possible operation only
bool checkType(Instruction *ins){
    assert(ins != NULL);
    if(!isPossibleOperator(ins)) return false;
    Value *insLhs = ins_get_lhs(ins);
    Value *insRhs = ins_get_rhs(ins);
    Value *insDest = ins_get_dest(ins);
    if(!isInt(insLhs) || !isInt(insRhs) || !isInt(insDest)){
        return false;
    }
    return true;
}

Value *Fold(Opcode op, Value *value1, Value *value2){
    assert(value1 != NULL && isImmInt(value1));
    assert(value2 != NULL && isImmInt(value2));

    Value *newConstant = (Value *)malloc(sizeof(Value));
    int val;
    int v1 = value1->pdata->var_pdata.iVal;
    int v2 = value2->pdata->var_pdata.iVal;
    //only int
    switch (op) {
        case Add:{
            val = v1 + v2;
            break;
        }
        case Sub:{
            val = v1 - v2;
            break;
        }
        case Mul:{
            val = v1 * v2;
            break;
        }
        case Div:{
            val = v1 / v2;
            break;
        }
        case Mod:{
            val = v1 % v2;
            break;
        }
        default:{
            assert(false);
        }
    }
    value_init_int(newConstant,val);
    return newConstant;
}

bool SimplifyCommutative(InstNode *instNode){
    bool changed = false;
    Instruction *ins = instNode->inst;
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

    //check the type due to crazy design of IR
    //we only want to deal with integer so that we don't care about roundings
    if(lhsIns == NULL || !checkType(lhsIns)) return changed;


    Value *lhsInsRhs = ins_get_rhs(lhsIns);
    Value *lhsInsLhs = ins_get_lhs(lhsIns);

    //same operation
    //add || mul
    if(lhsIns->Opcode == op && isImm(lhsInsRhs)) {
        //%1 = .. +/* C0
        //.. = %1 +/* C1

        //=> .. = .. +/* (C0 +/* C1)
        if (isImm(rhs)) {
            Value *newConstant = Fold(op, lhsInsRhs, rhs);

            //set current lhs & set currentRhs
            replace_lhs_operand(ins, lhsInsLhs);
            replace_rhs_operand(ins, newConstant);
            return true;
        }else if (rhsIns != NULL) {
            //
            if(!checkType(rhsIns)) return changed;

            Value *rhsInsLhs = ins_get_lhs(rhsIns);
            Value *rhsInsRhs = ins_get_rhs(rhsIns);

            if(rhsIns->Opcode == op && isImmInt(rhsInsRhs) && isOnlyUse(lhsIns) && isOnlyUse(rhsIns)){
                // fold ((V1 op C1) op (V2 op C2))
                //  ==> ((V1 op V2) op (C1 op C2))

                Value *newConstant = Fold(op,lhsInsRhs,rhsInsRhs);
                Instruction *newBinary = ins_new_binary_operator(op,lhsInsLhs,rhsInsLhs);
                newBinary->user.value.VTy->ID = Var_INT;
                newBinary->user.value.name = (char *)malloc(sizeof(char) * 10);
                strcpy(newBinary->user.value.name,"%new");
                //TODO is it to set parent according to cur_ or instNode
                newBinary->Parent = cur_->inst->Parent;
                InstNode *newBinaryNode = new_inst_node(newBinary);
                ins_insert_before(newBinaryNode,cur_);

                VectorPushBack(workList,newBinaryNode);

                Value *newDest = ins_get_dest(newBinary);

                //set curr
                replace_lhs_operand(ins,newDest);
                replace_rhs_operand(ins,newConstant);
                return true;
            }
        }
    }
    return changed;
}

// If this value is a multiply that can be folded into other computations
// (because it has a constant operand), return the non-constant operand.
// The constant is for sure a int!!
//TODO why this need use_list to be 1
Value *DynCastMul(Value *val,int paramNum){
    //due to immediate value has no name
    if(isImmInt(val)) return NULL;
    int countUse = 0;
    Use *uses = val->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    //we need value's name to judge if it is param
    //it is ok that if the val's name is %temp -> because it is definitely not the param
    assert(val->name != NULL);
    if(countUse == 1 && !isImm(val) && !isParam(val,paramNum)){
        Instruction *valIns = (Instruction *)val;
        if(!checkType(valIns)) return NULL;
        Value *valInsRhs = ins_get_rhs(valIns);
        Value *valInsLhs = ins_get_lhs(valIns);
        if(valIns->Opcode == Mul && isImmInt(valInsRhs)){
            return valInsLhs;
        }
    }
    return NULL;
}

//TODO Also why this need use_list to be 1
Value *DynCastAdd(Value *val,int paramNum){
    if(isImmInt(val)) return NULL;
    int countUse = 0;
    Use *uses = val->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    assert(val->name != NULL);
    if(countUse == 1 && !isImm(val) && !isParam(val,paramNum)){
        Instruction *valIns = (Instruction *)val;
        if(!checkType(valIns)) return NULL;
        Value *valInsRhs = ins_get_rhs(valIns);
        Value *valInsLhs = ins_get_lhs(valIns);
        if(valIns->Opcode == Add && isImmInt(valInsRhs)){
            return valInsLhs;
        }
    }
    return NULL;
}

//helper function to see if the mul is useless
//(x * C) / C -> X
//return the constant C
//TODO why we need use_list to be 1
Value *UselessCastMul(Value *val, int paramNum){
    if(isImmInt(val)) return NULL;
    int countUse = 0;
    Use *uses = val->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    assert(val->name != NULL);
    if(countUse == 1 && !isImmInt(val) && !isParam(val,paramNum)){
        Instruction *mul = (Instruction *)val;
        if(!checkType(mul)) return NULL;
        Value *constant = ins_get_rhs(mul);
        if(mul->Opcode == Mul && isImmInt(constant)){
            return constant;
        }
    }
    return NULL;
}

// (x + C) - C -> X
Value *UselessCastAdd(Value *val, int paramNum){
    if(isImmInt(val)) return NULL;
    int countUse = 0;
    Use *uses = val->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    assert(val->name != NULL);
    if(countUse == 1 && !isImmInt(val) && !isParam(val,paramNum)){
        Instruction *add = (Instruction *)val;
        if(!checkType(add)) return NULL;
        Value *constant = ins_get_rhs(add);
        if(add->Opcode == Add && isImmInt(constant)){
            return constant;
        }
    }
    return NULL;
}


// (X - C) + C - > X
//TODO
Value *UselessCastSub(Value *val, int paramNum){
    if(isImmInt(val)) return NULL;
    int countUse = 0;
    Use *uses = val->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    assert(val->name != NULL);
    if(countUse == 1 && !isImmInt(val) && !isParam(val,paramNum)){
        Instruction *sub = (Instruction *)val;
        if(!checkType(sub)) return NULL;
        Value *constant = ins_get_rhs(sub);
        if(sub->Opcode == Sub && isImmInt(constant)){
            return constant;
        }
    }
    return NULL;
}

// X / C * C - > X is wrong due to the rounding
InstNode *RunOnAdd(InstNode *instNode){
    bool changed = SimplifyCommutative(instNode);

    BasicBlock *block = instNode->inst->Parent;
    int paramNum = block->Parent->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);

    //check all int
    if(!checkType(instNode->inst)){
        return changed ? cur_ : nullptr;
    }

    //TODO not finished
    if(isImm(rhs)){
        //x + 0 -> x
        if(rhs->pdata->var_pdata.iVal == 0){
            return ReplaceInstUsesWith(instNode,lhs);
        }

        //(X - C) + C -> X
        Value *sc = UselessCastSub(lhs,paramNum);
        if(isSame(sc,rhs)){
            assert(isImmInt(rhs) && sc->pdata->var_pdata.iVal == rhs->pdata->var_pdata.iVal);
            Instruction *lhsIns = (Instruction *)lhs;
            Value *lhsInsLhs = ins_get_lhs(lhsIns);
            //
            return ReplaceInstUsesWith(cur_,lhsInsLhs);
        }
    }

    //x + x -> x * 2
    if(isSame(lhs,rhs)){
        Value *constTwo = (Value *)malloc(sizeof(Value));
        value_init_int(constTwo,2);
        Instruction *multi = ins_new_binary_operator(Mul,lhs,constTwo);
        multi->user.value.name = (char *)malloc(sizeof(char) * 10);
        strcpy(multi->user.value.name,"%multi");
        multi->user.value.VTy->ID = Var_INT;
        InstNode *multiNode = new_inst_node(multi);
        return multiNode;
    }

    //x * C + X -> X * (C + 1)
    if(DynCastMul(lhs,paramNum) == rhs){
        Instruction *lhsIns = (Instruction *)lhs;
        Value *c = ins_get_rhs(lhsIns);
        assert(isImmInt(c));

        Value *cp1 = (Value *)malloc(sizeof(Value));
        value_init_int(cp1,c->pdata->var_pdata.iVal + 1);
        Instruction *multi = ins_new_binary_operator(Mul,rhs,cp1);
        multi->user.value.name = (char *)malloc(sizeof(char) * 10);
        strcpy(multi->user.value.name,"%multi");
        multi->user.value.VTy->ID = Var_INT;
        InstNode *multiNode = new_inst_node(multi);
        return multiNode;
    }

    //X + X * C -> X * (C + 1)
    if(DynCastMul(rhs,paramNum) == lhs){
        Instruction *rhsIns = (Instruction *)rhs;
        Value *c = ins_get_rhs(rhsIns);
        assert(isImmInt(c));

        Value *cp1 = (Value *)malloc(sizeof(Value));
        value_init_int(cp1,c->pdata->var_pdata.iVal + 1);
        Instruction *multi = ins_new_binary_operator(Mul,lhs,cp1);
        multi->user.value.name = (char *)malloc(sizeof(char) * 10);
        strcpy(multi->user.value.name,"%multi");
        multi->user.value.VTy->ID = Var_INT;
        InstNode *multiNode = new_inst_node(multi);
        return multiNode;
    }
    return changed ? cur_ : NULL;
}

InstNode *RunOnSub(InstNode *instNode){
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);

    BasicBlock *block = instNode->inst->Parent;
    int paramNum = block->Parent->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    if(!checkType(instNode->inst)){
        return NULL;
    }

    //x - x -> 0
    //TODO not finished
    if(isSame(lhs,rhs)){
        Value *constZero = (Value *)malloc(sizeof(Value));
        value_init_int(constZero,0);
        return ReplaceInstUsesWith(cur_,constZero);
    }

    //X - O -> X
    if(isImmInt(rhs) && rhs->pdata->var_pdata.iVal == 0){
        return ReplaceInstUsesWith(cur_,lhs);
    }

    // X - X * C -> X * (1 - C)
    if(DynCastMul(rhs,paramNum) == lhs){
        //
        Instruction *rhsIns = (Instruction *)rhs;
        Value *constant = ins_get_rhs(rhsIns);
        Value *ncp1 = (Value *)malloc(sizeof(Value));
        value_init_int(ncp1,1 - constant->pdata->var_pdata.iVal);
        Instruction *newMul = ins_new_binary_operator(Mul,lhs,ncp1);
        newMul->user.value.name = (char *)malloc(sizeof(char) * 10);
        strcpy(newMul->user.value.name,"%multi");
        newMul->user.value.VTy->ID = Var_INT;
        InstNode *multiNode = new_inst_node(newMul);
        return multiNode;
    }

    // X * C - X -> X * (C - 1)
    if(DynCastMul(lhs,paramNum) == rhs){
        //
        Instruction *lhsIns = (Instruction *)lhs;
        Value *constant = ins_get_rhs(lhsIns);
        Value *cm1 = (Value *)malloc(sizeof(Value));
        value_init_int(cm1,constant->pdata->var_pdata.iVal - 1);
        Instruction *newMul = ins_new_binary_operator(Mul,rhs,cm1);
        newMul->user.value.name = (char *)malloc(sizeof(char) * 10);
        strcpy(newMul->user.value.name,"%multi");
        newMul->user.value.VTy->ID = Var_INT;
        InstNode *multiNode = new_inst_node(newMul);
        return multiNode;
    }


    // (X + Y) - Y -> X
    //TODO now we only consider the constant situation
    // X + C - C -> X
    Value *add = UselessCastAdd(lhs,paramNum);
    if(isSame(add,rhs)){
        assert(add->pdata->var_pdata.iVal == rhs->pdata->var_pdata.iVal);
        Instruction *lhsIns = (Instruction *)lhs;
        Value *lhsInsLhs = ins_get_lhs(lhsIns);
        return ReplaceInstUsesWith(cur_,lhsInsLhs);
    }

    //TODO Y - (X + Y) -> -X
    return NULL;
}

InstNode *RunOnMul(InstNode *instNode){
    bool changed = SimplifyCommutative(instNode);

//    BasicBlock *block = instNode->inst->Parent;
//    int paramNum = block->Parent->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);

    if(!checkType(instNode->inst)){
        return changed ? cur_ : NULL;
    }

    if(isImmInt(rhs)){
        //x * 0 -> 0
        if(rhs->pdata->var_pdata.iVal == 0){
            Value *zero = (Value *)malloc(sizeof(Value));
            value_init_int(zero,0);
            return ReplaceInstUsesWith(instNode,zero);
        }

        //x * 1 -> x
        if(rhs->pdata->var_pdata.iVal == 1){
            return ReplaceInstUsesWith(instNode,lhs);
        }
    }
    return changed ? cur_ : NULL;
}

InstNode *RunOnDiv(InstNode *instNode){
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);

    BasicBlock *block = instNode->inst->Parent;
    int paramNum = block->Parent->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;

    if(!checkType(instNode->inst)){
        return NULL;
    }

    if(isImmInt(rhs)){
        //x / 1 -> x
        if(rhs->pdata->var_pdata.iVal == 1){
            return ReplaceInstUsesWith(cur_,lhs);
        }

        //(x * C) / C -> X
        Value *mc = UselessCastMul(lhs,paramNum);
        if(isSame(mc,rhs)){
            assert(mc->pdata->var_pdata.iVal == rhs->pdata->var_pdata.iVal);
            Instruction *lhsIns = (Instruction *)lhs;
            Value *lhsInsLhs = ins_get_lhs(lhsIns);
            return ReplaceInstUsesWith(cur_,lhsInsLhs);
        }
    }

    if(isImmInt(lhs)){
        //0 / x -> 0
        if(lhs->pdata->var_pdata.iVal == 0){
            return ReplaceInstUsesWith(cur_,lhs);
        }
    }
    return NULL;
}

//TODO finish this
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
            //TODO the critical instruction should not be considered!
            result_ = NULL;
            return;
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


    HashSet *WorkList = HashSetInit();
    HashSetAdd(WorkList,entry);
    while(HashSetSize(WorkList) != 0){
        HashSetFirst(WorkList);
        BasicBlock *block = HashSetNext(WorkList);
        block->visited = true;
        HashSetRemove(WorkList,block);
        HashSetAdd(ALL,block);
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(WorkList,block->true_block);
        }
        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(WorkList, block->false_block);
        }
    }

    HashSetDeinit(WorkList);


    workList = VectorInit(1000);
    HashSetFirst(ALL);
    for(BasicBlock *block = HashSetNext(ALL); block != NULL; block = HashSetNext(ALL)){
        InstNode *blockHead = block->head_node;
        InstNode *blockTail = block->tail_node;
        while(blockHead != blockTail){
            if(isPossibleOperator(blockHead->inst) && checkType(blockHead->inst)){
                VectorPushBack(workList,blockHead);
            }
            blockHead = get_next_inst(blockHead);
        }
    }

    HashSetDeinit(ALL);

    //run on workList
    while(VectorSize(workList) != 0){
        unsigned last = VectorSize(workList) - 1;
        InstNode *node = NULL;
        VectorGet(workList,last,(void *)&node);
        VectorRemove(workList,last);

        //printf("now on %d\n",node->inst->i);

        Value *lhs = ins_get_lhs(node->inst);
        Value *rhs = ins_get_rhs(node->inst);

        //if is dead inst
        if(isDeadInst(node)){
            changed = true;
            //add operands to the workList
            //This could add critical operations to the workList
            //and we don't want this to happen, So we simply add possible operations to workList
            if(!isImm(lhs) && !isParam(lhs,paramNum)){
                Instruction *lhsIns = (Instruction *)lhs;
                InstNode *lhsNode = findNode(lhsIns->Parent,lhsIns);
                if(isPossibleOperator(lhsIns)){
                    VectorPushBack(workList,(void *)lhsNode);
                }
            }

            if(!isImm(rhs) && !isParam(rhs,paramNum)){
                Instruction *rhsIns = (Instruction *)rhs;
                InstNode *rhsNode = findNode(rhsIns->Parent,rhsIns);
                if(isPossibleOperator(rhsIns)){
                    VectorPushBack(workList,(void *)rhsNode);
                }
            }
            RemoveFromWorklist(workList,node);

            //remove this instruction from parent block
            deleteIns(node);
            continue;
        }
//
        //if we can const propagate it
        if(lhs != NULL && rhs != NULL && isImmInt(lhs) && isImmInt(rhs)){
            changed = true;
            //const propagate it
            Value *constant =  Fold(node->inst->Opcode,lhs,rhs);
            ReplaceInstUsesWith(node,constant);
            RemoveFromWorklist(workList,node);
            //erase this instruction
            deleteIns(node);
            continue;
        }


        //printf("run on %d\n",node->inst->i);
        //try to combine current instruction
        cur_ = node;
        result_ = NULL;
        RunPass(node);
//
//        //if effective
        if(result_){
            changed = true;
            //if it is not the same means this instruction can be replaced
            if(result_ != node){
                //insert new instruction into block
                BasicBlock *block = node->inst->Parent;
                result_->inst->Parent = block;
                ins_insert_before(result_,node);

                RemoveFromWorklist(workList,node);
                //replace
                //don't delete it just replace it!! the dead inst will automatically be removed

                Value *old = ins_get_dest(node->inst);
                Value *new = ins_get_dest(result_->inst);
                valueReplaceAll(old,new,currentFunction);
            }else{
                //if it is the same node means the result is modified
                if(isDeadInst(node)){
                    RemoveFromWorklist(workList,node);
                    deleteIns(node);
                    result_ = NULL;
                }
            }
            if(result_){
                VectorPushBack(workList, result_);
                AddUsesToWorklist(result_);
            }
            changed = true;
        }
    }
    VectorDeinit(workList);
    return changed;
}

//
// Created by Valerian on 2023/5/14.
//



//-----------------------------lsy
bool check(Instruction *instruction,int location)
{
    if(((location==0 && instruction->user.value.use_list!=NULL) || (location==1)) && (instruction->Opcode==Add || instruction->Opcode==Sub))
    {
        if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float || ins_get_rhs(instruction)->VTy->ID==Int ||
           ins_get_rhs(instruction)->VTy->ID==Float)
            return true;
        return false;
    }
    return false;
}

Value *get_const(Instruction *instruction, int *location)
{
    if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float){
        *location = 1;
        return ins_get_lhs(instruction);
    }
    *location = 2;
    return ins_get_rhs(instruction);
}

void combination(Instruction* instruction_A,Instruction* instruction_B){
    int location1 , location2;
    //合并得到常值
    Value *v_A_const= get_const(instruction_A,&location1);
    Value *v_A_value = NULL;
    if(ins_get_lhs(instruction_A) == v_A_const)
        v_A_value = ins_get_rhs(instruction_A);
    else
        v_A_value = ins_get_lhs(instruction_A);
    Value *v_B_const= get_const(instruction_B,&location2);

    Value *v_num=(Value*) malloc(sizeof (Value));
    //以第二条的opcode为准
    //1. 双add不用分类
    if(instruction_A->Opcode==Add && instruction_B->Opcode==Add){
        value_init_int(v_num,v_A_const->pdata->var_pdata.iVal+v_B_const->pdata->var_pdata.iVal);
        if(location2 == 2){
            //第二个add的第二个是常数
            replace_lhs_operand(instruction_B,v_A_value);
            replace_rhs_operand(instruction_B,v_num);
        } else {
            replace_rhs_operand(instruction_B,v_A_value);
            replace_lhs_operand(instruction_B,v_num);
        }
    }
    //2.
    else if (instruction_A->Opcode==Add && instruction_B->Opcode==Sub){
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);
        if(location2 == 2){
            replace_lhs_operand(instruction_B,v_A_value);
            replace_rhs_operand(instruction_B,v_num);
        } else {
            replace_rhs_operand(instruction_B,v_A_value);
            replace_lhs_operand(instruction_B,v_num);
        }
    }
    //3.
    else if(instruction_A->Opcode==Sub && instruction_B->Opcode==Add){
        if(location1 == 2){
            value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);
            if(location2 == 2){
                replace_lhs_operand(instruction_B,v_A_value);
                replace_rhs_operand(instruction_B,v_num);
            } else {
                replace_rhs_operand(instruction_B,v_A_value);
                replace_lhs_operand(instruction_B,v_num);
            }
        } else {
            value_init_int(v_num,v_B_const->pdata->var_pdata.iVal+v_A_const->pdata->var_pdata.iVal);
            instruction_B->Opcode = Sub;
            replace_lhs_operand(instruction_B,v_num);
            replace_rhs_operand(instruction_B,v_A_value);
        }
    }
    //4.
    else {
        if(location1 == 2 && location2 == 2){
            value_init_int(v_num,v_A_const->pdata->var_pdata.iVal+v_B_const->pdata->var_pdata.iVal);
            replace_lhs_operand(instruction_B,v_A_value);
            replace_rhs_operand(instruction_B,v_num);
        } else if(location1 == 2){
            value_init_int(v_num,v_A_const->pdata->var_pdata.iVal+v_B_const->pdata->var_pdata.iVal);
            replace_lhs_operand(instruction_B,v_num);
            replace_rhs_operand(instruction_B,v_A_value);
        } else if(location1 == 1 && location2 == 2){
            value_init_int(v_num,v_A_const->pdata->var_pdata.iVal-v_B_const->pdata->var_pdata.iVal);
            replace_lhs_operand(instruction_B,v_num);
            replace_rhs_operand(instruction_B,v_A_value);
        } else{
            instruction_B->Opcode = Add;
            replace_lhs_operand(instruction_B,v_A_value);
            replace_rhs_operand(instruction_B,v_num);
        }
    }
}

bool have_phi_use(Function* currentFunction, Value* oldValue){
    InstNode *currNode = currentFunction->entry->head_node;
    InstNode *tailNode = currentFunction->tail->tail_node;
    while(currNode != tailNode){
        if(currNode->inst->Opcode == Phi){
            HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                if(phiInfo->define == oldValue){
                    return true;
                }
            }
        }
        currNode = get_next_inst(currNode);
    }
    return false;
}

void instruction_combination(Function *currentFunction)
{
    //label_cancomb(currentFunction);
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    InstNode *currNode = get_next_inst(entry->head_node);

    while (currNode != get_next_inst(end->tail_node)) {
        int flag=1;
        if(check(currNode->inst,0))
        {
            Instruction *instruction=currNode->inst;
            Use* use=instruction->user.value.use_list;
            while(use!=NULL)
            {
                Value *left_user=&use->Parent->value;
                Instruction *instruction2=(Instruction*)left_user;
                //判断两条ir在同一基本块
                //如果有不在同一基本块的ir，暂时默认动不了这条ir
                if(instruction2->Parent->id==instruction->Parent->id && check(instruction2,1))
                {
                    use=use->Next;
                    //给当前这条node打条标记吧
                    if(flag)
                        currNode->inst->user.value.pdata->var_pdata.iVal=-10;
                    combination(currNode->inst,instruction2);
                }
                else        //TODO 提取加一个能否combi的判定,(好像不用，可以部分combi，部分不combi，也可以
                {
                    flag=0;
                    currNode->inst->user.value.pdata->var_pdata.iVal=0;
                    use=use->Next;
                }
            }
            if(have_phi_use(currentFunction, ins_get_dest(instruction))){
                flag=0;
                currNode->inst->user.value.pdata->var_pdata.iVal=0;
            }
        }
        currNode= get_next_inst(currNode);
    }

    currNode = get_next_inst(entry->head_node);

    while (currNode != get_next_inst(end->tail_node)) {
        if(currNode->inst->user.value.pdata->var_pdata.iVal==-10)
        {
            InstNode *now=currNode;
            currNode= get_prev_inst(currNode);
            deleteIns(now);
        }
        currNode= get_next_inst(currNode);
    }
}