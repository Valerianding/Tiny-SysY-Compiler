//
// Created by Valerian on 2023/4/16.
//

#include "cse.h"
const Opcode simpleOpcodes[] = {Add, Sub, Mul, Div, Mod,GEP};
//TODO 解决全局的公共子表达式 解决其对于Phi函数可能的破坏
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

    for(int i = 0; i < 16; i++){
        h1 *= 15485863u;
        h2 *= 949417133u;
        r *= 87701971u;
        h1 ^= h2 ^ r;
    }


    h1 ^= h1 >> 16;
    h1 *= 73244475u;
    h1 ^= h1 >> 15;
    h1 ^= h1 >> 16;

    return h1;
}


/*
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
        //printf("currNode is %d\n",currNode->inst->i);
        if(isSimpleOperator(currNode)) {
            //printf("simpleOperator %d\n", currNode->inst->i);
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

                    effective = true;
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
    HashMapDeinit(num2var);
    return effective;
}