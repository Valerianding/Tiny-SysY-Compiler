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

struct _PData{
    struct {
        int true_goto_location;       //跳转位置
        int false_goto_location;
    }instruction_pdata;

    struct {
        struct sc_map_sv* map;         //所指向的那张作用域的表,具体看后端需不需要，不需要其实都可以删了
        union {
            int iVal;
            float fVal;
        };
    }var_pdata;

    struct {
        Type return_type;            //返回类型
        Type param_type_lists[50];   //参数类型数组
        struct _mapList* map_list;         //所指向的那张函数作用域的表
        int param_num;              //TODO 可能要，也可能不要
    }symtab_func_pdata;            //目前只在符号表里用的func的结构，最终func结构还未完全确定

    struct {
        struct sc_map_sv* map;         //所指向的那张作用域的表,具体看后端需不需要，不需要其实都可以删了
        Type array_type;
        int dimention_figure;               //一维、二维......
        int dimentions[10];                 //每维的具体值，a[2][3]中的2,3

        union {
            int array[100];                 //memcpy使用
            float f_array[100];
        };
    }symtab_array_pdata;

    HashSet *pairSet; // 为了phi指令设计的 存pair类型的数据
    unsigned int define_flag:1;
};
typedef struct _PData PData;
typedef struct _Value Value;
struct _Value
{
    Type *VTy;
    struct _Use *use_list;

    unsigned char HasValueHandle : 1; // Has a ValueHandle pointing to this?
    unsigned NumUserOperands : NumUserOperandsBits;

    // Use the same type as the bitfield above so that MSVC will pack them.
    unsigned IsUsedByMD : 1;
    unsigned IsInstruction : 1;
    unsigned HasName : 1;
    unsigned HasMetadata : 1; // Has metadata attached to this?
    unsigned HasHungOffUses : 1;
    unsigned is_in : 1; //入口语句
    unsigned is_out : 1; //出口语句
    unsigned is_last : 1;

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
bool isLocalVarInt(Value *val); //是否是局部整型变量 包含了参数
bool isLocalVarFloat(Value *val); // 是否是局部浮点型变量 包含了参数
bool isLocalArray(Value *val); // 是否是局部数组
bool isLocalArrayInt(Value *val); // 是否是局部int类型数组
bool isLocalArrayFloat(Value *val); // 是否是局部float类型数组
bool isGlobalVar(Value *val); // 是否全局变量
bool isGlobalVarInt(Value *val); // 是否全局int变量
bool isGlobalVarFloat(Value *val); // 是否全局float变量
bool isGlobalArrayInt(Value *val); // 是否全局int数组
bool isGlobalArrayFloat(Value *val); // 是否全局float数组
#endif