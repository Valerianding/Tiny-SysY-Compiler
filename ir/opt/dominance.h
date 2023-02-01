//
// Created by Valerian on 2023/1/18.
//

#ifndef C22V1_DOMINANCE_H
#define C22V1_DOMINANCE_H
#include "hash.h"
#include "hash_set.h"
#include "hash_map.h"
#include "bblock.h"
#include "function.h"
void calculate_dominance(Function *currentFunction);
void calculate_dominance_frontier(Function *currentFunction);
void HashSetCopy(HashSet *dest,HashSet *src);
bool HashSetDifferent(HashSet *lhs,HashSet *rhs);
#endif //C22V1_DOMINANCE_H
