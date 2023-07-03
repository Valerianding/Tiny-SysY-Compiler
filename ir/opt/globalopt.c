//
// Created by Valerian on 2023/7/3.
//

#include "globalopt.h"
Vector *constant_;

void CheckGlobalVariable(InstNode *list){
    //
    constant_ = VectorInit(10);

    //go through all the global variables see if it is initialized

    InstNode *globalNode = list;
    while(globalNode->inst->Opcode != FunBegin){
        // only consider variables not arrays
        // TODO add array form
        if(globalNode->inst->Opcode == GLOBAL_VAR){
            //see if it is initialed
            Value *initValue = ins_get_rhs(globalNode->inst);
            if(initValue == NULL || !isImm(initValue)){
                globalNode = get_next_inst(globalNode);
                continue;
            }


            Value *dest = ins_get_dest(globalNode->inst);
            //see if it is only been read
            Use *uses = dest->use_list;
            while(uses != NULL){
                User *user = uses->Parent;
                Instruction *ins = (Instruction *)user;
                if(ins->Opcode == Store){
                    globalNode = get_next_inst(globalNode);
                    continue;
                }
                uses = uses->Next;
            }

            //add to constant_
            VectorPushBack(constant_,dest);

            printf("%s is a global only read variable!\n",dest->name);

        }
        globalNode = get_next_inst(globalNode);
    }

}