#include "value.h"
#include "use.h"
#include "symtab.h"

/// This method should only be used by the Use class.
// void addUse(Use &U) { U.addToList(&UseList); }
void value_add_use(Value *this, Use *U)
{
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

/* 可能需要修改 */
void value_init_int(Value *this,int num){
    value_init(this);
    this->pdata->var_pdata.iVal = num;
}

void value_init_float(Value *this,float num){
    value_init(this);
    this->pdata->var_pdata.fVal = num;
}

//FIXME: test purpose only!
extern Symtab test_symtab;
Symtab* get_sym_tab(Value *V) {
   Symtab *ST = &test_symtab;
//    if (Instruction *I = dyn_cast<Instruction>(V)) {
//      if (BasicBlock *P = I->getParent())
//        if (Function *PP = P->getParent())
//          ST = PP->getValueSymbolTable();
//    } else if (BasicBlock *BB = dyn_cast<BasicBlock>(V)) {
//      if (Function *P = BB->getParent())
//        ST = P->getValueSymbolTable();
//    } else if (GlobalValue *GV = dyn_cast<GlobalValue>(V)) {
//      if (Module *P = GV->getParent())
//        ST = &P->getValueSymbolTable();
//    } else if (Argument *A = dyn_cast<Argument>(V)) {
//      if (Function *P = A->getParent())
//        ST = P->getValueSymbolTable();
//    } else {
//      assert(isa<Constant>(V) && "Unknown value type!");
//      return true;  // no name is setable for this.
//    }
   return ST;
}

Value *create_value(Value** v){
    (*v) = (Value*)malloc(sizeof(Value));
    value_init(*v);
}