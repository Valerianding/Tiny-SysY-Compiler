#ifndef USE_DEF
#define USE_DEF

#include "value.h"
struct _Value;
struct _User;

typedef struct _Use Use;
struct _Use{
  
   struct _Value *Val;
   struct _Use *Next;
   struct _Use **Prev;
   struct _User *Parent;
  
};


void use_create(Use *this, struct _User *Parent);
void use_add_to_list(Use *this, Use **List);
void use_set_value(Use* this, Value *V);

#endif