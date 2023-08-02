//
// Created by Valerian on 2023/8/1.
//

#include "loop2memcpy.h"
bool CheckLoopMemcpy(Loop *loop){
    if(HashSetSize(loop->loopBody) != 2) return false;

    InstNode *entryHead = loop->head->head_node;
    InstNode *entryTail = loop->head->tail_node;

    int insCount = 0;
    while(entryHead != entryTail){
        insCount++;
        entryHead = get_next_inst(entryHead);
    }

    if(insCount != 3) return false;

    if(!loop->initValue) return false;

    if(!loop->inductionVariable) return false;

    if(loop->initValue->VTy->ID != Int || loop->initValue->pdata->var_pdata.iVal != 0) return false;

    if(!loop->modifier) return false;


    Instruction *modifierIns = (Instruction *)loop->modifier;

    if(modifierIns->Opcode != Add) return false;

    Value *modifyLhs = ins_get_lhs(modifierIns);
    Value *modifyRhs = ins_get_rhs(modifierIns);

    if(modifyLhs != loop->inductionVariable && modifyRhs != loop->inductionVariable) return false;

    //
    Value *other = (modifyLhs == loop->inductionVariable) ? modifyRhs : modifyLhs;

    if(other->VTy->ID != Int || other->pdata->var_pdata.iVal != 1) return false;

    //condition must be a slt
    Instruction *endIns = (Instruction *)loop->end_cond;
    Value *endLhs = ins_get_lhs(endIns);
    if(endLhs != loop->inductionVariable) return false;


    if(endIns->Opcode != LESS) return false;

    return true;
}

