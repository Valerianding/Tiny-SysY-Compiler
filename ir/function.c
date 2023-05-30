#include "function.h"

Function *function_create(){
    Function *this = (Function*)malloc(sizeof(Function));
    memset(this,0,sizeof(Function));
    return this;
}

void func_set(Function *this,BasicBlock *head,BasicBlock *tail){
    this->entry = head;
    this->tail = tail;
    HashSet *allNode = HashSetInit();
    InstNode *begin = head->head_node;
    InstNode *end = tail->tail_node;
    while(begin != end){
        BasicBlock *block = begin->inst->Parent;
        if(!HashSetFind(allNode,block)){
            HashSetAdd(allNode,block);
        }
        begin = get_next_inst(begin);
    }

    HashSetFirst(allNode);
    //printf("function contain:");
    for(BasicBlock *block = HashSetNext(allNode); block != NULL; block = HashSetNext(allNode)){
        //printf(" %p",block);
        block->Parent = this;
    }
    HashSetDeinit(allNode);
    printf("\n");
}

InstNode *get_func_start(InstNode *cur){
    struct sc_list *temp = &cur->list;
    while(cur->list.prev != NULL){
        temp = temp->prev;
        InstNode *prev = sc_list_entry(temp,InstNode,list);
        if(prev->inst->Opcode == FunBegin){
            return prev;
        }
    }
    return nullptr;
}

void print_function_info(Function *this){
    printf("function : %p entry : b%d tail : b%d\n", this, this->entry->id, this->tail->id);
    printf("------------------------\n");
}