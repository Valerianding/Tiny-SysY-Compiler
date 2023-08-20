#include <stdlib.h>
#include <//assert.h>
#include "user.h"
#include "use.h"


User *user_new(){
    int size = sizeof(User);
    void *Storage = malloc(size);
    User *Obj = (User *)(Storage);
    (Obj->use_list) = Storage;
    (Obj->value).NumUserOperands = 0;
    (Obj->value).HasHungOffUses = true;
    return Obj;
}


User *user_new1(unsigned use_num){
    uint8_t *Storage = (uint8_t *)malloc(sizeof(User) + sizeof(Use) * use_num);
    return user_construct(Storage, use_num);
}

User* user_construct(void* place, int use_num){
    // 在place地方建立一个user，内在已分配好
    Use *Start = (Use *)(place);
    Use *End = Start + use_num;
    User *Obj = (User *)(End);
    if(use_num == 0){
        Obj->use_list = NULL;
        Obj->value.NumUserOperands = 0;
        Obj->value.HasHungOffUses = false;
        return Obj;
    }else{
        Obj->use_list = Start;
    }
    Obj->value.NumUserOperands = use_num;
    Obj->value.HasHungOffUses = false;

    for (; Start != End; Start++){
        // 在 Start 地址上用构造 Use对象
        // new (Start) Use(Obj);
        use_create(Start, Obj);
    }
    return Obj;
}

int user_get_size(int use_num){
    if(use_num > 0)
        return sizeof(User) + sizeof(Use) * use_num;
    else
        return sizeof(User);
}

Use* user_get_operand_use(User* this, unsigned i) {
    if(this->value.HasHungOffUses)
        return this->use_list;
    else{
        //assert(i < this->value.NumUserOperands && "user_get_operand_use() out of range!");
        return &(this->use_list[i]);
    }
}

Use *getHungOffOperands(User* this) { 
    // return *(reinterpret_cast<Use **>(this) - 1); 
    return *((Use **)(this) - 1); 
}

Use* get_operand(User* this, int ind){
    if(this->value.HasHungOffUses){
        return this->use_list;
    }else{
        //assert(this->value.NumUserOperands > ind && "operand index overflow");
        return this->use_list + ind;
    }
}