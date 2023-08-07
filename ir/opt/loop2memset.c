//
// Created by Valerian on 2023/7/18.
//

#include "loop2memset.h"
//this pass will try to convert store zero to a array in a loopAnalysis into memset
//need to run it after LICM
InstNode *newMemset(Value *ptr, Value *stored, Value *length){
    Instruction *memsetIns = ins_new(3);

    Use *pUse = user_get_operand_use(&memsetIns->user,0);
    use_set_value(pUse,ptr);
    pUse = user_get_operand_use(&memsetIns->user,1);
    use_set_value(pUse,stored);
    pUse = user_get_operand_use(&memsetIns->user,2);
    use_set_value(pUse,length);

    memsetIns->Opcode = SysYMemset;
    memsetIns->i = instruction_uid;
    instruction_uid++;

    //
    InstNode *memsetNode = new_inst_node(memsetIns);
    return memsetNode;
}

bool LoopConvCheckLoop(Loop *loop){
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

    //init value must be zero TODO can be other but we should add gep instruction before memset
    if(loop->initValue->VTy->ID != Int || loop->initValue->pdata->var_pdata.iVal != 0){
        return false;
    }

    //modifier must be an add ind, 1
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

    printf("now is loopAnalysis %d!\n",loop->head->id);

    if(!LoopConvCheckLoop(loop)){
        printf("one loopAnalysis is not satisfy for remove!\n");
        return false;
    }

    //must be rhs of end_cond
    Instruction *endIns = (Instruction *)loop->end_cond;
    Value *count = ins_get_rhs(endIns);

    assert(loop->body_block);
    BasicBlock *body_block = loop->body_block;


    //body block only contain 4 instructions // This means we don't consider two dimension right now

    //(access store modifier jump)
    int bodycount = 0;
    InstNode *bodyHead = body_block->head_node;
    InstNode *bodyTail = body_block->tail_node;
    while(bodyHead != bodyTail){
        bodycount++;
        bodyHead = get_next_inst(bodyHead);
    }

    if(bodycount != 4) return false;

    InstNode *gepInstNode = NULL;
    //skip label node
    bodyHead = loop->body_block->head_node;
    bodyHead = get_next_inst(bodyHead);
    int index = 0;


    printf("bodyHead id %d\n",bodyHead->inst->i);
    Value *basePtr = NULL; // gep的lhs
    Value *gepValue = NULL; // gep的dest
    Value *stored = NULL;
    while(bodyHead != bodyTail){
        switch (index) {
            case 0:{
                //must be an access instruction=
                if(bodyHead->inst->Opcode != GEP){
                    return false;
                }

                //index must be induction variable
                Value *gepIndex = ins_get_rhs(bodyHead->inst);
                if(gepIndex != loop->inductionVariable) {
                    return false;
                }


                //base ptr
                //size is always 4
                basePtr = ins_get_lhs(bodyHead->inst);
                gepValue = ins_get_dest(bodyHead->inst);
                break;
            }
            case 1:{
                //must be an store instruction
                if(bodyHead->inst->Opcode != Store){
                    return false;
                }

                //let's see the stored value
                stored = ins_get_lhs(bodyHead->inst);
                //it can only be zero

                if(!isImm(stored) || stored->pdata->var_pdata.iVal != 0){
                    return false;
                }

                //pointer must be the last access instruction
                Value *storePtr = ins_get_rhs(bodyHead->inst);
                if(storePtr != gepValue){
                    return false;
                }
                break;
            }
            case 2:{
                //must be a modifier
                Instruction *modifierIns = (Instruction *)loop->modifier;
                Value *modify = ins_get_dest(modifierIns);
                Value *curDest = ins_get_dest(bodyHead->inst);
                if(modify != curDest){
                    return false;
                }
                break;

            }
            default:{
                return false;
            }
        }
        index++;
        bodyHead = get_next_inst(bodyHead);
    }

    if(basePtr != NULL && gepValue != NULL && stored != NULL){
        printf("stored %s to gep %s base %s!\n",stored->name,gepValue->name,basePtr->name);
    }

    //ok now we can remove loop body


    //at to this point we know that the loop has a dedicated exist
    assert(loop->containMultiBackEdge == false);

    //update the predecessors of entry block


    //yet it is certain that it has only one entry and one exit

    BasicBlock *loopEntry = loop->head;
    //remove tail
    HashSetRemove(loop->head->preBlocks,loop->tail);
    assert(HashSetSize(loop->head->preBlocks) == 1);

    //remove loop body
    cleanBlock(loop->body_block);

    //replace induction variable
    valueReplaceAll(loop->inductionVariable,count,loopEntry->Parent);

    //remove all instructions in entry except for label node
    InstNode *entryRemove = get_next_inst(loopEntry->head_node);
    InstNode *entryNext = get_next_inst(loopEntry->tail_node);
    while(entryRemove != entryNext){
        InstNode *entryTemp = get_next_inst(entryRemove);
        deleteIns(entryRemove);
        entryRemove = entryTemp;
    }

    Value *type_size = (Value *)malloc(sizeof(Value));
    value_init_int(type_size,4);


    InstNode *entryLabel= loopEntry->head_node;

    //mul count * 4
    Instruction *lengthIns = ins_new_binary_operator(Mul,count,type_size);
    InstNode *lengthNode = new_inst_node(lengthIns);
    //设置dest的Type，理论上来说应该只能是VAR_INT
    lengthNode->inst->user.value.VTy->ID = Var_INT;
    Value *length = ins_get_dest(lengthIns);
    ins_insert_after(lengthNode,entryLabel);
    lengthNode->inst->Parent = loopEntry;
    lengthNode->inst->user.value.name = (char *)malloc(sizeof(char) * 10);
    strcpy(lengthNode->inst->user.value.name,"%length");
    //create give param
    //insert function call on loopAnalysis entry
    Instruction *giveparam1 = ins_new_unary_operator(GIVE_PARAM,basePtr);
    InstNode *paramPtr = new_inst_node(giveparam1);
    ins_insert_after(paramPtr,lengthNode);
    paramPtr->inst->Parent = loopEntry;

    //
    Instruction *giveparam2 = ins_new_unary_operator(GIVE_PARAM,stored);
    InstNode *paramValue = new_inst_node(giveparam2);
    ins_insert_after(paramValue,paramPtr);
    paramValue->inst->Parent = loopEntry;

    //
    Instruction *giveparam3 = ins_new_unary_operator(GIVE_PARAM,length);
    InstNode *paramLength = new_inst_node(giveparam3);
    ins_insert_after(paramLength,paramValue);
    paramLength->inst->Parent = loopEntry;

    //create a memset instruction
    Instruction *memset = ins_new_zero_operator(SysYMemset);
    InstNode *memsetNode = new_inst_node(memset);
    ins_insert_after(memsetNode,paramLength);
    memsetNode->inst->Parent = loopEntry;

    //insert a jump instruction
    Instruction *jumpIns = ins_new_zero_operator(br);
    InstNode *jumpNode = new_inst_node(jumpIns);
    ins_insert_after(jumpNode,memsetNode);
    jumpNode->inst->Parent = loopEntry;

    //change the entry's successors
    assert(loop->exit_block != NULL);

    printf("loopAnalysis exit block is %d!\n",loop->exit_block->id);
    BasicBlock *exit_block = loop->exit_block;
    loopEntry->true_block = exit_block;
    loopEntry->false_block = NULL;
    loopEntry->head_node = entryLabel;
    loopEntry->tail_node = jumpNode;

    //我们还需要移除这个loop 以保证parent不会再访问这个loop了
    //并且parent的loop还需要重构避免bug的出现
    //TODO may have bug here
    if(loop->parent != NULL){
        Loop *parent = loop->parent;
        HashSetRemove(parent->child,loop);
    }
    return true;
}

//同样的我们希望的是DFS 遍历整个树
bool loop2memset(Loop *root){
    bool effective = false;
    HashSetFirst(root->child);
    for(Loop *child = HashSetNext(root->child); child != NULL; child = HashSetNext(root->child)){
        effective |= loop2memset(child);
    }
    //do operation now
    effective |= LoopConv(root);
    return effective;
}

bool Loop2Memset(Function *currentFunction){
    bool effective = false;
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        effective |= loop2memset(root);
    }
    renameVariables(currentFunction);
    return effective;
}