#include "value.h"
#include "use.h"
#include "symtab.h"

Value *value_create(){
    Value *val = (Value*)malloc(sizeof(Value));
    memset(val,0,sizeof(Value));
    return val;
}

void value_add_use(Value *this, Use *U){
    use_add_to_list(U, &(this->use_list));
}

void value_init(Value* this){
    memset(this, 0, sizeof(Value));
    this->VTy = (Type*)malloc(sizeof(Type));
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

//
void value_replace(Value *newValue,Use *use){
    use->Val = newValue;
    use_remove_from_list(use);
    if(newValue)
        value_add_use(newValue,use);
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


bool isVar(Value *val){
    return isVarType(val->VTy);
}

bool isImm(Value *val){
    return isImmType(val->VTy);
}

bool isArray(Value *val){
    return isArrayType(val->VTy);
}

void typePrinter(Value *val){
    if(isIntType(val->VTy)){
        printf("int type");
    }else if(isFloatType(val->VTy)){
        printf("float type");
    }else if(isArrayType(val->VTy)){
        printf("array type");
    }else{
        printf("err type");
    }
}