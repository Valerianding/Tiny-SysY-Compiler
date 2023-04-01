#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>


#define nullptr 0

enum TypeID {
    Unknown = 0,
    Param_INT,
    Param_FLOAT,
    MAIN_INT,        //只有一个返回语句的main函数
    MAIN_FLOAT,

                       // 即使有初始化 值也可能改变
    Var_INT,          //无初始值的int型    变量
    Var_FLOAT,        //无初始值的float型  变量
    Int,              //int类型整数
    Float,            //float类型浮点数

    Const_INT,        // 常变量 应该用不到了
    Const_FLOAT,

    ArrayTyID_ConstINT,
    ArrayTyID_ConstFLOAT,
    FunctionTyID,
    VoidTyID,

    AddressTyID,       //地址


    GlobalVarInt,  //全局INT类型变量
    GlobalVarFloat, //全局Float类型变量

    ArrayTy_INT,
    ArrayTy_FLOAT,
    GlobalArrayInt, // 全局Int类型数组
    GlobalArrayFloat, // 全局float类型数组
};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24;     // Space for subclasses to store data.
}Type;

bool isIntType(Type *type);
bool isFloatType(Type *type);
bool isVarType(Type *type);
bool isImmType(Type *type);
bool isArrayType(Type *type);
bool isGlobalArrayType(Type *type);
bool isGlobalVarType(Type *type);
#endif