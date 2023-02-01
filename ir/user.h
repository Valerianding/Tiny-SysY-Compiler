#ifndef USER_DEF
#define USER_DEF

#include "value.h"
#include "use.h"
typedef struct _User User;

struct _User{
    Value value;
    struct _Use *use_list;
};

User *user_new();
User *user_new1(unsigned use_num);
int user_get_size();
User* user_construct(void* place, int use_num);
Use* user_get_operand_use(User* this, unsigned i);
#endif