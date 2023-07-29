//
// Created by Valerian on 2023/7/3.
//

#include "globalopt.h"
Vector *constant_;

//we only considered -> if it is only been read
//more case: only stored but never used
//TODO for array & global variable
void CheckGlobalVariable(InstNode *list){

    //first of all we go through all the node to see if the initValue actually is uncritical for all
    InstNode *node = list;
    while(node != NULL){
        assert(node->inst->isCritical == false);
        node = get_next_inst(node);
    }

    //go through all first delete not used global variable
    InstNode *globalNode = get_next_inst(list);
    while(globalNode->inst->Opcode != FunBegin){
        assert(globalNode->inst->Opcode == GLOBAL_VAR || globalNode->inst->Opcode == ALLBEGIN);
        Value *dest = ins_get_dest(globalNode->inst);
        printf("dest is %s\n",dest->name);
        Use *destUses = dest->use_list;
        if(destUses == NULL){

            InstNode *tempNode = get_next_inst(globalNode);
            deleteIns(globalNode);
            globalNode = tempNode;
        }else{
            globalNode = get_next_inst(globalNode);
        }
    }

    constant_ = VectorInit(10);

    //go through all the global variables see if it is initialized

    globalNode = list;
    while(globalNode->inst->Opcode != FunBegin){
        // only consider variables not arrays
        // TODO add array form
        if(globalNode->inst->Opcode == GLOBAL_VAR){
            //see if it is initialed

            //TODO undefined global variables are initialized to zero
            Value *initValue = ins_get_rhs(globalNode->inst);
            if(initValue == NULL || !isImm(initValue)){
                globalNode = get_next_inst(globalNode);
                continue;
            }
            bool flag = true;
            Value *dest = ins_get_dest(globalNode->inst);

            //see if it is only been read
            Use *uses = dest->use_list;
            while(uses != NULL){
                User *user = uses->Parent;
                Instruction *ins = (Instruction *)user;
                if(ins->Opcode == Store){
                    flag = false;
                    break;
                }
                uses = uses->Next;
            }
            //add to constant_
            if(flag) {
                VectorPushBack(constant_, dest);
                printf("%s is a global only read variable initValule is %d!\n", dest->name,initValue->pdata->var_pdata.iVal);
            }
        }
        globalNode = get_next_inst(globalNode);
    }


    Value *val;
    int size = VectorSize(constant_);
    for(int i = 0; i < size; i++){
        VectorGet(constant_,i,&val);
        if(val != NULL){
            printf("%s is stored into Vector!\n",val->name);
        }

        //转换成InstNode
        Instruction *ins = (Instruction *)val;
        BasicBlock *globalBlock = ins->Parent;
        assert(globalBlock != NULL);
        InstNode *uselessNode = findNode(globalBlock,ins);

        //include attach prev next
        deleteIns(uselessNode);
        //replace load with it's initValue
        //%1 = load @a
        //...  = %1 + c
        Use *uses = val->use_list;
        while(uses != NULL){
            User *user = uses->Parent;
            //mark the instruction to be useless
            //TODO remember thar true is useless
            Instruction *curIns = (Instruction *)user;

            curIns->isCritical = true;

            //change the dest value type and it's pdata of load
            Value *insDest = ins_get_dest(curIns);

            //init Value for this global variable

            Instruction *originalGlobal = (Instruction *)val;
            Value *initValue = ins_get_rhs(originalGlobal);
            switch (val->VTy->ID) {
                case GlobalVarInt:{
                    printf("case int initValue : %d\n",initValue->pdata->var_pdata.iVal);

                    insDest->VTy->ID = Int;
                    insDest->pdata->var_pdata.iVal = initValue->pdata->var_pdata.iVal;
                    break;
                }
                case GlobalVarFloat:{
                    printf("case float initValue : %lf\n",initValue->pdata->var_pdata.fVal);

                    insDest->VTy->ID = Float;
                    insDest->pdata->var_pdata.fVal = initValue->pdata->var_pdata.fVal;
                    break;
                }
                default:{
                    assert(false);
                }
            }
            uses = uses->Next;
        }
    }

    InstNode *tempNode = instruction_list;
    printf("here\n");
    while(tempNode != NULL){
        if(tempNode->inst->isCritical == true){
            InstNode *nextNode = get_next_inst(tempNode);
            deleteIns(tempNode);
            tempNode = nextNode;
        }else{
            tempNode = get_next_inst(tempNode);
        }
    }


    //global array only store
    //%1 = gep @arr
    //... = gep % 1
    // ..
    //store or load
    globalNode = list;
    while(globalNode->inst->Opcode != FunBegin){
        printf("here! %d\n",globalNode->inst->i);
        Value *dest = ins_get_dest(globalNode->inst);
        if(globalNode->inst->Opcode == GLOBAL_VAR && isGlobalArray(dest)){
            bool OnlyStore = true;
            Value *dest = ins_get_dest(globalNode->inst);

            //see if it is only stored
            Use *uses = dest->use_list;
            while(uses != NULL){
                User *user = uses->Parent;
                Instruction *ins = (Instruction *)user;
                if(ins->Opcode == GEP){
                    HashSet *workList = HashSetInit();
                    Value *insDest = ins_get_dest(ins);
                    HashSetAdd(workList,insDest);
                    while(HashSetSize(workList) != 0){
                        HashSetFirst(workList);
                        Value *gep = HashSetNext(workList);
                        HashSetRemove(workList,gep);
                        Use *gepUses = gep->use_list;
                        while(gepUses != NULL){
                            Instruction *userIns = (Instruction *)gepUses->Parent;
                            if(userIns->Opcode == Load){
                                OnlyStore = false;
                                break;
                            }else if(userIns->Opcode == GEP){
                                Value *userDest = ins_get_dest(userIns);
                                HashSetAdd(workList,userDest);
                            }else if(userIns->Opcode == GIVE_PARAM){
                                OnlyStore = false;
                                break;
                            }
                            gepUses = gepUses->Next;
                        }
                        if(OnlyStore == false){
                            break;
                        }
                    }
                    HashSetDeinit(workList);
                }
                if(OnlyStore == false){
                    break;
                }
                uses = uses->Next;
            }

            if(OnlyStore){
                uses = dest->use_list;
                assert(uses != NULL);
                //所有对于这个global array的gep和gep产生的及gep产生的所有instruction都会被remove
                while(uses != NULL){
                    Instruction *userIns = (Instruction *)uses->Parent;
                    InstNode *instNode = findNode(userIns->Parent, userIns);
                    HashSet *workList = HashSetInit();
                    Value *userDest = ins_get_dest(userIns);
                    deleteIns(instNode);
                    HashSetAdd(workList,userDest);
                    while(HashSetSize(workList) != 0){
                        HashSetFirst(workList);
                        Value *other = HashSetNext(workList);
                        HashSetRemove(workList,other);
                        Use *otherUses = other->use_list;
                        while(otherUses != NULL){
                            Instruction *ins = (Instruction *)otherUses->Parent;
                            InstNode *node = findNode(ins->Parent,ins);
                            Value *dest = ins_get_dest(ins);
                            HashSetAdd(workList,dest);
                            deleteIns(node);
                            otherUses = otherUses->Next;
                        }
                    }
                    uses = uses->Next;
                }

                InstNode *tempNext= get_next_inst(globalNode);
                printf("tempNext %d\n",tempNext->inst->i);
                deleteIns(globalNode);
                globalNode = tempNext;
            }else{
                globalNode = get_next_inst(globalNode);
                printf("globalNext %d\n",globalNode->inst->i);
            }
        }else{
            globalNode = get_next_inst(globalNode);
        }
    }
}

