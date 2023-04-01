#include "type.h"

bool isIntType(Type *type){
    if(type->ID == Var_INT ){
        return true;
    }
    return false;
}

bool isFloatType(Type *type){
    if(type->ID == Var_FLOAT ){
        return true;
    }
    return false;
}

bool isVarType(Type *type){
    if( type->ID == Var_FLOAT  || type->ID == Var_INT){
        return true;
    }
    return false;
}

bool isImmType(Type *type){
    if(type->ID == Int || type->ID == Float){
        return true;
    }
    return false;
}

bool isArrayType(Type *type){
    if(type->ID == ArrayTy_FLOAT || type->ID==ArrayTy_INT || type->ID == ArrayTyID_ConstINT || type->ID==ArrayTyID_ConstFLOAT || type->ID == GlobalArrayInt || type->ID == GlobalArrayFloat){
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