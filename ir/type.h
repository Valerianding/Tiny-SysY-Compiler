#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>


#define nullptr 0

enum TypeID {
    Param_INT,
    Param_FLOAT,
    MAIN_INT,        //只有一个返回语句的main函数
    MAIN_FLOAT,

                        // 即使有初始化 值也可能改变
    Var_INT,          //无初始值的int型    变量
    Var_FLOAT,        //无初始值的float型  变量
    Var_initINT,      //有初始值的int型    变量
    Var_initFLOAT,    //有初始值的float型  变量
    Int,              //int类型整数
    Float,            //float类型浮点数

    Const_INT,        //立即数
    Const_FLOAT,

    ArrayTyID,
    ArrayTyID_Init,
    ArrayTyID_Const,
    FunctionTyID,
    VoidTyID,

    AddressTyID,       //地址
};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24;     // Space for subclasses to store data.
//    struct _Type * const *ContainedTys;
}Type;

bool isIntType(Type *type);
bool isFloatType(Type *type);
bool isVarType(Type *type);
bool isImmType(Type *type);
#endif