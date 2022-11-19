#ifndef VALUE_DEF
#define VALUE_DEF
#include <stdlib.h>
#include <stdbool.h>
// #include <stddef.h>
#include "type.h"
#include <stdint.h>

struct _Symtab;
struct _Use;

#define  NumUserOperandsBits 27

typedef struct _Value Value;
struct _Value
{
    Type *VTy;
    struct _Use *use_list;

    unsigned char HasValueHandle : 1; // Has a ValueHandle pointing to this?

    unsigned NumUserOperands : NumUserOperandsBits;

    // Use the same type as the bitfield above so that MSVC will pack them.
    unsigned IsUsedByMD : 1;
    unsigned HasName : 1;
    unsigned HasMetadata : 1; // Has metadata attached to this?
    unsigned HasHungOffUses : 1;


    void *pdata;
};

// llvm 中定义了 Constant ConstantInt, ConstantFP 等类，且Constant继承于 User
// 这里做了简化： ConstantNum 只处理常数， Function 之类的常数不在此处理，所以不用考虑 User
//typedef struct _ConstantNum ConstantNum;
//struct _ConstantNum{
//    struct _Value value;
//    union{
//        int num_int;
//        float num_float;
//    }num;
//};

void value_init(Value* this);
//void const_init_int(Value* this, int num);
//void const_init_float(Value* this, float num);

void value_add_use(Value* this, struct _Use *U);
Type *getType(Value* this);
void value_set_name(Value* this, char* name);
struct _Symtab* get_sym_tab(Value *V);

/* 需要一个为value设置pdata的函数 */
void value_init_int(Value *this,int num);
void value_init_float(Value *this,float num);

//ConstantNum* const_new_int(int num);
//ConstantNum* const_new_float(float num);

#endif