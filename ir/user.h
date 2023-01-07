#ifndef USER_DEF
#define USER_DEF

#include "value.h"
#include "use.h"
typedef struct _User User;

struct _User{
    // user 使用多个 value，在LLVM中， 通过函数 allocate_fixed_operand_user 及 user 的构造函数
    // （重载 new），在创建 user 的同时，创建了相应的 use，相关于在 user 之前有1个 use 或 多个 use
    // llvm 是隐式的创建，这里是显示的创建
    
    // user 继承 value
    Value value;
    
    struct _Use *use_list;
};

User *user_new();
User *user_new1(unsigned use_num);
int user_get_size();
User* user_construct(void* place, int use_num);
Use* user_get_operand_use(User* this, unsigned i);
#endif