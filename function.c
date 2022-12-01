//
// Created by Valerian on 2022/12/1.
//

#include "function.h"

BasicBlockNode* new_BasicBlockNode(BasicBlock *this){
    BasicBlockNode *temp = (BasicBlockNode*)malloc(sizeof(BasicBlockNode));
    temp->block = this;
    sc_list_init(&temp->list);
    return temp;
}

void Function_init(Function *this){
    this->bb_list = NULL;
    this->Next = NULL;
    //this->Prev = NULL;
}

void Function_add_block_back(Function *this,BasicBlock *block){
    //this->bb_list->list
    BasicBlockNode *temp = new_BasicBlockNode(block);
    sc_list_add_tail(&this->bb_list->list,&temp->list);
}

void Function_pop_block_back(Function *this){
    sc_list_pop_tail(&this->bb_list->list);
}

BasicBlockNode *Function_get_block_back(Function *this){
    struct sc_list *list = sc_list_tail(&this->bb_list->list);
    BasicBlockNode *temp = sc_list_entry(list,BasicBlockNode,list);
    return temp;
}

void Function_add_function(Function *this,Function *next){
    assert(this->Next == NULL);
    this->Next = next;
}

