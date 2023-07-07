//
// Created by Valerian on 2023/5/28.
//

#ifndef C22V1_MEMLVN_H
#define C22V1_MEMLVN_H
#include "function.h"
#include "utility.h"


typedef struct Array{
    Value *array;
    unsigned int index; //多少层计算出来的Hash
}Array;

typedef struct StoreInfo{
    InstNode *storeInstruction; //当前store语句在哪里
    Value *storedValue;
}StoreInfo;

bool memlvn(Function *current);
bool mem_lvn(BasicBlock *block, HashSet *arrays,Function *currentFunction);
Array *arrayCreate(Value *array, unsigned int index);
#endif //C22V1_MEMLVN_H
