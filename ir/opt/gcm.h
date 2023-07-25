//
// Created by Valerian on 2023/7/22.
//

#ifndef C22V1_GCM_H
#define C22V1_GCM_H
#include "utility.h"
#include "function.h"
#include "dominance.h"
void markDominanceDepth(Function *function);
void ScheduleEarly(Function *currentFunction);
void clearInsVisited(Function *function);
void ScheduleLate(Function *function);
#endif //C22V1_GCM_H
