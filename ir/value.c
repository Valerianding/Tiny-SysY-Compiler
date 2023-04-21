#include "value.h"
#include "use.h"
#include "symtab.h"

void value_add_use(Value *this, Use *U){
    use_add_to_list(U, &(this->use_list));
}

void value_init(Value* this){
    memset(this, 0, sizeof(Value));
    this->VTy = (Type*)malloc(sizeof(Type));
    this->VTy->ID = Unknown;
    this->pdata = (PData*)malloc(sizeof(PData));
    this->use_list = NULL;
}

/// All values are typed, get the type of this value.
Type *getType(Value* this) { return this->VTy; }


void value_set_name(Value* this, char* name){
    Symtab *st = get_sym_tab(this);
    if(st != NULL){
        symtab_insert_value_name(st, name, this);
        this->HasName = true;
    }
}

void value_init_int(Value *this,int num){
    value_init(this);
    this->pdata->var_pdata.iVal = num;
    this->VTy->ID=Int;
}

void value_init_float(Value *this,float num){
    value_init(this);
    this->pdata->var_pdata.fVal = num;
    this->VTy->ID=Float;
}


void value_replaceAll(Value *oldValue,Value *newValue){
    // 使用两个Use的原因是需要维护Next的正确性
    if(oldValue->use_list != NULL){
        Use *use1 = oldValue->use_list;
        Use *use2 = use1->Next;
        while(use1 != NULL){
            printf("at use : %p, replace by : %p\n",use1,newValue);
            value_add_use(newValue,use1);
            use1->Val = newValue;
            use1 = use2;
            use2 = (use2 == NULL ? NULL : use2->Next);
        }
    }
    oldValue->use_list = NULL;
}

//FIXME
struct _Symtab* get_sym_tab(Value *V){
    return NULL;
}

bool isImm(Value *val){
    if(isImmInt(val) || isImmFloat(val)){
        return true;
    }
    return false;
}

bool isImmInt(Value *val){
    if(isImmIntType(val->VTy)){
        return true;
    }
    return false;
}

bool isImmFloat(Value *val){
    if(isImmFloatType(val->VTy)){
        return true;
    }
    return false;
}

bool isLocalVar(Value *val){
    if(isLocalVarInt(val) || isLocalVarFloat(val)){
        return true;
    }
    return false;
}

bool isLocalVarInt(Value *val){
    if(isLocalVarIntType(val->VTy) && val->IsFromArray == false){
        return true;
    }
    return false;
}

bool isLocalVarFloat(Value *val){
    if(isLocalVarFloatType(val->VTy) && val->IsFromArray == false){
        return true;
    }
    return false;
}

bool isLocalArrayInt(Value *val){
    if(isLocalArrayIntType(val->VTy)){
        return true;
    }
    return false;
}

bool isLocalArrayFloat(Value *val){
    if(isLocalArrayFloatType(val->VTy)){
        return true;
    }
    return false;
}

bool isGlobalVarInt(Value *val){
    if(isGlobalVarIntType(val->VTy)){
        return true;
    }
    return false;
}

bool isGlobalVarFloat(Value *val){
    if(isGlobalVarFloatType(val->VTy)){
        return true;
    }
    return false;
}

bool isGlobalArrayInt(Value *val){
    if(isGlobalArrayIntType(val->VTy)){
        return true;
    }
    return false;
}

bool isGlobalArrayFloat(Value *val){
    if(isGlobalArrayFloatType(val->VTy)){
        return true;
    }
    return false;
}

bool isGlobalVar(Value *val){
    if(isGlobalVarInt(val) || isGlobalVarFloat(val)){
        return true;
    }
    return false;
}

bool isLocalArray(Value *val){
    if(isLocalArrayInt(val) || isLocalArrayFloat(val)){
        return true;
    }
    return false;
}

bool isGlobalArray(Value *val){
    if(isGlobalArrayInt(val) || isGlobalArrayFloat(val)){
        return true;
    }
    return false;
}

bool isArrayInitialize(Value *val){
    if(val->pdata->symtab_array_pdata.is_init == (unsigned int)1){
        return true;
    }else{
        return false;
    }
}