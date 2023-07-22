//
// Created by Valerian on 2023/7/22.
//

#include "gcm.h"
const Opcode pinnedOperation[] = {Phi,br,br_i1, Call, Return,GIVE_PARAM};

void bfsTravelDomTree(DomTreeNode *root,int level){
    root->depth = level;
    HashSetFirst(root->children);
    for(DomTreeNode *child = HashSetNext(root->children); child != NULL; child = HashSetNext(root->children)){
        bfsTravelDomTree(child,level + 1);
    }
}

void markDominanceDepth(Function *function){
    //bfs 遍历支配树
    bfsTravelDomTree(function->root,0);
}

