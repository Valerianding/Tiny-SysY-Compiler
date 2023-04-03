#include "type.h"

bool isImmIntType(Type *type){
    if(type->ID == Int){
        return true;
    }
    return false;
}

bool isImmFloatType(Type *type){
    if(type->ID == Float){
        return true;
    }
    return false;
}

bool isLocalVarIntType(Type *type){
    if(type->ID == Var_INT){
        return true;
    }
    return false;
}

bool isLocalVarFloatType(Type *type){
    if(type->ID == Var_FLOAT){
        return true;
    }
    return false;
}

bool isLocalArrayIntType(Type *type){

}

bool isLocalArrayFloatType(Type *type){

}

bool isGlobalVarIntType(Type *type){
    if(type->ID == GlobalVarInt){
        return true;
    }
    return false;
}

bool isGlobalVarFloatType(Type *type){
    if(type->ID == GlobalVarFloat){
        return true;
    }
    return false;
}

bool isGlobalArrayIntType(Type *type){
    if(type->ID == GlobalArrayInt){
        return true;
    }
    return false;
}

bool isGlobalArrayFloatType(Type *type){
    if(type->ID == GlobalArrayFloat){
        return true;
    }
    return false;
}