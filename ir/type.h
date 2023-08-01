#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>
#include <stdio.h>
#define nullptr 0

enum TypeID {
    Unknown = 0,
    Param_INT = 1,       //只有前端使用
    Param_FLOAT = 2,     //只有前端使用
    MAIN_INT = 3,        //只有一个返回语句的main函数,只有前端使用
    MAIN_FLOAT = 4,      //只有前端使用

    Int = 5,              //int类型整数
    Float = 6,            //float类型浮点数

    Const_INT = 7,        // 常变量 只有前端使用
    Const_FLOAT = 8,      //只有前端使用


    FunctionTyID = 9,      //表明是函数类型的value
    VoidTyID = 10,          //主要用于标识函数返回值类型

    AddressTyID = 11,       //地址

    Var_INT = 12,          //无初始值的int型    变量
    Var_FLOAT = 13,        //无初始值的float型  变量
    GlobalVarInt = 14,  //全局INT类型变量
    GlobalVarFloat = 15, //全局Float类型变量

    ArrayTyID_ConstINT = 16,
    ArrayTyID_ConstFLOAT = 17,

    GlobalArrayConstINT = 18,
    GlobalArrayConstFLOAT = 19,

    ArrayTy_INT = 20,
    ArrayTy_FLOAT = 21,

    GlobalArrayInt = 22, // 全局Int类型数组
    GlobalArrayFloat = 23, // 全局float类型数组
};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24;     // Space for subclasses to store data.
}Type;


bool isImmIntType(Type *type);
bool isImmFloatType(Type *type);
bool isLocalVarIntType(Type *type);
bool isLocalVarFloatType(Type *type);
bool isLocalArrayIntType(Type *type);
bool isLocalArrayFloatType(Type *type);
bool isGlobalVarIntType(Type *type);
bool isGlobalVarFloatType(Type *type);
bool isGlobalArrayIntType(Type *type);
bool isGlobalArrayFloatType(Type *type);
bool isGlobalType(Type* type);
bool isConstant(Type* type);

//lsy
bool isFloatType(int type);
void typePrinter(Type *type);
#endif