//
// Created by 12167 on 2023/8/2.
//

#ifndef C22V1_GRAPH_COLOR_H
#define C22V1_GRAPH_COLOR_H

#include "instruction.h"
#include "container/stack.h"
#include "bblock.h"
#include "function.h"
#include "utility.h"
#include "line_scan.h"
#include "dominance.h"
#include <math.h>

#define K 10
#define  _HEURISTIC_BASE 1.45

extern int myreg[16];

typedef enum State_type_{
    USUAL,
    PreColored,          //我们好像都没有这个type
    Virtual,             //我们好像都没有这个type
    Allocated,
}State_type;

typedef struct Node_{
    Value *value;

    //对于每一个非预着色的虚拟寄存器 this , adjOpdSet 是与 this 冲突的 Node 的集合
    HashSet *adjOpdSet;
    //从一个结点到与该结点相关的传送指令表的集合, 元素类型为MachineMove
    HashSet *moveSet;
    //当前度数
    int degree ;
    //当一条传送指令 (u, v) 已被合并,并且 v 已放入到 已合并 Operand 集合 coalescedNodeSet时,alias(v) = u
    struct Node_ *alias;

    State_type type;

    //实际寄存器号
    int reg ;

    int loopCounter;
}Node;

typedef struct MachineMove_{
    Node* src;
    Node* dst;
}MachineMove;

typedef struct AdjPair_{
    Node * u;
    Node * v;
}AdjPair;

void reg_alloca(Function *start);

#endif //C22V1_GRAPH_COLOR_H
