//
// Created by Valerian on 2023/7/30.
//

#ifndef C22V1_LOOPNORM_H
#define C22V1_LOOPNORM_H
#include "function.h"
#include "utility.h"
//requires dominance & loopinfo before
//requires recompute dominance & loopinfo after
void LoopNormalize(Function *currentFunction);
#endif //C22V1_LOOPNORM_H
