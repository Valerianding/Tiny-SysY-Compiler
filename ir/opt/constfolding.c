//
// Created by Valerian on 2023/4/7.
//

#include "constfolding.h"
bool ConstFolding(Function *currentFunction){
    // runs on function
    // 我们仅仅去检查那些
    bool effective = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    bool changed = true;
    while(changed){
        changed = false;
        InstNode *currNode = entry->head_node;
        while(currNode != tail->tail_node){
            // 不包含
            if(isSimpleOperator(currNode)){
                // 应该得是两个
                Value *lhs = ins_get_lhs(currNode->inst);
                Value *rhs = ins_get_rhs(currNode->inst);
                Value *dest = ins_get_dest(currNode->inst);
                // 左右都是立即数

                if(isImm(lhs) && isImm(rhs)){
                    changed = true;
                    if(isLocalVarInt(dest)){
                        Value *replace = (Value *)malloc(sizeof(Value));
                        int result = 0;
                        float left = getOperandValue(lhs);
                        float right = getOperandValue(rhs);
                        switch(currNode->inst->Opcode){
                            case Add:
                                result = (int)(left + right);
                                break;
                            case Sub:
                                result = (int)(left - right);
                                break;
                            case Mul:
                                result = (int)(left * right);
                                break;
                            case Div:
                                result = (int)(left / right);
                                break;
                            case Mod:
                                result = (int)left % (int)right;
                                break;
                            default:
                                assert(false);
                        }
                        value_init_int(replace,result);
                        valueReplaceAll(dest,replace,currentFunction);
                    }else if(isLocalVarFloat(dest)){
                        changed = true;
                        float left = getOperandValue(lhs);
                        float right = getOperandValue(rhs);
                        float result = 0;
                        Value *replace = (Value *)malloc(sizeof(Value));

                        switch(currNode->inst->Opcode){
                            case Add:
                                result = left + right;
                                break;
                            case Sub:
                                result = left - right;
                                break;
                            case Mul:
                                result = left * right;
                                break;
                            case Div:
                                result = left / right;
                                break;
                            default:
                                assert(false);
                        }
                        value_init_float(replace,result);
                        valueReplaceAll(dest,replace,currentFunction);
                    }
                    // 还要记得删除这里的语句
                    //我们直接改了dest所以就不用value replace

                    //TODO May Have bug here！！
                    effective = true;
                    InstNode *nextNode = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = nextNode;
                }else{
                    currNode = get_next_inst(currNode);
                }
            }else{
                currNode = get_next_inst(currNode);
            }
        }
    }
    return effective;
}

//simplify branch
bool BranchOptimizing(Function *currentFunction) {
    bool changed = false;
    BasicBlock *entry = currentFunction->entry;

    //
    HashSet *workList = HashSetInit();
    clear_visited_flag(entry);

    HashSetAdd(workList,entry);

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);

        block->visited = true;

        InstNode *blockHead = block->head_node;
        InstNode *blockTail = block->tail_node;


        if(blockTail->inst->Opcode == br_i1){
            InstNode *cmpNode = get_prev_inst(blockTail);

            assert(isCompareOperator(cmpNode));
            Value *cmpLhs = ins_get_lhs(blockHead->inst);
            Value *cmpRhs = ins_get_rhs(blockHead->inst);

            bool condition;
            if(isImmInt(cmpLhs) && isImmInt(cmpRhs)){
                switch (blockHead->inst->Opcode) {
                    case LESS:{
                        if(cmpLhs->pdata->var_pdata.iVal < cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    case LESSEQ:{
                        if(cmpLhs->pdata->var_pdata.iVal < cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    case GREAT:{
                        if(cmpLhs->pdata->var_pdata.iVal > cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    case GREATEQ:{
                        if(cmpLhs->pdata->var_pdata.iVal > cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    case EQ:{
                        if(cmpLhs->pdata->var_pdata.iVal == cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    case NOTEQ:{
                        if(cmpLhs->pdata->var_pdata.iVal != cmpRhs->pdata->var_pdata.iVal){
                            condition = true;
                        }else{
                            condition = false;
                        }
                        break;
                    }
                    default:{
                        assert(false);
                    }
                }
            }


            if(condition){
                //condition is true
                BasicBlock *falseTarget = block->false_block;


                //TODO now we just simply say that the unreachable target can't have any phis

                InstNode *falseHead = falseTarget->head_node;
                InstNode *falseTail = falseTarget->tail_node;

                bool containPhi = false;
                while(falseHead != falseTail){
                    if(falseHead->inst->Opcode == Phi){
                        containPhi = true;
                    }
                    falseHead = get_next_inst(falseHead);
                }

                if(containPhi == false){
                    HashSetRemove(block->false_block->preBlocks,block);

                    Instruction *newJump = ins_new_zero_operator(br);
                    InstNode *jumpNode = new_inst_node(newJump);

                    ins_insert_before(jumpNode,blockTail);

                    //delete this branch
                    deleteIns(blockHead);

                }else{
                    blockHead = get_next_inst(blockHead);
                }


            }else{
                BasicBlock *trueBlock = block->true_block;

                InstNode *trueHead = trueBlock->head_node;
                InstNode *trueTail = trueBlock->tail_node;

                bool containPhi = false;
                while(trueHead != trueTail){
                    if(trueHead->inst->Opcode == Phi){
                        containPhi = true;
                    }
                    trueHead = get_next_inst(trueHead);
                }

                if(containPhi == false){
                    HashSetRemove(block->true_block->preBlocks,block);
                }
            }
        }else{
            blockHead = get_next_inst(blockHead);
        }

        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
    return changed;
}