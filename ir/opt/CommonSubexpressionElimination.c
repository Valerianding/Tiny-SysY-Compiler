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


bool isSimpleOperator(InstNode *instNode){
    for (int i = 0; i < sizeof(simpleOpcodes) / sizeof(Opcode); i++){
        if (instNode->inst->Opcode == simpleOpcodes[i]){
            return true;
        }
    }
    return false;
}
/*
 *
 *need improve time
 *we use hash for an expression
 *we construct a hash key
 *one hash map is : unsigned hash key  -> expression
 *another hash map is : num -> value
 */

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

bool commonSubexpression1(BasicBlock *block, Function *currentFunction){
    bool effective = false;
    InstNode *currNode = block->head_node;
    HashMap *commonSubExpression = HashMapInit();
    //反正block的结尾
    while(currNode != block->tail_node){
        printf("currNode is %d\n",currNode->inst->i);
        if(isSimpleOperator(currNode)){
            printf("simpleOperator %d\n",currNode->inst->i);
            Value *lhs = ins_get_lhs(currNode->inst);
            Value *rhs = ins_get_rhs(currNode->inst);
            Value *dest = ins_get_dest(currNode->inst);
            if((isImm(lhs) || isLocalVar(lhs) || isLocalArray(lhs) || isGlobalArray(lhs) || isGlobalVar(lhs) ||
                    isAddress(lhs)) && (isImm(rhs) || isLocalVar(rhs))){
                //看看现在的HashMap里面包不包含
                HashMapFirst(commonSubExpression);
                bool flag = false;
                Value *replace = NULL;
                printf("HashMapSize is %d\n",HashMapSize(commonSubExpression));
                for(Pair *subExpr = HashMapNext(commonSubExpression); subExpr != NULL; subExpr = HashMapNext(commonSubExpression)){
                    Subexpression *subexpression = subExpr->value;
                    if(subexpression->op != currNode->inst->Opcode) continue;
                    // TODO 没有考虑IMM！！
                    switch (currNode->inst->Opcode) {
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
                            printf("Gep\n");
                            if(subexpression->lhs == lhs && currNode->inst->Opcode == subexpression->op){
                                printf("same array!\n");
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
                    valueReplaceAll(dest,replace,currentFunction);


                    InstNode *next = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = next;
                    printf("next currNode is %d\n",currNode->inst->i);
                }else{

                    Subexpression *newSubExpression = createSubExpression(lhs,rhs,currNode->inst->Opcode);
                    HashMapPut( commonSubExpression,dest,newSubExpression);
                    currNode = get_next_inst(currNode);
                    printf("next currNode is %d\n",currNode->inst->i);
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

uint32_t hash_expr(int Opcode, Value* lhs, Value* rhs) {
    uintptr_t p1;
    if(isImm(lhs)){
        if(isImmInt(lhs)){
            p1 = (uintptr_t)lhs->pdata->var_pdata.iVal;
        }else{
            p1 = (uintptr_t)lhs->pdata->var_pdata.fVal;
        }
    }else{
        p1 = (uintptr_t)lhs;
    }

    uintptr_t p2;
    if(isImm(rhs)){
        if(isImmInt(rhs)){
            p2 = (uintptr_t)rhs->pdata->var_pdata.iVal;
        }else{
            p2 = (uintptr_t)rhs->pdata->var_pdata.fVal;
        }
    }else{
        p2 = (uintptr_t)rhs;
    }

    uint32_t h1 = (uint32_t)p1;
    uint32_t h2 = (uint32_t)p2;
    uint32_t r = Opcode;

    h1 *= 15485863u;
    h2 *= 949417133u;
    r *= 87701971u;
    h1 ^= h2 ^ r;

    h1 ^= h1 >> 16;
    h1 *= 73244475u;
    h1 ^= h1 >> 15;
    h1 ^= h1 >> 16;

    return h1;
}


/*
 *
 *
 * construct a hash_num
 * see if it has been seen before
 * if so, find the dest
 * else add t
 */
bool commonSubexpression(BasicBlock *block, Function *currentFunction){
    HashMap *num2var = HashMapInit();

    bool effective = false;
    InstNode *currNode = block->head_node;
    InstNode *tailNode = block->tail_node;
    //反正block的结尾
    while(currNode != tailNode){
        printf("currNode is %d\n",currNode->inst->i);
        if(isSimpleOperator(currNode)) {
            printf("simpleOperator %d\n", currNode->inst->i);
            Value *lhs = ins_get_lhs(currNode->inst);
            Value *rhs = ins_get_rhs(currNode->inst);
            Value *dest = ins_get_dest(currNode->inst);
//            if ((isImm(lhs) || isLocalVar(lhs) || isLocalArray(lhs) || isGlobalArray(lhs) || isGlobalVar(lhs) ||
//                 isAddress(lhs)) && (isImm(rhs) || isLocalVar(rhs))) {

                //construct a hash key
                unsigned long int hash_value = hash_expr(currNode->inst->Opcode,lhs,rhs);
                //printf("construct a hash_valus : %d ",hash_value);


                //see if we have seen before
                Value *key = HashMapGet(num2var,(void *)hash_value);
                if(key != NULL){
                    //printf("has seen before!\n");
                    //has seen before
                    valueReplaceAll(dest,key,currentFunction);

                    //delete this instruction
                    InstNode *nextNode = get_next_inst(currNode);
                    deleteIns(currNode);
                    currNode = nextNode;
                }else{
                    //printf("first see!\n");
                    //not seen before
                    HashMapPut(num2var,(void *)hash_value,dest);
                    currNode = get_next_inst(currNode);
                }
//            }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
}
