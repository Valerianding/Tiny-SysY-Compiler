//
// Created by Valerian on 2023/1/18.
//
#include "dominance.h"

//只针对BasicBlock类型
void HashSetCopy(HashSet *dest,HashSet *src){
    HashSetFirst(src);
    for(BasicBlock *key = HashSetNext(src); key != NULL; key = HashSetNext(src)){
        if(!HashSetFind(dest,key)){
            HashSetAdd(dest,key);
        }
    }
}

bool HashSetDifferent(HashSet *lhs,HashSet *rhs){
    //左边是小的 右边是大的
    unsigned int leftSize = HashSetSize(lhs);
    unsigned int rightSize = HashSetSize(rhs);
    if(leftSize != rightSize) return true;
    HashSetFirst(lhs);
    for(BasicBlock *key = HashSetNext(lhs); key != NULL; key = HashSetNext(lhs)){
        if(!HashSetFind(rhs,key)) return true;
    }
    return false;
}

DomTreeNode *createDomTreeNode(BasicBlock *block,BasicBlock *parent){
    DomTreeNode *domTreeNode = (DomTreeNode*)malloc(sizeof(DomTreeNode));
    memset(domTreeNode,0,sizeof(DomTreeNode));
    domTreeNode->children = HashSetInit();
    domTreeNode->block = block;
    domTreeNode->parent = parent;
    block->domTreeNode = domTreeNode;
    return domTreeNode;
}


void calculate_dominance(Function *currentFunction) {
    // 注意要删除不可达的BasicBlock
    BasicBlock *entry = currentFunction->entry;
    entry->dom = HashSetInit();
    HashSetAdd(entry->dom,entry);

    HashSet *allNode = HashSetInit();
    //拿到第一条InstNode
    BasicBlock *prev = nullptr;
    InstNode *cur = entry->head_node;
    InstNode *end = currentFunction->tail->tail_node;
    while(cur != end){
        BasicBlock *parent = cur->inst->Parent;
        if(prev != parent){
            HashSetAdd(allNode,parent);
        }
        cur = get_next_inst(cur);
    }
    BasicBlock *endBlock = end->inst->Parent;
    if(!HashSetFind(allNode,endBlock)){
        HashSetAdd(allNode,endBlock);
    }
    HashSetFirst(allNode);
    cur = entry->head_node;
    while(cur != end){
        BasicBlock *block = cur->inst->Parent;
        if(block != entry && block->dom == NULL){
            block->dom = HashSetInit();
        }
        if(!block->visited && block != entry){
            HashSetFirst(allNode);
            printf("addBlock :");
            for(BasicBlock *addBlock = HashSetNext(allNode); addBlock != NULL; addBlock = HashSetNext(allNode)){
                printf(" b%d",addBlock->id);
                HashSetAdd(block->dom,addBlock);
            }
            printf("\n");
            block->visited = true;
        }
        cur = get_next_inst(cur);
    }

    printf("right before all\n");

    //全集计算完毕
    bool changed = true;
    while(changed){
        changed = false;
        //对于除了entry外的每一个block
        cur = currentFunction->entry->head_node;


        printf("iterate begin!\n");

        clear_visited_flag(cur->inst->Parent);

        printf("cleared all visited flag!\n");
        while(cur != end){
            BasicBlock *parent = cur->inst->Parent;
            //除了头节点

            if(parent->visited != true && parent != entry){
                //
                printf("current at : b%d",parent->id);
                parent->visited = true;

                //新建一个hashset
                HashSet *newSet = HashSetInit();

                //让它为全集
                HashSetCopy(newSet,allNode);

                //开始迭代
                //找到前驱节点
                HashSet *prevBlocks = get_prev_block(parent);

                HashSetFirst(prevBlocks);

                //对于所有前驱节点
                printf(" prevBlocks:");
                for(BasicBlock *prevBlock = HashSetNext(prevBlocks); prevBlock != NULL; prevBlock = HashSetNext(prevBlocks)){
                    printf(" b%d",prevBlock->id);
                    HashSet *prevDom = prevBlock->dom;
                    newSet = HashSetIntersect(newSet,prevDom);
                }
                printf(" ");

                HashSetFirst(newSet);

                //并自己
                HashSetAdd(newSet,parent);

                printf("newSet contain:");
                for(BasicBlock *key = HashSetNext(newSet);key != NULL; key = HashSetNext(newSet)){
                    printf(" b%d",key->id);
                }

                //判断跟现在的集合是否有差别
                changed = HashSetDifferent(newSet,parent->dom);

                if(changed){
                    HashSetDeinit(parent->dom);
                    parent->dom = newSet;
                }else{
                    HashSetDeinit(newSet);
                }
                printf(" changed : %d\n",changed);
            }
            cur = get_next_inst(cur);
        }
        printf("This round changed : %d\n",changed);
    }
    printf("after dominance!\n");
    HashSetDeinit(allNode);
}

