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
    if(type->ID == ArrayTy_INT || type->ID == ArrayTyID_ConstINT){
        return true;
    }
    return false;
}

bool isLocalArrayFloatType(Type *type){
    if(type->ID == ArrayTy_FLOAT || type->ID == ArrayTyID_ConstFLOAT){
        return true;
    }
    return false;
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
    if(type->ID == GlobalArrayInt || type->ID == GlobalArrayConstINT){
        return true;
    }
    return false;
}

bool isGlobalArrayFloatType(Type *type){
    if(type->ID == GlobalArrayFloat || type->ID == GlobalArrayConstFLOAT){
        return true;
    }
    return false;
}

bool isGlobalType(Type* type){
    if(type->ID == GlobalArrayConstINT || type->ID == GlobalArrayInt || type->ID ==GlobalVarInt || type->ID ==GlobalVarFloat || type->ID ==GlobalArrayFloat || type->ID ==GlobalArrayConstFLOAT)
        return true;
    return false;
}

bool isConstant(Type* type){
    if(type->ID == Int || type->ID == Float)
        return true;
    return false;
}

bool isFloatType(int type){
    if(type == Float || type == Var_FLOAT || type == Const_FLOAT || type == GlobalArrayConstFLOAT || type == GlobalArrayFloat || type == GlobalVarFloat || type == ArrayTy_FLOAT || type == ArrayTyID_ConstFLOAT)
        return true;
    return false;
}

bool isIntType(int type){
    if(type == Int || type == Var_INT || type == Const_INT || type == GlobalArrayConstINT || type == GlobalArrayInt || type == GlobalVarInt || type == ArrayTy_INT || type == ArrayTyID_ConstINT)
        return true;
    return false;
}

void typePrinter(Type *type){
    switch (type->ID) {
        case Unknown:{
            printf("Unknow ");
            break;
        }
        case Int:{
            printf("Int ");
            break;
        }
        case Float:{
            printf("Float ");
            break;
        }
        case AddressTyID:{
            printf("Addresstype ");
            break;
        }
        case Var_INT:{
            printf("var_int ");
            break;
        }
        case Var_FLOAT:{
            printf("var_float ");
            break;
        }
        case GlobalVarFloat:{
            printf("GlobalVarFloat ");
            break;
        }
        case GlobalVarInt:{
            printf("GlobalVarInt ");
            break;
        }
        case ArrayTyID_ConstINT:{
            printf("ArrayConstInt ");
            break;
        }
        case ArrayTyID_ConstFLOAT:{
            printf("ArrayConstFloat ");
            break;
        }
        case GlobalArrayConstINT:{
            printf("GlobalArrayConstInt ");
            break;
        }
        case GlobalArrayConstFLOAT:{
            printf("GlobalArrayConstFloat ");
            break;
        }
        case ArrayTy_INT:{
            printf("ArrayInt ");
            break;
        }
        case ArrayTy_FLOAT:{
            printf("ArrayFloat ");
            break;
        }
        case GlobalArrayInt:{
            printf("GlobalArrayInt ");
            break;
        }
        case GlobalArrayFloat:{
            printf("GlobalArrayFloat ");
            break;
        }
        default:{
            printf("error Type ");
        }
    }
}