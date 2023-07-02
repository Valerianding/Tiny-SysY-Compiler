
//
// Created by Valerian on 2023/7/1.
//

#ifndef C22V1_LICM_H
#define C22V1_LICM_H
#include "Loop.h"
#include "utility.h"
bool dfsLoopTree(Loop *loop);
bool LICM_EACH(Loop *loop);
#endif //C22V1_LICM_H