void calculate_dominance_frontier(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    if(entry == end){
        entry->df = HashSetInit();
        return;
    }
    HashSet *allBlocks = HashSetInit();
    //
    clear_visited_flag(entry);

    //
    InstNode *currNode = entry->head_node;
    while(currNode != get_next_inst(end->tail_node)){
        BasicBlock *block = currNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            if(!HashSetFind(allBlocks,block)){
                HashSetAdd(allBlocks,block);
            }
        }
        currNode = get_next_inst(currNode);
    }

    //还需要一个tempSet
    HashSet *tempSet = HashSetInit();
    HashSetFirst(allBlocks);
    for(BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)){
        HashSetAdd(tempSet,block);
    }

    //
    HashSetFirst(allBlocks);
    for(BasicBlock *X = HashSetNext(allBlocks); X != NULL; X = HashSetNext(allBlocks)){
        X->df = HashSetInit();
        HashSetFirst(tempSet);
        for(BasicBlock *Y = HashSetNext(tempSet); Y != NULL; Y = HashSetNext(tempSet)){
            if(HashSetFind(Y->dom,X) && Y->true_block != NULL && !HashSetFind(Y->true_block->dom,X)){
                HashSetAdd(X->df,Y->true_block);
            }
            if(HashSetFind(Y->dom,X) && Y->false_block != NULL && !HashSetFind(Y->false_block->dom,X)){
                HashSetAdd(X->df,Y->false_block);
            }
        }
    }

    HashSetFirst(allBlocks);
    for(BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)){
        HashSetFirst(block->df);
        printf("b%d df : ",block->id);
        for(BasicBlock *df = HashSetNext(block->df); df != NULL; df = HashSetNext(block->df)){
            printf("b%d ",df->id);
        }
        printf("\n");
    }
    HashSetDeinit(allBlocks);
    HashSetDeinit(tempSet);
    clear_visited_flag(entry);
}

void calculate_iDominator(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    //不动的
    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    clear_visited_flag(head->inst->Parent);
    //从头来计算
    InstNode *curNode = head;
    while(curNode != get_next_inst(tail)){
        BasicBlock *curBlock = curNode->inst->Parent;

        if(curBlock->visited) {
            curNode = get_next_inst(curNode);
        }else{
            curBlock->visited = true;
            HashSet *tempSet = HashSetInit();
            HashSetCopy(tempSet,curBlock->dom);
            //去掉本身
            HashSetRemove(tempSet,curBlock);
            HashSetFirst(curBlock->dom);
            for(BasicBlock *key = HashSetNext(curBlock->dom);key != NULL; key = HashSetNext(curBlock->dom)){
                //除去现在这个
                if(key == curBlock) continue;

                HashSet *keySet = key->dom;
                HashSetFirst(tempSet);
                for(BasicBlock *key1 = HashSetNext(tempSet); key1 != NULL; key1 = HashSetNext(tempSet)){
                    if(key1 != key && HashSetFind(keySet,key1)){
                        HashSetRemove(tempSet,key1);
                    }
                }
            }
            HashSetFirst(tempSet);
            curBlock->iDom = NULL;
            for(BasicBlock *key = HashSetNext(tempSet); key != NULL; key = HashSetNext(tempSet)){
                printf("b%d, idom: b%d\n",curBlock->id,key->id);
                curBlock->iDom = key;
            }

            HashSetDeinit(tempSet);
            curNode = get_next_inst(curNode);
        }
    }
    clear_visited_flag(entry);
}

