//
// Created by Valerian on 2022/12/2.
//

#include "liveness.h"
#include "stdlib.h"
#include "string.h"
livenessnode *ll_create(Value *this){
    livenessnode *temp = (livenessnode *)malloc(sizeof(livenessnode));
    temp->this = this;
    temp->prev = NULL;
    temp->next = NULL;
    return temp;
}

void ll_add(livenessnode *head,Value *this){
   if(ll_isexsit(head,this) == -1){
       livenessnode *temp = ll_create(this);
       for(;head->next != NULL; head = head->next){}
       head->next = temp;
       temp->prev = head;
   }
}

void ll_delete(livenessnode *this,Value *v){
    for(livenessnode *temp = this; temp != NULL; temp = temp->next){
        if(temp->this == v){
            livenessnode *prev = temp->prev;
            livenessnode *next = temp->next;
            prev->next = next;
            next->prev = prev;
            free(temp);
            return;
        }
    }
}

int ll_isexsit(livenessnode *this,Value *v){
    for(livenessnode *temp = this; temp != NULL; temp = temp->next){
        if(this->this == v) return 1;
    }
    return -1;
}