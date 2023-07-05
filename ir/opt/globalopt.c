//
// Created by Valerian on 2023/7/3.
//

#include "globalopt.h"
Vector *constant_;

void CheckGlobalVariable(InstNode *list){

    //first of all we go through all the node to see if the initValue actually is uncritical for all
    InstNode *node = list;
    while(node != NULL){
        assert(node->inst->isCritical == false);
        node = get_next_inst(node);
    }

    constant_ = VectorInit(10);

    //go through all the global variables see if it is initialized

    InstNode *globalNode = list;
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

            //it means that these are all loads
            ins->isCritical = true;


            //change the dest value type and it's pdata
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


    //OK now we can delete all the isCritical == True

}