#include "use.h"

void use_create(Use *this, struct _User *Parent){
    this->Parent = Parent;
}

void use_add_to_list(Use *this, Use **List) {
     this->Next = *List;
     if (this->Next)
       this->Next->Prev = &(this->Next);
     this->Prev = List;
     *(this->Prev) = this;
}

void use_remove_from_list(Use *this) {
    if(this->Prev)
        *(this->Prev) = this->Next;
    if (this->Next)
        this->Next->Prev = this->Prev;
}


void use_set_value(Use* this, Value *V) {
  if (this->Val) 
    use_remove_from_list(this);
  this->Val = V;
  if(V)
    value_add_use(V, this);
}
