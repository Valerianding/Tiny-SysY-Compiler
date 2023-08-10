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
            if(otherIns->Opcode != Add && otherIns->Opcode != Sub){
                return;
            }

            assert(otherIns->Opcode == Add || otherIns->Opcode == Sub);

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

bool loopReduce(Loop *loop){
    //handle child first
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        loopReduce(child);
    }

    //Value * -> RecExpression
    if(!CheckLoopReduce(loop)) return false;

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
    //add / sub /
    //
    HashSetFirst(loop->loopBody);
    for(BasicBlock *loopBody = HashSetNext(loop->loopBody); loopBody != NULL; loopBody = HashSetNext(loop->loopBody)){
        InstNode *blockHead = loopBody->head_node;
        InstNode *blockTail = loopBody->tail_node;

        //
        while(blockHead != blockTail){
            if(blockHead)
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