#ifndef TYPE_DEF
#define TYPE_DEF

#include <stdbool.h>


#define nullptr 0

enum TypeID {
    Param_INT,
    Param_FLOAT,
    MAIN_INT,        //只有一个返回语句的main函数
    MAIN_FLOAT,

    Var_INT,          //无初始值的int型变量
    Var_FLOAT,        //无初始值的float型变量
    Var_initINT,      //有初始值的int型变量
    Var_initFLOAT,    //有初始值的float型变量
    Int,              //int类型整数
    Float,            //float类型浮点数

    Const_INT,
    Const_FLOAT,

    ArrayTyID,
    ArrayTyID_Init,
    FunctionTyID,
    VoidTyID,

};

typedef struct _Type{
    enum TypeID   ID : 8;            // The current base type of this type.
    unsigned SubclassData : 24;     // Space for subclasses to store data.
//    struct _Type * const *ContainedTys;
}Type;

bool isVoidTy(Type *this);
bool isFloatTy(Type *this);
bool isIntegerTy(Type *this);
bool isFunctionTy(Type *this);
bool isFloatingPointTy(Type *this);
bool isPointerTy(Type *this);
bool isConstTy(Type *this);

#endif