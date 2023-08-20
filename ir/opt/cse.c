//
// Created by Valerian on 2023/4/16.
//

#include "cse.h"
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
        //assert(block != NULL);
        block->visited = true;
        HashSetRemove(workList,block);
        effective |= commonSubexpression(block,currentFunction);
        if(block->true_block && block->true_block->visited == false)
            HashSetAdd(workList,block->true_block);
        if(block->false_block && block->false_block->visited == false)
            HashSetAdd(workList,block->false_block);
    }
    HashSetDeinit(workList);
    renameVariables(currentFunction);
    return effective;
}



//TODO 利用num
unsigned int hash_expr(int Opcode, Value* lhs, Value* rhs) {
     unsigned  p1;
    if(isImm(lhs)){
        if(isImmInt(lhs)){
            p1 = (unsigned long)lhs->pdata->var_pdata.iVal;
        }else{
            p1 = (unsigned long)lhs->pdata->var_pdata.fVal;
        }
    }else{
        p1 = (unsigned long)lhs;
    }

    unsigned p2;
    if(isImm(rhs)){
        if(isImmInt(rhs)){
            p2 = (unsigned long)rhs->pdata->var_pdata.iVal;
        }else{
            p2 = (unsigned long)rhs->pdata->var_pdata.fVal;
        }
    }else{
        p2 = (unsigned long)rhs;
    }

    //assert(sizeof(unsigned ) == sizeof(int));

    unsigned *memory = (unsigned *)malloc(sizeof(unsigned) * 3);
    memset(memory,0,sizeof(unsigned) * 3);
    memory[0] = Opcode;
    memory[1] = p1;
    memory[2] = p2;
    return HashMurMur32(memory, sizeof(unsigned ) * 3);
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
                //construct a hash key
                unsigned long int hash_value = hash_expr(currNode->inst->Opcode,lhs,rhs);
                //printf("construct a hash_valus : %d ",hash_value);


                //see if we have seen before
                Value *key = HashMapGet(num2var,(void *)hash_value);

                //make sure we do not mess up with types
                if(key != NULL && dest->VTy->ID == key->VTy->ID){
                    //printf("has seen before!\n");
                    //has seen before

                    effective = true;
                    valueReplaceAll(dest,key,currentFunction);

                    //delete this instruction
                    printf("Here!\n");
                    currNode = get_next_inst(currNode);

                }else{
                    //printf("first see!\n");
                    //not seen before
                    HashMapPut(num2var,(void *)hash_value,dest);
                    currNode = get_next_inst(currNode);
                }
        }else{
            currNode = get_next_inst(currNode);
        }
    }
    HashMapDeinit(num2var);
    return effective;
}