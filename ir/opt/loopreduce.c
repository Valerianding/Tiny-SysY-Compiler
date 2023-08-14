//
// Created by Valerian on 2023/8/9.
//

#include "loopreduce.h"
//this pass will perform induction variable strength reduction

typedef struct BasicInductionVariableExpression{
    Value *initValue;
    Opcode op;
    Value *step;
}BIVExpression;

BIVExpression *newBIVExpression(Value *initValue,Opcode op, Value *step){
    BIVExpression *bivExpression = (BIVExpression *)malloc(sizeof(BIVExpression));
    bivExpression->initValue = initValue;
    assert(op == Sub || op == Add);
    bivExpression->op = op;
    bivExpression->step = step;
    return bivExpression;
}

//simple check, we strengthen check in
bool CheckLoopReduce(Loop *loop){
    if(HashSetSize(loop->loopBody) != 2) return false;

    if(!loop->body_block) return false;

    if(!loop->exit_block) return false;

    if(!loop->end_cond) return false;

    if(!loop->modifier) return false;

    return true;
}




//check phi node -> collect set of basic induction variable
void LoopReduceCheckNode(InstNode *instNode,Loop *loop){
    assert(instNode->inst->Opcode == Phi);

    HashSet *phiSet = instNode->inst->user.value.pdata->pairSet;
    assert(phiSet != NULL);

    //phi = [init value, addRec]

    assert(HashSetSize(phiSet) == 2);

    Function *currentFunction = loop->head->Parent;
    int paramNum = currentFunction->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;


    Value *phiDest = ins_get_dest(instNode->inst);

    Value *initValue = NULL;
    Value *step = NULL;
    Opcode op;

    HashSetFirst(phiSet);
    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
        BasicBlock *from = phiInfo->from;
        if(!HashSetFind(loop->loopBody,from)){
            initValue = phiInfo->define;
        }else{
            //check this value's from
            Value *other = phiInfo->define;
            if(isImm(other) || isParam(other,paramNum)) return;

            //
            Instruction *otherIns = (Instruction *)other;

            //this instruction must be ( add / sub phi, constant)

            //TODO Now we let the opcode must be add
            if(otherIns->Opcode != Add){
                return;
            }

            op = otherIns->Opcode;

            Value *otherLhs = ins_get_lhs(otherIns);

            //rhs must be the phi
            if(otherLhs != phiDest){
                return;
            }

            //TODO need to be a regional constant
            //TODO Here we simply makes it a constant int
            Value *otherRhs = ins_get_rhs(otherIns);
            if(!isImmInt(otherRhs)) return;

            //
            step = otherRhs;
        }
    }

    //
    if(initValue == NULL || step == NULL) return;

    //now we can be certain that this is a basic induction variable
    BIVExpression *bivExpression = newBIVExpression(initValue,op,step);

    printf("find BasicB %s\n",phiDest->name);
    HashMapPut(loop->inductionVariables,phiDest,bivExpression);
}

//Only consider Add、Sub、Mul
bool couldSimplify(InstNode *instNode,Loop *loop){
    switch (instNode->inst->Opcode) {
        case Add:case Sub:case Mul: break;
        default: return false;
    }

    //check Type
    Value *dest = ins_get_dest(instNode->inst);
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);
    if(!isInt(dest) || !isInt(lhs) || !isInt(rhs)) return false;

    //must be -> add/sub/mul basicInductionVariable, Regional Constant
    if(!HashMapContain(loop->inductionVariables,lhs)){
        return false;
    }

    if(!isRegionalConstant(rhs,loop)){
        return false;
    }

    return true;
}

bool loopReduce(Loop *loop){
    //handle child first
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        loopReduce(child);
    }

    if(!CheckLoopReduce(loop)) return false;

    //Value * -> RecExpression
    loop->inductionVariables = HashMapInit(); // only used in this function -> needs to be Destroy after


    //collect basic induction variables
    BasicBlock *loopEntry = loop->head;
    InstNode *loopHead = loopEntry->head_node;
    InstNode *loopTail = loopEntry->tail_node;
    while(loopHead != loopTail){
        if(loopHead->inst->Opcode == Phi){
            LoopReduceCheckNode(loopHead,loop);
        }
        loopHead = get_next_inst(loopHead);
    }

    //find those can be simplified -> iterated induction variables
    //add / sub / Mul
    //
    HashSetFirst(loop->loopBody);
    for(BasicBlock *loopBody = HashSetNext(loop->loopBody); loopBody != NULL; loopBody = HashSetNext(loop->loopBody)){
        InstNode *blockHead = loopBody->head_node;
        InstNode *blockTail = loopBody->tail_node;

        //
        while(blockHead != blockTail){
            if(couldSimplify(blockHead,loop)){
                //if this node could Simplify

                Value *lhs = ins_get_lhs(blockHead->inst);

                Value *modify = ins_get_rhs(blockHead->inst);

                assert(HashMapContain(loop->inductionVariables,lhs));

                BIVExpression *bivExpression = (BIVExpression *)HashMapGet(loop->inductionVariables,lhs);

                assert(bivExpression != NULL);

                //get the initValue & Step
                Value *initValue = bivExpression->initValue;

                Value *step = bivExpression->step;

                assert(HashSetSize(loopEntry->preBlocks) == 2);

                //create initValue in the preHeader!
                BasicBlock *preHeader = NULL;

                HashSetFirst(loopEntry->preBlocks);
                for(BasicBlock *preBlock = HashSetNext(loopEntry->preBlocks); preBlock != NULL; preBlock = HashSetNext(loopEntry->preBlocks)){
                    if(!HashSetFind(loop->loopBody,preBlock)){
                        preHeader = preBlock;
                    }
                }

                assert(preHeader != NULL);

                InstNode *preHeaderTail = preHeader->tail_node;

                //cal init value in the preHeader
                Opcode op = blockHead->inst->Opcode;
                Instruction *newPhiInitValue = ins_new_binary_operator(op,initValue,modify);
                newPhiInitValue->user.value.name = (char *)malloc(sizeof(char) * 10);
                strcpy(newPhiInitValue->user.value.name,"%newInit");
                newPhiInitValue->user.value.VTy->ID = Var_INT;
                newPhiInitValue->Parent = preHeader;
                InstNode *newPhiInitNode = new_inst_node(newPhiInitValue);
                ins_insert_before(newPhiInitNode,preHeaderTail);

                //cal modifier for new phi
                Instruction *newModify = ins_new_binary_operator(op,step,modify);
                newModify->user.value.name = (char *)malloc(sizeof(char) * 10);
                strcpy(newModify->user.value.name,"%newStep");
                newModify->user.value.VTy->ID = Var_INT;
                newModify->Parent = preHeader;
                InstNode *newModifyNode = new_inst_node(newModify);
                ins_insert_before(newModifyNode,preHeaderTail);

                //insert a new phi instruction in the loopEntry
                Instruction *newPhi = ins_new_zero_operator(Phi);
                newPhi->user.value.name = (char *)malloc(sizeof(char) * 10);
                newPhi->user.value.VTy->ID = Var_INT;
                newPhi->user.value.pdata->pairSet = HashSetInit();

                //insert initValue to phiInfo
                //insert a new instruction to modify this phi in current Place

                //replace use of current dest with this new modified value
            }
            blockHead = get_next_inst(blockHead);
        }
    }

    HashMapDeinit(loop->inductionVariables);
    return true;
}

void LoopReduce(Function *currentFunction){
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        loopReduce(root);
    }
}