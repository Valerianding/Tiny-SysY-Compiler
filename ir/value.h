#ifndef VALUE_DEF
#define VALUE_DEF
#include <stdlib.h>
#include <stdbool.h>
#include "type.h"
#include <stdint.h>

struct _Symtab;
struct _Use;

#define  NumUserOperandsBits 27

union _PData{
    struct {
        int goto_location;       //跳转位置
    }instruction_pdata;

    struct {
        struct sc_map_sv* map;         //所指向的那张作用域的表
        union {
            int iVal;
            float fVal;
        };
    }var_pdata;

    struct {
        Type return_type;            //返回类型
        Type param_type_lists[10];   //参数的类型数组
        struct sc_map_sv* map;         //所指向的那张作用域的表
        int param_num;              //TODO 可能要，也可能不要
    }symtab_func_pdata;            //目前只在符号表里用的func的结构，最终func结构还未完全确定

};
typedef union _PData PData;
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
    PData *pdata;

    //代表栈帧中地址%1,%2...的value
    //TODO 暂时这样存，本来想直接在pData里加int index，但是比如后面store指令要用到%1 Value的时候，每次都要根据1创一个Value，感觉更麻烦?
    struct _Value* v_alias;
};

void value_init(Value* this);
Value *create_value(Value **v);

void value_add_use(Value* this, struct _Use *U);
Type *getType(Value* this);
void value_set_name(Value* this, char* name);
void value_get_name(Value* this, char* name);

struct _Symtab* get_sym_tab(Value *V);

/* 需要一个为value设置pdata的函数 */
void value_init_int(Value *this,int num);
void value_init_float(Value *this,float num);

#endif