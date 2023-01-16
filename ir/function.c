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
}