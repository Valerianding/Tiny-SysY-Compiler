//
// Created by Valerian on 2023/8/5.
//

#include "loopconv.h"

//dirty conversion for loop

//check if the instruction satisfy our need

// on the condition that the loop's trip count is know to be constant
//phi =  phi [initValue, modified value]
//modified value  = phi op loop-invariant variable
//so the phi is for sure the lhs of current node
bool CheckNode(InstNode *instNode, Loop *loop){
    switch (instNode->inst->Opcode) {
        case Add: case Sub: break;
        default: return false;
    }
    BasicBlock *block =  instNode->inst->Parent;
    Function *func = block->Parent;

    int paramNum = func->entry->head_node->inst->user.use_list->Val->pdata->symtab_func_pdata.param_num;
    Value *lhs = ins_get_lhs(instNode->inst);
    Value *rhs = ins_get_rhs(instNode->inst);
    Value *dest = ins_get_dest(instNode->inst);

    //don't mess up with Types
    if(!isInt(dest) || !isInt(rhs) || !isInt(dest)) return false;

    if(isImm(lhs) || isParam(lhs,paramNum)){
        return false;
    }
    Instruction *lhsIns = (Instruction *)lhs;
    if(lhsIns->Opcode != Phi) return false;

    //the phi can't have other uses within the loop!
    bool UsedOnceInLoop = true;
    Value *phiValue = ins_get_dest(lhsIns);

    //这个phi的use_list 除了phi都有 而phi的只需要判断是不是在entry里面就行了 -> 卡死了一个body_block
    InstNode *entryHead = loop->head->head_node;
    InstNode *entryTail = loop->head->tail_node;
    while(entryHead != entryTail){
        if(entryHead->inst->Opcode == Phi){
            HashSet *phiSet = entryHead->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                Value *define = phiInfo->define;
                if(define == phiValue){
                    return false;
                }
            }
        }
        entryHead = get_next_inst(entryHead);
    }

    Use *phiUses = phiValue->use_list;
    while(phiUses != NULL){
        User *user = phiUses->Parent;
        Instruction *userIns = (Instruction *)user;
        BasicBlock *userBlock = userIns->Parent;
        if(userBlock == loop->body_block){
            return false;
        }
        phiUses = phiUses->Next;
    }

    //need initValue to be constant
    //also one is form the Loop, and the other is not from the loop
    HashSet *phiSet = instNode->inst->user.value.pdata->pairSet;
    HashSetFirst(phiSet);
    Value *other = NULL;
    assert(HashSetSize(phiSet) == 2);
    for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
        BasicBlock *from = phiInfo->from;
        Value *define = phiInfo->define;
        if(!HashSetFind(loop->loopBody,from)){
            if(define == NULL || !isImmInt(define)){
                return false;
            }
        }else{
            other = define;
        }
    }

    if(other == NULL || other != dest) return false;

    //need the dest only used in that phi!
    if(dest->use_list != NULL) return false;


    //rhs need to be loop-free variable!!
    if(isImmInt(rhs) || isParam(rhs,paramNum)) return true;

    Instruction *rhsIns = (Instruction *)rhs;

    if(HashSetFind(loop->loopBody,rhsIns->Parent)) return false;

    //同时计算不来自循环也需要不是load、call出来的变量
    //TODO is it safe now?
    if(rhsIns->Opcode == Call || rhsIns->Opcode == Load) return false;

    return true;
}

//检查是不是静态循环
//默认是add inductionVar , 1
bool CheckConvLoop(Loop *loop){
    if(HashSetSize(loop->loopBody) != 2) return false;

    BasicBlock *loopEntry = loop->head;
    if(HashSetSize(loopEntry->preBlocks) != 2) return false;

    if(!loop->inductionVariable) return false;

    if(!loop->initValue) return false;

    if(!loop->modifier) return false;

    Instruction *modifierIns = (Instruction *)loop->modifier;

    if(modifierIns->Opcode != Add) return false;

    Value *modifyLhs = ins_get_lhs(modifierIns);
    Value *modifyRhs = ins_get_rhs(modifierIns);

    if(modifyLhs != loop->inductionVariable && modifyRhs != loop->inductionVariable) return false;

    Value *other = (modifyLhs == loop->inductionVariable) ? modifyRhs : modifyLhs;

    if(other->VTy->ID != Int || other->pdata->var_pdata.iVal != 1) return false;

    if(!loop->end_cond) return false;

    Instruction *endIns = (Instruction *)loop->end_cond;
    Value *cmpLhs = ins_get_lhs(endIns);
    Value *cmpRhs = ins_get_rhs(endIns);

    if(cmpLhs != loop->inductionVariable) return false;
    if(!isImmInt(cmpRhs)) return false;

    if(endIns->Opcode != LESS) return false;
    return true;
}




bool loopSimplify(Loop *loop){
    bool changed = false;
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)){
        changed |= loopSimplify(child);
    }

    if(!CheckConvLoop(loop)){
        return changed;
    }

    BasicBlock *bodyBlock = loop->body_block;
    InstNode *bodyHead = bodyBlock->head_node;
    InstNode *bodyTail = bodyBlock->tail_node;
    while(bodyHead != bodyTail){
        if(CheckNode(bodyHead,loop)){
            printf("node %d can be replaced!\n");
            //find the init Value and the trip count of the loop

            //create a mult instruction in the preHeader

            //insert an instruction with op of original computation

            //replace the phi after and
        }
        bodyHead = get_next_inst(bodyHead);
    }
}

// on the condition that the loop's trip count is know to be constant
//phi =  phi [initValue, modified value]
//modified value  = phi op loop-invariant variable
//
bool LoopSimplify(Function *currentFunction){
    HashSet *loops = currentFunction->loops;
    HashSetFirst(loops);
//    for(Loop *root = HashSetNext(loops);)
    return true;
}