//
// Created by Valerian on 2023/7/22.
//

#ifndef C22V1_GCM_H
#define C22V1_GCM_H
#include "utility.h"
#include "function.h"
#include "dominance.h"
#include "dvnt.h"
void markDominanceDepth(Function *function);
void markLoopNest(Function *function);
void ScheduleEarly(Function *currentFunction);
void clearInsVisited(Function *function);
void ScheduleLate(Function *function);
void printALLInfo(Function *function);
void GCM(Function *currentFunction);
#endif //C22V1_GCM_H
