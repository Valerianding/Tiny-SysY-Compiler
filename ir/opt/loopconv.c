//
// Created by Valerian on 2023/7/18.
//

#include "loopconv.h"

//this pass will try to convert store constant to a array in a loop into memset
//try to run it after LICM

bool CheckLoop(Loop *loop){
    if(HashSetSize(loop->loopBody) != 2) return false;

    InstNode *entryHead = loop->head->head_node;
    InstNode *entryTail = loop->head->tail_node;

    int insCount = 0;
    while(entryHead != entryTail){
        insCount++;
        entryHead = get_next_inst(entryHead);
    }

    if(insCount != 3) return false;

    if(!loop->inductionVariable) return false;

    if(!loop->initValue) return false;

    //init value must be zero
    if(loop->initValue->VTy->ID != Int || loop->initValue->pdata->var_pdata.iVal != 0){
        return false;
    }

    //modifier must be an add ind, %1
    if(!loop->modifier) return false;

    Instruction *modifierIns = (Instruction *)loop->modifier;

    if(modifierIns->Opcode != Add) return false;

    Value *modifyLhs = ins_get_lhs(modifierIns);
    Value *modifyRhs = ins_get_rhs(modifierIns);

    if(modifyLhs != loop->inductionVariable && modifyRhs != loop->inductionVariable) return false;

    //
    Value *other = (modifyLhs == loop->inductionVariable) ? modifyRhs : modifyLhs;

    if(other->VTy->ID != Int || other->pdata->var_pdata.iVal != 1) return false;


    if(!loop->end_cond) return false;

    //condition must be a slt
    Instruction *endIns = (Instruction *)loop->end_cond;
    Value *endLhs = ins_get_lhs(endIns);
    if(endLhs != loop->inductionVariable) return false;

    // TODO maybe less eq ?
    if(endIns->Opcode != LESS) return false;

    printf("finally true !!\n");
    return true;
}

bool LoopConv(Loop *loop){
    if(!CheckLoop(loop)) return false;

    //
    Instruction *endIns = (Instruction *)
    Value *count = loop->end_cond
}