#include "bblock.h"
#include "stdio.h"
void bblock_init(BasicBlock* this, Function* func) {
    //对头上的两个后继节点另外开辟use
    uint8_t *storage = (uint8_t*)malloc(sizeof(Use) * 2);
    Use *temp1 = (Use*)storage;
    temp1->Parent = &this->user;

    Use *temp2 = (Use*)(storage) + 1;
    temp2->Parent = &this->user;
    //
    this->user.use_list = (Use*)storage;
    value_init(&this->user.value);
    this->Parent = func;
    this->inst_list = NULL;
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
    if(this->inst_list == NULL){
        this->inst_list = node;
    }else{
        sc_list_add_tail(&((this->inst_list)->list), &node->list);
    }
    if((this->inst_list)->inst == NULL) printf("Is NULL\n");
}


void bblock_pop_inst_back(BasicBlock* this){
    struct sc_list *tmp = sc_list_pop_tail(&(this->inst_list)->list);
	//InstNode* pnode = sc_list_entry(tmp, InstNode, list);

    //return pnode->inst;
}

InstNode* bblock_get_inst_back(BasicBlock* this){
    struct sc_list *temp = sc_list_tail(&(this->inst_list)->list);
    InstNode* pnode = sc_list_entry(temp,InstNode,list);
    return pnode;
}

InstNode* bblock_get_inst_head(BasicBlock* this){
    return this->inst_list;
}

Function *bblock_get_parent(BasicBlock* this){ 
    return this->Parent; 
}

void moveBefore(BasicBlock *this,BasicBlock *MovePos){
    Use *temp = this->user.use_list;
    if(temp->Val == NULL){
        use_set_value(temp,&MovePos->user.value);
    }else{
        temp = temp + 1;
        if(temp->Val == NULL) {
            use_set_value(temp,&MovePos->user.value);
        }else{
            printf("already have two kids!\n");
        }
    }
}

void moveAfter(BasicBlock *this,BasicBlock *MovePos){
    Use *temp = MovePos->user.use_list;
    if(temp->Val == NULL){
        use_set_value(temp,&this->user.value);
    }else{
        temp = temp + 1;
        if(temp->Val == NULL){
            use_set_value(temp,&this->user.value);
        }else{
            printf("already have two kids!\n");
        }
    }
}

size_t bb_count_ins(BasicBlock *this){
    if(this->inst_list == NULL) return 0;
    else return sc_list_count(&(this->inst_list)->list);
}