void replaceGlobal(Function *currentFunction,Value *globalVar){
    BasicBlock *entry = currentFunction->entry;
    InstNode *entryHead = entry->head_node;
    Instruction *newAlloca = ins_new_zero_operator(Alloca);
    newAlloca->Parent = entry;
    InstNode *newAllocaNode = new_inst_node(newAlloca);
    //give it a name so rename phrase can actually knows
    newAlloca->user.value.name = (char *)malloc(sizeof(char) * 10);
    strcpy(newAlloca->user.value.name,"%alloca");
    ins_insert_after(newAllocaNode,entryHead);

    //对全局变量进行初始化
    Instruction *globalIns = (Instruction *)globalVar;
    Value *initValue = ins_get_rhs(globalIns);
    assert(isImm(initValue));
    Value *dest = ins_get_dest(newAlloca);
    //insert a store instruction
    //alloca
    InstNode *tempNode = get_next_inst(entryHead);
    while(tempNode->inst->Opcode == Alloca){
        tempNode = get_next_inst(tempNode);
    }

    Instruction *newStore = ins_new_binary_operator(Store,initValue,dest);
    newStore->Parent = entry;
    InstNode *newStoreNode = new_inst_node(newStore);
    ins_insert_before(newStoreNode,tempNode);

    //对于这个alloca设置Type

    switch (globalVar->VTy->ID) {
        case GlobalVarFloat:{
            dest->VTy->ID = Var_FLOAT;
            break;
        }
        case GlobalVarInt:{
            dest->VTy->ID = Var_INT;
            break;
        }
        default:
            assert(false);
    }
    valueReplaceAll(globalVar,dest,currentFunction);
}

//This pass will performs global 2 local
void global2local(InstNode *list){
    InstNode *globalNode = get_next_inst(list);
    while(globalNode->inst->Opcode != FunBegin){
        assert(globalNode->inst->Opcode == GLOBAL_VAR || globalNode->inst->Opcode == ALLBEGIN);
        Value *dest = ins_get_dest(globalNode->inst);
        printf("dest is %s\n",dest->name);
        Use *destUses = dest->use_list;
        if(destUses == NULL){
            InstNode *tempNode = get_next_inst(globalNode);
            deleteIns(globalNode);
            globalNode = tempNode;
        }else{
            globalNode = get_next_inst(globalNode);
        }
    }

    globalNode = list;
    while(globalNode->inst->Opcode != FunBegin){
        if(globalNode->inst->Opcode == GLOBAL_VAR){
            Value *dest = ins_get_dest(globalNode->inst);
            if(isGlobalVar(dest)){
                //check if it is only used in one function
                Use *uses = dest->use_list;
                bool OnlyUsedInOneFunction = true;
                Function *prevFunction = NULL;
                while(uses != NULL){
                    Instruction *useIns = (Instruction *)uses->Parent;
                    BasicBlock *block = useIns->Parent;
                    Function *current = block->Parent;
                    if(prevFunction != NULL && prevFunction != current){
                        OnlyUsedInOneFunction = false;
                        break;
                    }
                    prevFunction = current;
                    uses = uses->Next;
                }

                if(OnlyUsedInOneFunction){
                    //let it become the local variable in this function

                    //insert a alloca instruction
                    //replace all the global var with this alloca
                    printf("var %s is only used!\n",dest->name);
                    //TODO because there is no used global variable
                    if(prevFunction != NULL)
                        replaceGlobal(prevFunction,dest);
                }
            }
        }
        globalNode = get_next_inst(globalNode);
    }
}