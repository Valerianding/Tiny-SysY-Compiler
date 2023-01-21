//
// Created by Valerian on 2023/1/18.
//

#include "dominance.h"
HashMap *dominace;  // value: block key: Dom(block)->hashset(block)
LinkedBlockList *LinkedListCreate(){
    LinkedBlockList *list = (LinkedBlockList*)malloc(sizeof(LinkedBlockList));
    LinkedListInit(list);
    return list;
}

void LinkedListInit(LinkedBlockList *list){
    memset(list,0,sizeof(LinkedBlockList));
}

void LinkedListAdd(LinkedBlockList *list,BasicBlock *block){
    BlockNode *currentBlockNode = (BlockNode*)malloc(sizeof(BlockNode));
    currentBlockNode->block = block;
    sc_list_init(&currentBlockNode->list);
    sc_list_add_tail(&list->dummpNode.list,&currentBlockNode->list);
    list->size++;
}

BasicBlock *LinkedListPop(LinkedBlockList *list){
    if(LinkedListEmpty(list)) return nullptr;
    struct sc_list *prev = nullptr;
    struct sc_list *curr = &list->dummpNode.list;
    while(curr->next != nullptr){
        prev = curr;
        curr = curr->next;
    }
    prev->next = nullptr;
    BlockNode *blockNode = sc_list_entry(curr,BlockNode,list);
    list->size--;
    return blockNode->block;
}

bool LinkedListEmpty(LinkedBlockList *list){
    if(list->size == 0 && list->dummpNode.list.next == NULL) return true;
    else return false;
}

void calculate_dominance(Function *currentFunction) {
    dominace = HashMapInit();

    BasicBlock *entry = currentFunction->head;
    LinkedBlockList *tempList = LinkedListCreate(); // 临时存放Block
    LinkedBlockList *allNodeList = LinkedListCreate();  // 存放所有Block
    //全部链表
    LinkedListAdd(allNodeList,entry);

    if (entry->true_block) {
        LinkedListAdd(allNodeList,entry->true_block);
        LinkedListAdd(tempList,entry->true_block);
    }
    if (entry->false_block) {
        LinkedListAdd(allNodeList,entry->false_block);
        LinkedListAdd(tempList,entry->false_block);
    }

    while (!LinkedListEmpty(tempList)) {
        BasicBlock *cur = LinkedListPop(tempList);
        if (cur->visited) continue;
        cur->visited = true;
        if (cur->true_block) {
            LinkedListAdd(tempList,cur->true_block);
            LinkedListAdd(allNodeList,cur->true_block);
        }
        if (cur->false_block) {
            LinkedListAdd(tempList,cur->false_block);
            LinkedListAdd(allNodeList,cur->false_block);
        }
    }

    HashSet *entrydom = HashSetInit();
    HashSetAdd(entrydom,entry);
    HashMapPut(dominace,entry,entrydom);
//    for(){
//        HashSet *currentBlockDom = HashSetInit();
//        HashSetPutALL()
//    }
//    bool changed = true;
//    while(changed){
//        changed = false;
//        //
//        for(){
//
//        }
//    }
}