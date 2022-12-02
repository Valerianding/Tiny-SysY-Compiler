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
    this->VTy->ID = ConstIntegerTyID;
    this->pdata = (int*)malloc(sizeof(int));
    int* temp = (int*)this->pdata;
    *(temp) = num;
}

void value_init_float(Value *this,float num){
    value_init(this);
    this->VTy->ID = ConstFloatTyID;
    this->pdata = (float*)malloc(sizeof(float));
    float* temp = (float*)this->pdata;
    *(temp) = num;
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
//
//ConstantNum* const_new_int(int num){
//    ConstantNum *c = malloc(sizeof(ConstantNum));
//    c->num.num_int = num;
//    value_init(&(c->value));
//    return c;
//}
//
//ConstantNum* const_new_float(float num){
//    ConstantNum *c = malloc(sizeof(ConstantNum));
//    c->num.num_float = num;
//    value_init(&(c->value));
//    return c;
//}

//void const_init_int(ConstantNum* c, int num){
//    c->num.num_int = num;
//    value_init(&(c->value));
//}
//
//void const_init_float(ConstantNum* c, float num){
//    c->num.num_float = num;
//    value_init(&(c->value));
//}

// enum ReplaceMetadataUses { No, Yes };
// void doRAUW(Value* this, Value *New, enum ReplaceMetadataUses ReplaceMetaUses)
// {
//     assert(New && "Value::replaceAllUsesWith(<null>) is invalid!");
//     assert(!contains(New, this) &&
//            "this->replaceAllUsesWith(expr(this)) is NOT valid!");
//     assert(getType(New) == getType(this) &&
//            "replaceAllUses of value with new value of different type!");

//     // Notify all ValueHandles (if present) that this value is going away.
//     if (this->HasValueHandle)
//         ValueHandleBase::ValueIsRAUWd(this, New);
//     if (ReplaceMetaUses == ReplaceMetadataUses::Yes && isUsedByMetadata())
//         ValueAsMetadata::handleRAUW(this, New);

//     while (!materialized_use_empty())
//     {
//         Use &U = *UseList;
//         // Must handle Constants specially, we cannot call replaceUsesOfWith on a
//         // constant because they are uniqued.
//         if (auto *C = dyn_cast<Constant>(U.getUser()))
//         {
//             if (!isa<GlobalValue>(C))
//             {
//                 C->handleOperandChange(this, New);
//                 continue;
//             }
//         }

//         U.set(New);
//     }

//     if (BasicBlock *BB = dyn_cast<BasicBlock>(this))
//         BB->replaceSuccessorsPhiUsesWith(cast<BasicBlock>(New));
// }

Value *create_value(Value** v){
    (*v) = (Value*)malloc(sizeof(Value));
    value_init(*v);
}