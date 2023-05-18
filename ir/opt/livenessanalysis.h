//
// Created by Valerian on 2023/3/19.
//

#ifndef C22V1_LIVENESSANALYSIS_H
#define C22V1_LIVENESSANALYSIS_H
#include "utility.h"
void calculateLiveness(Function *currentFunction);
void calculateLiveness1(Function *currentFunction);
bool HashSetCopyValue(HashSet *dest, HashSet *src);
void printLiveness(Function *currentFunction);
void cleanLiveSet(Function *currentFunction);
#endif //C22V1_LIVENESSANALYSIS_H
