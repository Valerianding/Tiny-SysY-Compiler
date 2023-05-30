//
// Created by Valerian on 2023/4/16.
//

#include "CommonSubexpressionElimination.h"
const Opcode simpleOpcodes[] = {Add, Sub, Mul, Div, Mod,GEP};
//TODO 解决全局的公共子表达式 解决其对于Phi函数可能的破坏
Subexpression *createSubExpression(Value *lhs, Value *rhs, Opcode op){
    Subexpression *subexpression = (Subexpression*)malloc(sizeof(Subexpression));
    memset(subexpression, 0, sizeof(Subexpression));
    subexpression->lhs = lhs;
    subexpression->rhs = rhs;
    subexpression->op = op;
    return subexpression;
}

bool commonSubexpressionElimination(Function *currentFunction){
    bool effective = false;
    //runs for each BasicBlock
    BasicBlock *entry = currentFunction->entry;
    clear_visited_flag(entry);

    HashSet *workList = HashSetInit();
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        assert(block != NULL);
        block->visited = true;
        HashSetRemove(workList,block);
        effective |= commonSubexpression(block,currentFunction);
        if(block->true_block && block->true_block->visited == false)
            HashSetAdd(workList,block->true_block);
        if(block->false_block && block->false_block->visited == false)
            HashSetAdd(workList,block->false_block);
    }
    renameVariables(currentFunction);
    return effective;
}

bool isSame(Value *left, Value *right){
    if(isImmInt(left) && isImmInt(right) && (left->pdata->var_pdata.iVal == right->pdata->var_pdata.iVal)){
        return true;
    }else if(isImmFloat(left) && isImmFloat(right) && (left->pdata->var_pdata.fVal == right->pdata->var_pdata.fVal)){
        return true;
    }else if(left == right){
        return true;
    }
    return false;
}

bool commonSubexpression(BasicBlock *block, Function *currentFunction){
    bool effective = false;
    InstNode *currNode = block->head_node;
    HashMap *commonSubExpression = HashMapInit();
    //反正block的结尾
    while(currNode != block->tail_node){
        if(isSimpleOperator(currNode)){
            printf("before here!\n");
            Value *lhs = ins_get_lhs(currNode->inst);
            Value *rhs = ins_get_rhs(currNode->inst);
            Value *dest = ins_get_dest(currNode->inst);
            if((isImm(lhs) || isLocalVar(lhs) || isLocalArray(lhs) || isGlobalArray(lhs) || isGlobalVar(lhs) ||
                    isAddress(lhs)) && (isImm(rhs) || isLocalVar(rhs))){
                //看看现在的HashMap里面包不包含
                printf("here!\n");
                HashMapFirst(commonSubExpression);
                bool flag = false;
                Value *replace = NULL;
                for(Pair *subExpr = HashMapNext(commonSubExpression); subExpr != NULL; subExpr = HashMapNext(commonSubExpression)){
                    Subexpression *subexpression = subExpr->value;
                    // TODO 没有考虑IMM！！
                    switch (subexpression->op) {
                        case Add: {
                            printf("Add\n");
                            if(((isSame(subexpression->lhs,lhs) && isSame(subexpression->rhs,rhs)) || (isSame(subexpression->lhs,rhs) && isSame(subexpression->rhs,lhs))) && subexpression->op == currNode->inst->Opcode){
                                //并且还需要类型相等才能替换
                                replace = subExpr->key;
                                if(replace->VTy->ID != dest->VTy->ID){
                                    replace = NULL;
                                }else{
                                    flag = true;
                                }
                            }
                            break;
                        }
                        case Sub:{
                            printf("Sub\n");
                            if((isSame(subexpression->lhs,lhs) && isSame(subexpression->rhs,rhs)) && subexpression->op == currNode->inst->Opcode){
                                replace = subExpr->key;
                                if(replace->VTy->ID != dest->VTy->ID){
                                    replace = NULL;
                                }else{
                                    flag = true;
                                }
                            }
                            break;
                        }
                        case Mul:{
                            printf("Mul\n");
                            if(((isSame(subexpression->lhs,lhs) && isSame(subexpression->rhs,rhs)) || (isSame(subexpression->lhs,rhs) && isSame(subexpression->rhs,lhs))) && subexpression->op == currNode->inst->Opcode){
                                //满足条件
                                replace = subExpr->key;
                                if(replace->VTy->ID != dest->VTy->ID){
                                    replace = NULL;
                                }else{
                                    flag = true;
                                }
                            }
                            break;
                        }
                        case Div:{
                            printf("Div\n");
                            if(isSame(subexpression->lhs,lhs) && isSame(subexpression->rhs,rhs) && subexpression->op == currNode->inst->Opcode){
                                replace = subExpr->key;
                                if(replace->VTy->ID != dest->VTy->ID){
                                    replace = NULL;
                                }else{
                                    flag = true;
                                }
                            }
                            break;
                        }
                        case Mod:{
                            printf("Mod\n");
                            if(isSame(subexpression->lhs,lhs) && isSame(subexpression->rhs,rhs) && subexpression->op == currNode->inst->Opcode){
                                replace = subExpr->key;
                                if(replace->VTy->ID != dest->VTy->ID){
                                    replace = NULL;
                                }else{
                                    flag = true;
                                }
                            }
                            break;
                        }
                        case GEP:{
                            printf("case GEP!\n");
                            if(subexpression->lhs == lhs && currNode->inst->Opcode == subexpression->op){
                                printf("some array!\n");
                                if(isImmInt(subexpression->rhs) && isImmInt(rhs) && (subexpression->rhs->pdata->var_pdata.iVal == rhs->pdata->var_pdata.iVal)){
                                    replace = subExpr->key;
                                    flag = true;
                                }else if(subexpression->rhs == rhs){
                                    replace = subExpr->key;
                                    flag = true;
                                }
                            }
                            break;
                        }
                        default:{
                            assert(false);
                        }
                    }
                }

                //
                if(flag){
                    printf("%s replaced by: %s\n",dest->name,replace->name);
                    effective = true;
                    assert(replace != NULL);
                    value_replaceAll(dest,replace);
                    //TODO phi 里面的还是没有更新的
                    InstNode *funcHead = currentFunction->entry->head_node;
                    InstNode *funcTail = currentFunction->tail->tail_node;
                    while(funcHead != funcTail){
                        if(funcHead->inst->Opcode == Phi){
                            HashSet *phiSet = funcHead->inst->user.value.pdata->pairSet;
                            HashSetFirst(phiSet);
                            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                                if(phiInfo->define == dest){
                                    phiInfo->define = replace;
                                }
                            }
                        }
                        funcHead = get_next_inst(funcHead);
                    }


                    InstNode *next = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = next;
                }else{
                    Subexpression *newSubExpression = createSubExpression(lhs,rhs,currNode->inst->Opcode);
                    HashMapPut( commonSubExpression,dest,newSubExpression);
                    currNode = get_next_inst(currNode);
                }
            }else{
                currNode = get_next_inst(currNode);
            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
    HashMapDeinit(commonSubExpression);
    return effective;
}

bool isSimpleOperator(InstNode *instNode){
    for (int i = 0; i < sizeof(simpleOpcodes) / sizeof(Opcode); i++){
        if (instNode->inst->Opcode == simpleOpcodes[i]){
            return true;
        }
    }
    return false;
}