void DomTreeAddChild(DomTreeNode *parent, DomTreeNode *child){
    HashSetAdd(parent->children, child);
}

void calculate_DomTree(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    printf("entryBlock : %d",entry->id);
    printf(" endBlock : %d",end->id);
    printf("\n");
    //不动的
    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    // 先对每一个BasicBlock进行建立DomNode
    clear_visited_flag(entry);

    InstNode *curNode = head;
    while(curNode != get_next_inst(tail)){
        BasicBlock *block = curNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            assert(block->domTreeNode == NULL);
            block->domTreeNode = (DomTreeNode*)malloc(sizeof(DomTreeNode));
            memset(block->domTreeNode,0,sizeof(DomTreeNode));
            block->domTreeNode->block = block;
            block->domTreeNode->children = HashSetInit();
        }
        curNode = get_next_inst(curNode);
    }

    //从头来计算
    clear_visited_flag(head->inst->Parent);
    curNode = head;
    while(curNode != get_next_inst(tail)){
        BasicBlock *block = curNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            BasicBlock *iDom = block->iDom;
            DomTreeNode *domTreeNode = block->domTreeNode;
            domTreeNode->parent = iDom;
            if(iDom != NULL){
                DomTreeNode *prevNode = iDom->domTreeNode;
                assert(prevNode != NULL);
                DomTreeAddChild(prevNode, domTreeNode);
                //printf("b%d add child b%d\n",prevNode->block->id,block->id);
                assert(HashSetFind(prevNode->children,domTreeNode));
            }
        }
        curNode = get_next_inst(curNode);
    }


    // 检查DomTree的构建是否成功
    InstNode *checkNode = head;

    //Function的跟节点保留
    currentFunction->root = entry->domTreeNode;
    assert(entry->domTreeNode != nullptr);

    DomTreePrinter(entry->domTreeNode);
    clear_visited_flag(checkNode->inst->Parent);
    while(checkNode != get_next_inst(tail)){
        BasicBlock *parent = checkNode->inst->Parent;
        if(parent->visited == false){
            parent->visited = true;
            DomTreeNode *domTreeNode = parent->domTreeNode;
            printf("b%d ",parent->id);
            if(parent->iDom != nullptr)
                printf("idom : b%d ",parent->iDom->id);
            HashSet *childSet = domTreeNode->children;
            HashSetFirst(childSet);
            for(DomTreeNode *childNode = HashSetNext(childSet); childNode != NULL; childNode = HashSetNext(childSet)){
                printf("b%d ",childNode->block->id);
            }
            printf("\n");
        }
        checkNode = get_next_inst(checkNode);
    }
}

void DomTreePrinter(DomTreeNode *root){
    if(HashSetSize(root->children) == 0){
        return;
    }
    HashSetFirst(root->children);
    for(DomTreeNode *key = HashSetNext(root->children); key != nullptr; key = HashSetNext(root->children)){
        BasicBlock *block = key->block;
        printf("block:%d",block->id);
        DomTreePrinter(key);
    }
    return;
}


void constructReverseCfg(Function *currentFunction){
    // 构建逆序图 方便我们计算RDF和

    // 同时我们希望能在原本的BasicBlock里面保留这些计算的结果
}