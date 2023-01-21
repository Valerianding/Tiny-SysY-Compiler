#include <function.h>


void Function_init(Function *this){
    memset(this,0,sizeof(Function));
}

Function *get_next_func(Function *this){
    return this->Next;
}

Function *function_create(){
    Function *this = (Function*)malloc(sizeof(Function));
    memset(this,0,sizeof(Function));
    return this;
}

void func_set(Function *this,BasicBlock *head,BasicBlock *tail){
    this->head = head;
    this->tail = tail;
    /*
     * #TODO 如何让所有基本块全部划分到这个Function里面
     */

    BasicBlock *entry = this->head;
    entry->Parent = this;
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
        cur->Parent = this;
        if (cur->true_block) {
            LinkedListAdd(tempList,cur->true_block);
            LinkedListAdd(allNodeList,cur->true_block);
        }
        if (cur->false_block) {
            LinkedListAdd(tempList,cur->false_block);
            LinkedListAdd(allNodeList,cur->false_block);
        }
    }
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
    printf("------------------------\n");
    printf("function : %p head : %p tail : %p\n",this,this->head,this->tail);
}