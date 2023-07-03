#ifndef VALUE_DEF
#define VALUE_DEF
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "type.h"
#include "symtab.h"
#include "hash_set.h"

struct _BasicBlock;
struct _Value;
struct _Symtab;
struct _Use;
typedef struct _Use Use;

// 为了phi指令设计的
typedef struct pair{
    struct _Value *define; // 变量的到达
    struct _BasicBlock *from; //从哪个基本块来的
}pair;

#define  NumUserOperandsBits 27


/* 怎么能有这么丑陋的数据结构 受不了一点！*/
struct _PData{
    struct {
        int true_goto_location;       //跳转位置
        int false_goto_location;
    }instruction_pdata;

    struct {
        union {
            int iVal;
            float fVal;
        };
        unsigned int is_offset:1;
    }var_pdata;

    struct {
        Type return_type;            //返回类型
        Type param_type_lists[1000];   //参数类型数组
        int param_num;
        unsigned int flag_inline:1;    //标记是否能内联，初始都不能，就为0
    }symtab_func_pdata;            //目前只在符号表里用的func的结构，最终func结构还未完全确定

    struct {
        int dimention_figure;               //数组维数是一维、二维......
        int dimentions[10];                 //每维的具体值，a[2][3]中的2,3；如果是参数数组，第一维是负数
        unsigned int address_type:1;     //type为address的时候标识是int还是float，0是int,1是float

        union {
            int array[301];                 //memcpy使用
            float f_array[301];
        };
        unsigned is_init:1;  //用于判断数组是否初始化
    }symtab_array_pdata;

    struct _mapList* map_list;

    HashSet *pairSet; // 为了phi指令设计的 存pair类型的数据
    unsigned int define_flag:1;
};
typedef struct _PData PData;
typedef struct _Value Value;
struct _Value{
    Type *VTy;
    struct _Use *use_list;
    unsigned NumUserOperands : NumUserOperandsBits;
    unsigned HasHungOffUses : 1;
    unsigned HasName : 1;
    bool Useless : 1;
    bool IsPhi : 1;

    HashSet *visitedObjects; //访问过的变量和数组
    bool containInput : 1; //是否含有输入
    bool containOutput : 1; //是否含有输出

    char *name;
    struct _Value *alias;
    PData *pdata;
};

void value_init(Value* this);
void value_add_use(Value* this, struct _Use *U);
struct _Symtab* get_sym_tab(Value *V);
void value_init_int(Value *this,int num);
void value_init_float(Value *this,float num);
void value_replaceAll(Value *oldValue,Value *newValue);

bool isImm(Value *val);  //是否是立即数
bool isImmInt(Value *val); //是否是整数立即数
bool isImmFloat(Value *val); //是否是浮点立即数

bool isLocalVar(Value *val);   //是否是局部变量
bool isLocalVarInt(Value *val); //是否是局部整型变量 包含了参数
bool isLocalVarFloat(Value *val); // 是否是局部浮点型变量 包含了参数

bool isLocalArray(Value *val); // 是否是局部数组
bool isLocalArrayInt(Value *val); // 是否是局部int类型数组
bool isLocalArrayFloat(Value *val); // 是否是局部float类型数组

bool isGlobalVar(Value *val); // 是否全局变量
bool isGlobalVarInt(Value *val); // 是否全局int变量
bool isGlobalVarFloat(Value *val); // 是否全局float变量

bool isGlobalArray(Value *val); // 是否是全局数组
bool isGlobalArrayInt(Value *val); // 是否全局int数组
bool isGlobalArrayFloat(Value *val); // 是否全局float数组
bool isAddress(Value *val);
bool isArrayInitialize(Value *val); // 判断是否初始化了数组
#endif