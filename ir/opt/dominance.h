//
// Created by Valerian on 2023/1/18.
//

#ifndef C22V1_DOMINANCE_H
#define C22V1_DOMINANCE_H
#include "hash.h"
#include "hash_set.h"
#include "hash_map.h"

/*
 * 如何求一个基本块的支配基本块？
 * Dom n = n 并 n 的 前驱节点m的支配节点的交集
 * 1.移除没有users的alloca指令
 * 2.
 */

#include "hash.h"
#include "hash_set.h"
#include "hash_map.h"
#include "bblock.h"
#include "function.h"
typedef struct{
    int size;
    BlockNode dummpNode;
}LinkedBlockList;
LinkedBlockList *LinkedListCreate();
void LinkedListInit(LinkedBlockList *list);
void LinkedListAdd(LinkedBlockList *list,BasicBlock *block);
BasicBlock *LinkedListPop(LinkedBlockList *list);
bool LinkedListEmpty(LinkedBlockList *list);



void HashSetPutAll(HashSet *self,BasicBlock *current,BlockList *list);
void calculate_all_dominance(Function *head);
void calculate_dominance(Function *currentFunction);
#endif //C22V1_DOMINANCE_H
