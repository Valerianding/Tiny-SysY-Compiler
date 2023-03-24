#include "type.h"

bool isIntType(Type *type){
    if(type->ID == Int || type->ID == Var_INT || type->ID == Var_initINT)
        return true;
    else
        return false;
}

bool isFloatType(Type *type){
    if(type->ID == Float || type->ID == Var_FLOAT || type->ID == Var_initFLOAT)
        return true;
    else
        return false;
}

bool isVarType(Type *type){
    if(type->ID == Var_initFLOAT || type->ID == Var_FLOAT || type->ID == Var_initINT || type->ID == Var_INT)
        return true;
    else
        return false;
}

bool isImmType(Type *type){
    if(type->ID == Int || type->ID == Float)
        return true;
    else
        return false;
}

bool isArrayType(Type *type){
    if(type->ID == ArrayTyID || type->ID == ArrayTyID_Const || type->ID == ArrayTyID_Init || type->ID == GlobalArrayInt || type->ID == GlobalArrayFloat){
        return true;
    }
    return false;
}

bool isGlobalVarType(Type *type){
    if(type->ID == GlobalVarFloat || type->ID == GlobalVarInt){
        return true;
    }
    return false;
}

bool isGlobalArrayType(Type *type){
    if(type->ID == GlobalArrayFloat || type->ID == GlobalArrayInt){
        return true;
    }
    return false;
}