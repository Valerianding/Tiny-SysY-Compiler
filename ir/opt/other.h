//
// Created by Valerian on 2023/5/25.
//

#ifndef C22V1_OTHER_H
#define C22V1_OTHER_H
#include "bblock.h"
#include "function.h"
#include "loopinfo.h"
#include "sideeffect.h"
#include "utility.h"
extern InstNode *instruction_list;
void RemoveUselessFunction(InstNode *insHead);
#endif //C22V1_OTHER_H
