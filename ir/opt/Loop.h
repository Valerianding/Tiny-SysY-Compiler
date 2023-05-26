//
// Created by Valerian on 2023/4/18.
//

#ifndef C22V1_LOOP_H
#define C22V1_LOOP_H
#include "function.h"
#include "utility.h"
#include <stack.h>
typedef struct Loop{
    struct Loop *parent;
    BasicBlock *head;
    BasicBlock *tail; //back-edge's tail
    HashSet *exit;
    HashSet *loopBody;
    HashSet *child; // loop *
}Loop;


// 每次需要对循环做优化的时候就再计算
void loop(Function *currentFunction);
Loop *constructLoop(BasicBlock *head,BasicBlock *tail);
void loopVariant(HashSet *loop, BasicBlock *head);
#endif //C22V1_LOOP_H
