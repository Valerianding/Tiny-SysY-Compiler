#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>
#include <stdio.h>
#define nullptr 0

enum TypeID {
    Unknown = 0,
    Param_INT,       //只有前端使用
    Param_FLOAT,     //只有前端使用
    MAIN_INT,        //只有一个返回语句的main函数,只有前端使用
    MAIN_FLOAT,      //只有前端使用

    Int,              //int类型整数
    Float,            //float类型浮点数

    Const_INT,        // 常变量 只有前端使用
    Const_FLOAT,      //只有前端使用


    FunctionTyID,      //表明是函数类型的value
    VoidTyID,          //主要用于标识函数返回值类型

    AddressTyID,       //地址

    Var_INT,          //无初始值的int型    变量
    Var_FLOAT,        //无初始值的float型  变量
    GlobalVarInt,  //全局INT类型变量
    GlobalVarFloat, //全局Float类型变量

    ArrayTyID_ConstINT,
    ArrayTyID_ConstFLOAT,
    GlobalArrayConstINT,
    GlobalArrayConstFLOAT,
    ArrayTy_INT,
    ArrayTy_FLOAT,
    GlobalArrayInt, // 全局Int类型数组
    GlobalArrayFloat, // 全局float类型数组
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
void typePrinter(Type *type);
#endif