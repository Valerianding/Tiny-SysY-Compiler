#ifndef USE_DEF
#define USE_DEF

#include "value.h"

/// A Use represents the edge between a Value definition and its users.
///
/// This is notionally a two-dimensional linked list. It supports traversing
/// all of the uses for a particular value definition. It also supports jumping
/// directly to the used value when we arrive from the User's operands, and
/// jumping directly to the User when we arrive from the Value's uses.
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

#endif