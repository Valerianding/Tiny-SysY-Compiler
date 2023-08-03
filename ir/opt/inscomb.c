////
//// Created by Valerian on 2023/5/14.
////
//
//#include "inscomb.h"
//
//This pass will do inst combine
#include "inscomb.h"
InstNode *cur_;
InstNode *result_;
HashSet *workList;

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
        //-> because we don't know the calculation operation's input is from what
        return false;
    }
    Value *dest = ins_get_dest(instNode->inst);
    if(dest->use_list == NULL){
        return true;
    }
    return false;
}

//add all users of current instruction to workList
void AddUsesToWorklist(InstNode *inst){

}

InstNode *ReplaceInstUsesWith(InstNode *inst, Value *newValue){
    AddUsesToWorklist(inst);

}

//only consider binary operations
bool CheckInt(InstNode *instNode){
    assert(instNode != NULL);
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);
    Value *dest = ins_get_dest(instNode->inst);
    if(!isImmInt(lhs) || !isImmInt(rhs) || !isImmInt(dest)) return false;
    return true;
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
//so we'd better check out type for the combine instruction
bool checkType(Instruction *ins){
    assert(ins != NULL);
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

                //TODO is it to set parent according to cur_ or instNode
                newBinary->Parent = cur_->inst->Parent;
                InstNode *newBinaryNode = new_inst_node(newBinary);
                ins_insert_before(newBinaryNode,cur_);

                //TODO add to workList
                HashSetAdd(workList,newBinaryNode);

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

//TODO why this need use_list to be 1
Value *DynCastMul(Value *lhs,int paramNum){
    int countUse = 0;
    Use *uses = lhs->use_list;
    while(uses != NULL){
        countUse++;
        uses = uses->Next;
    }
    printf("%s\n",lhs->name);
    if(countUse == 1 && !isImm(lhs) && !isParam(lhs,paramNum)){
        Instruction *lhsIns = (Instruction *)lhs;
        Value *lhsInsRhs = ins_get_rhs(lhsIns);
        Value *lhsInsLhs = ins_get_lhs(lhsIns);
        if(lhsIns->Opcode == Mul && isImmInt(lhsInsRhs)){
            return lhsInsLhs;
        }
    }
    return NULL;
}

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

    //x + 0 -> x
    //TODO not finished
    if(isImm(rhs) && rhs->pdata->var_pdata.iVal == 0){
        ReplaceInstUsesWith(instNode,lhs);
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

    //x - x -> 0
    if(isSame(lhs,rhs)){

    }
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
    workList = HashSetInit();
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

    printf("size of workList %d\n", HashSetSize(workList));
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

        printf("run on %d\n",node->inst->i);
        //try to combine current instruction
        cur_ = node;
        result_ = NULL;
        RunPass(node);

        //if effective
        if(result_){
            changed = true;
            //if it is not the same means this instruction can be replaced
            if(result_ != node){
                //insert new instruction into block
                BasicBlock *block = node->inst->Parent;
                result_->inst->Parent = block;
                ins_insert_before(result_,node);

                //replace
                //don't delete it just replace it!! the dead inst will automatically be removed

                Value *old = ins_get_dest(node->inst);
                Value *new = ins_get_dest(result_->inst);
                valueReplaceAll(old,new,currentFunction);
            }else{

            }
        }
    }
    return changed;
}

//
// Created by Valerian on 2023/5/14.
//



//-----------------------------lsy

/*
 * 目前做了一个基本块内的,int类型的inscomb
 */

//判断类型是不是加减,TODO 乘除未做
//检查type与是否有常数
//检查后面是否被用到过
//location为0指前面那条，location为1指后面那条
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

//获取常数value的位置,value1或value2
int get_const_location(Instruction* instruction)
{
    if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float)
        return 1;
    return 2;
}

Value *get_value(Instruction *instruction)
{
    //如果两边都是int,取左边作为const，右边作value
    if((ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float) && (ins_get_rhs(instruction)->VTy->ID==Int || ins_get_rhs(instruction)->VTy->ID==Float))
        return ins_get_rhs(instruction);

    if(ins_get_lhs(instruction)->VTy->ID!=Int && ins_get_lhs(instruction)->VTy->ID!=Float)
        return ins_get_lhs(instruction);
    return ins_get_rhs(instruction);
}

Value *get_const(Instruction *instruction)
{
    if(ins_get_lhs(instruction)->VTy->ID==Int || ins_get_lhs(instruction)->VTy->ID==Float)
        return ins_get_lhs(instruction);
    return ins_get_rhs(instruction);
}

//合并
void combination(Instruction *instruction_A,Instruction * instruction_B)
{
    //合并得到常值
    Value *v_A_const= get_const(instruction_A);
    Value *v_A_value= get_value(instruction_A);
    Value *v_B_const= get_const(instruction_B);

    Value *v_num=(Value*) malloc(sizeof (Value));
    //以第二条的opcode为准
    if((instruction_A->Opcode==Add && instruction_B->Opcode==Add) || (instruction_A->Opcode==Sub && instruction_B->Opcode==Sub))  //+ +
        value_init_int(v_num,v_A_const->pdata->var_pdata.iVal+v_B_const->pdata->var_pdata.iVal);
    else if(instruction_A->Opcode==Add && instruction_B->Opcode==Sub)
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);
    else if(instruction_A->Opcode==Sub && instruction_B->Opcode==Add && get_const_location(instruction_A)==1)     //要改opcode,并且生成后value一定在右边
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal+v_A_const->pdata->var_pdata.iVal);
    else
        value_init_int(v_num,v_B_const->pdata->var_pdata.iVal-v_A_const->pdata->var_pdata.iVal);

    //B的左值作为最终左值,不动
    //替换常数值和另一个value
    if(instruction_A->Opcode==Sub && instruction_B->Opcode==Add && get_const_location(instruction_A)==1)
    {
        instruction_B->Opcode=Sub;
        replace_lhs_operand(instruction_B,v_num);
        replace_rhs_operand(instruction_B,v_A_value);
    }
    if(get_const_location(instruction_B)==1)
    {
        replace_lhs_operand(instruction_B,v_num);
        replace_rhs_operand(instruction_B,v_A_value);
    }
    else
    {
        replace_rhs_operand(instruction_B,v_num);
        replace_lhs_operand(instruction_B,v_A_value);
    }
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