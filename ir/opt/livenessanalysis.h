//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_LIVENESSANALYSIS_H
#define C22V1_LIVENESSANALYSIS_H
#include "utility.h"
void calculateLiveness(Function *currentFunction);
bool HashSetCopyValue(HashSet *dest, HashSet *src);
void printLiveness(BasicBlock *block);
#endif //C22V1_LIVENESSANALYSIS_H
