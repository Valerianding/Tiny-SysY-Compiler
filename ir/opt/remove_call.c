#include "remove_call.h"
int call_self;
int call_else;
int phi_num;
int have_imm;
int issimple(Function * tempFunction)
{
    call_self=0;
    call_else=0;
    phi_num=0;
    BasicBlock *entry = tempFunction->entry;
    BasicBlock *end = tempFunction->tail;

    InstNode *currNode = entry->head_node;
    InstNode *endNode = get_next_inst(end->tail_node);
    currNode = get_next_inst(currNode);  // skip for FuncBegin
    while(currNode != endNode){
        switch (currNode->inst->Opcode)
        {
            case Call:
            {
                if(strcmp(currNode->inst->user.use_list->Val->name,tempFunction->name)==0)
                {
                    call_self=1;
                }
                else
                {
                    call_else=1;
                }
                break;

            }
            case Phi:
            {
                int i=0;
                have_imm=0;
                HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                HashSetFirst(phiSet);
                unsigned int size=HashSetSize(phiSet);
                phi_num=size;
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    BasicBlock *from = phiInfo->from;
                    Value *incomingVal = phiInfo->define;
                    if(i + 1 == size)      //最后一次
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                            have_imm++;
                        }else if(incomingVal != NULL){
                            
                        }else{
                            
                        }
                    }
                    else
                    {
                        if(incomingVal != NULL && isImm(incomingVal)){
                           have_imm++;
                        }else if(incomingVal != NULL){
                           
                        }else{
                            
                        }
                    }
                    i++;
                }
                break;
            }
        }
        currNode = get_next_inst(currNode);
    }
    if(call_self==1&&call_else==0&&phi_num==2&&have_imm==1)
    {
        tempFunction->issimplerecursive=1;
        return 1;
    }
    else
    {
        tempFunction->issimplerecursive=0;
        return 0;
    }
    // printf("func:%s is simple :%d\n",tempFunction->name,tempFunction->issimplerecursive);
    return 0;
}

// void remake_func(Function * tempFunction)
// {

// }