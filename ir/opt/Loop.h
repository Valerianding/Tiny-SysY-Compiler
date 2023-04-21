//
// Created by Valerian on 2023/4/18.
//

#ifndef C22V1_LOOP_H
#define C22V1_LOOP_H
#include "function.h"
#include "utility.h"
#include <stack.h>
void loop(Function *currentFunction);
void findbody(BasicBlock *head,BasicBlock *tail);
void loopVariant(HashSet *loop, BasicBlock *head);
#endif //C22V1_LOOP_H
