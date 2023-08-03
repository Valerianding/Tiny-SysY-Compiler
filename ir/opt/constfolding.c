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
                        dest->VTy->ID = Int;
                        float left = getOperandValue(lhs);
                        float right = getOperandValue(rhs);
                        switch(currNode->inst->Opcode){
                            case Add:
                               dest->pdata->var_pdata.iVal = (int)(left + right);
                               break;
                            case Sub:
                                dest->pdata->var_pdata.iVal = (int)(left - right);
                                break;
                            case Mul:
                                dest->pdata->var_pdata.iVal = (int)(left * right);
                                break;
                            case Div:
                                dest->pdata->var_pdata.iVal = (int)(left / right);
                                break;
                            case Mod:
                                assert(false);
                                dest->pdata->var_pdata.iVal = (int)left % (int)right;
                                break;
                            default:
                                assert(false);
                        }
                    }else if(isLocalVarFloat(dest)){
                        dest->VTy->ID = Float;
                        float left = getOperandValue(lhs);
                        float right = getOperandValue(rhs);
                        switch(currNode->inst->Opcode){
                            case Add:
                                dest->pdata->var_pdata.fVal = left + right;
                                break;
                            case Sub:
                                dest->pdata->var_pdata.fVal = left - right;
                                break;
                            case Mul:
                                dest->pdata->var_pdata.fVal = left * right;
                                break;
                            case Div:
                                dest->pdata->var_pdata.fVal = left / right;
                                break;
                            default:
                                assert(false);
                        }
                    }
                    // 还要记得删除这里的语句
                    //我们直接改了dest所以就不用value replace
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

bool BranchOptimizing(Function *currentFunction) {
    bool effective = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    bool changed = true;
    while (changed) {
        changed = false;
        InstNode *currNode = entry->head_node;
        while (currNode != tail->tail_node) {
            if (isCompareOperator(currNode)) {
                //
                Value *lhs = ins_get_lhs(currNode->inst);
                Value *rhs = ins_get_rhs(currNode->inst);
                Value *dest = ins_get_dest(currNode->inst);
                //
                if (isImm(lhs) && isImm(rhs)) {
                    changed = true;
                    float lhsValue = getOperandValue(lhs);
                    float rhsValue = getOperandValue(rhs);

                    //如果是INT我们就可以进行分支的优化了
                    dest->VTy->ID = Int;
                    switch (currNode->inst->Opcode) {
                        case EQ:
                            if (lhsValue == rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                        case NOTEQ:
                            if (lhsValue != rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                        case LESS:
                            if (lhsValue < rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                        case LESSEQ:
                            if (lhsValue <= rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                        case GREAT:
                            if (lhsValue > rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                        case GREATEQ:
                            if (lhsValue >= rhsValue) {
                                dest->pdata->var_pdata.iVal = true;
                            } else {
                                dest->pdata->var_pdata.iVal = false;
                            }
                            break;
                    }
                }
            }
        }
    }

    //判断条件现在是否被优化成了
    InstNode *branchNode = entry->head_node;
    while (branchNode != tail->tail_node) {
        if (branchNode->inst->Opcode == br_i1) {
            BasicBlock *parent = branchNode->inst->Parent;
            // 看我们的进行判断的Value1的值到底是多少呢
            Value *cond = ins_get_lhs(branchNode->inst);
            Value *insValue = ins_get_dest(branchNode->inst);
            if (isImm(cond)) {
                effective = true;
                if (cond->pdata->var_pdata.iVal) {
                    // true
                    // 需不需要修改
                    insValue->pdata->instruction_pdata.false_goto_location = 0;
                    // 记得修改DomTreeNode 上的内存这样的话 我们可以在mem2reg之前跑一次

                    BasicBlock *falseBlock = parent->false_block;

                    // 剔除
                    parent->false_block = NULL;
                    // 删除对应的基本块内的use

                    //
                } else {
                    insValue->pdata->instruction_pdata.true_goto_location = insValue->pdata->instruction_pdata.false_goto_location;
                    insValue->pdata->instruction_pdata.false_goto_location = 0;
                    // false;
                    parent->true_block = parent->false_block;
                    parent->false_block = NULL;
                }
                branchNode->inst->Opcode = br;
            }
        }
        branchNode = get_next_inst(branchNode);
    }
}