#include "bblock.h"

void bblock_init(BasicBlock* this, Function* func){
    this->Parent = func;
    sc_list_init(&this->inst_list);
}

// only used by bblock
static InstNode* new_inst_node(Instruction* inst){
    InstNode* n = malloc(sizeof(InstNode));
    n->inst = inst;
    // 初始化结点
    sc_list_init(&(n->list));
    return n;
}

void bblock_add_inst_back(BasicBlock* this, Instruction* inst){
    InstNode* node = new_inst_node(inst);
    sc_list_add_tail(&this->inst_list, &node->list);
}


Instruction* bblock_pop_inst_back(BasicBlock* this){
    struct sc_list *tmp = sc_list_pop_tail(&this->inst_list);
	InstNode* pnode = sc_list_entry(tmp, InstNode, list);
    return pnode->inst;
}

Function *bblock_get_parent(BasicBlock* this){ 
    return this->Parent; 
}