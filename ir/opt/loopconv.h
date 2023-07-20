//
// Created by Valerian on 2023/7/18.
//

#ifndef C22V1_LOOPCONV_H
#define C22V1_LOOPCONV_H
#include "utility.h"
#include "loopinfo.h"
extern int instruction_uid;
bool CheckLoop(Loop *loop);
bool LoopConversion(Function *currentFunction);
bool dfsTravel(Loop *root);
#endif //C22V1_LOOPCONV_H