bool loop2memcpy(Loop *loop){
    bool effective = false;
    HashSetFirst(loop->child);
    for(Loop *child = HashSetNext(loop->child); child != NULL; child = HashSetNext(loop->child)) {
        effective |= loop2memcpy(child);
    }

    if(!CheckLoopMemcpy(loop)) return false;

    Instruction *endIns = (Instruction *)loop->end_cond;
    Value *count = ins_get_rhs(endIns);

    assert(loop->body_block != NULL);
    BasicBlock *body_block = loop->body_block;

    //(access access load store add jump)
    int bodyInsCount = 0;
    InstNode *bodyHead = body_block->head_node;
    InstNode *bodyTail = body_block->tail_node;
    while(bodyHead != bodyTail){
        bodyInsCount++;
        bodyHead = get_next_inst(bodyHead);
    }
    InstNode *currNode = get_next_inst(body_block->head_node);
    Value *srcArray = NULL;
    Value *destArray = NULL;

    Value *anticipateStore = NULL;
    Value *anticipateLoad = NULL;
    int index = 0;
    while(currNode != bodyTail){
        switch (index) {
            case 0:{
                if(currNode->inst->Opcode != GEP){
                    assert(false);
                    return false;
                }
                Value *gepIndex = ins_get_rhs(currNode->inst);
                if(gepIndex != loop->inductionVariable){
                    return false;
                }

                destArray = ins_get_lhs(currNode->inst);
                anticipateStore = ins_get_dest(currNode->inst);
                break;
            }
            case 1:{

                if(currNode->inst->Opcode != GEP){
                    return false;
                }

                Value *gepIndex = ins_get_rhs(currNode->inst);
                if(gepIndex != loop->inductionVariable){
                    return false;
                }

                srcArray = ins_get_lhs(currNode->inst);
                anticipateLoad = ins_get_dest(currNode->inst);
                break;
            }
            case 2:{
                if(currNode->inst->Opcode != Load){
                    return false;
                }

                Value *loadPlace = ins_get_lhs(currNode->inst);
                if(loadPlace != anticipateLoad){
                    return false;
                }
                break;
            }

            case 3:{
                if(currNode->inst->Opcode != Store){
                    return false;
                }

                Value *storePlace = ins_get_rhs(currNode->inst);
                if(storePlace != anticipateStore){
                    return false;
                }
                break;
            }

            case 4:{
                Instruction *modifierIns = (Instruction *)loop->modifier;
                Value *modify = ins_get_dest(modifierIns);
                Value *curDest = ins_get_dest(currNode->inst);
                if(curDest != modify){
                    return false;
                }
                break;
            }
            default:{
                return false;
            }
        }
        index++;
        currNode = get_next_inst(currNode);
    }

    printf("src is %s dest is %s length %s\n",srcArray->name,destArray->name,count->name);

    //
    assert(loop->containMultiBackEdge == false);

    BasicBlock *loopEntry = loop->head;

    //remove tail from entry's predecessors
    HashSetRemove(loop->head->preBlocks,loop->tail);
    assert(HashSetSize(loopEntry->preBlocks) == 1);

    //remove loop body
    cleanBlock(loop->body_block);

    //replace induction variable
    valueReplaceAll(loop->inductionVariable,count,loopEntry->Parent);

    //remove all instructions in entry except for label node
    InstNode *removeNode = get_next_inst(loopEntry->head_node);
    InstNode *entryNext = get_next_inst(loopEntry->tail_node);
    while(removeNode != entryNext){
        InstNode *tempNode = get_next_inst(removeNode);
        deleteIns(removeNode);
        removeNode = tempNode;
    }

    Value *type_size = (Value *)malloc(sizeof(Value));
    value_init_int(type_size,4);

    InstNode *entryLabel = loopEntry->head_node;

    Instruction *lengthIns = ins_new_binary_operator(Mul,count,type_size);
    InstNode *lengthNode = new_inst_node(lengthIns);
    //设置dest的Type，理论上来说应该只能是VAR_INT
    lengthNode->inst->user.value.VTy->ID = Var_INT;
    ins_insert_after(lengthNode,entryLabel);
    lengthNode->inst->Parent = loopEntry;
    lengthNode->inst->user.value.name = (char *)malloc(sizeof(char) * 10);
    strcpy(lengthNode->inst->user.value.name,"%length");

    //(dest src length)
    Instruction *param1 = ins_new_unary_operator(GIVE_PARAM,destArray);
    InstNode *paramNode1 = new_inst_node(param1);
    ins_insert_after(paramNode1,lengthNode);
    paramNode1->inst->Parent = loopEntry;

    Instruction *param2 = ins_new_unary_operator(GIVE_PARAM,srcArray);
    InstNode *paramNode2 = new_inst_node(param2);
    ins_insert_after(paramNode2,paramNode1);
    paramNode2->inst->Parent = loopEntry;


    Value *length = ins_get_dest(lengthIns);
    Instruction *param3 = ins_new_unary_operator(GIVE_PARAM,length);
    InstNode *paramNode3 = new_inst_node(param3);
    ins_insert_after(paramNode3,paramNode2);
    paramNode3->inst->Parent = loopEntry;

    Instruction *memcpy = ins_new_zero_operator(SysYMemcpy);
    InstNode *memcpyNode = new_inst_node(memcpy);
    ins_insert_after(memcpyNode,paramNode3);
    memcpyNode->inst->Parent = loopEntry;

    //insert a jump at the end
    Instruction *jumpIns = ins_new_zero_operator(br);
    InstNode *jumpNode = new_inst_node(jumpIns);
    ins_insert_after(jumpNode,memcpyNode);
    jumpNode->inst->Parent = loopEntry;

    assert(loop->exit_block != NULL);

    loopEntry->true_block = loop->exit_block;
    loopEntry->false_block = NULL;
    loopEntry->head_node = entryLabel;
    loopEntry->tail_node = jumpNode;


    //remove this loop from parent
    if(loop->parent != NULL){
        Loop *parent = loop->parent;
        HashSetRemove(parent->child,loop);
    }
    return true;
}

bool Loop2Memcpy(Function *currentFunction){
    bool effective = false;
    HashSetFirst(currentFunction->loops);
    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
        effective |= loop2memcpy(root);
    }
}