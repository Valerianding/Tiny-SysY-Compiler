////
//// Created by tom on 23-2-25.
////
#include "offset.h"
offset *new_offset_node(){
    offset *node=(offset*) malloc(sizeof(offset));
    memset(node,0,sizeof(offset));
    return node;
}
offset *offset_init(InstNode *ins){
    offset *head=NULL;
    offset *temp=NULL;
    for(;ins!=NULL&&ins->inst->Opcode!=Return;ins= get_next_inst(ins)){

        if(ins->inst->Opcode==Alloca){

            if(temp==NULL){
                temp=new_offset_node();
                head=temp;
                temp->value= user_get_operand_use(&ins->inst->user,0)->Val;

            }
            else{
                offset *temp2=new_offset_node();
                temp->next=temp2;
                temp=temp->next;
                temp->value=user_get_operand_use(&ins->inst->user,0)->Val;

            }
        }
    }

    return head;
}
void offset_head_free(offset *offset_head){

    offset *next=NULL;
    while(offset_head!=NULL){
        next=offset_head->next;
        free(offset_head);
        offset_head=next;
    }

    return;
}