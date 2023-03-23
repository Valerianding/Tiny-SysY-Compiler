//
// Created by Valerian on 2023/1/18.
//
#include "dominance.h"
#include "utility.h"
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
    //currentFunction->dominance = HashMapInit();


    // 注意要删除不可达的BasicBlock
    BasicBlock *entry = currentFunction->entry;
    entry->dom = HashSetInit();
    HashSetAdd(entry->dom,entry);
    //HashMapPut(currentFunction->dominance, entry, entry->dom);

    HashSet *allNode = HashSetInit();
    //拿到第一条InstNode
    BasicBlock *prev = nullptr;
    InstNode *cur = entry->head_node;
    InstNode *end = currentFunction->tail->tail_node;


    printf("cur : %d\n",cur->inst->i);
    printf("end : %d\n",end->inst->i);
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
    //对于每一个节点
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *end = currentFunction->tail;

    printf("entryBlock : %d",entry->id);
    printf(" endBlock : %d",end->id);
    printf("\n");
    //不动的
    InstNode *head = entry->head_node;
    InstNode *tail = end->tail_node;

    clear_visited_flag(head->inst->Parent);
    //从头来计算
    InstNode *curNode = head;
    while(curNode != get_next_inst(tail)){
        BasicBlock *cur = curNode->inst->Parent;

        //初始化现在的df
        cur->df = HashSetInit();

        InstNode *temp = head;
        //找到X支配的节点Y
        printf("b%d domBlock:",cur->id);
        while(temp != get_next_inst(tail)){
            BasicBlock *tempBlock = temp->inst->Parent;
            if(HashSetFind(tempBlock->dom,cur) ){
                printf(" %d",tempBlock->id);
                //如果支配它的话找temp的后继 并且不支配的话
                if(tempBlock->true_block && !HashSetFind(tempBlock->true_block->dom,cur) && !HashSetFind(cur->df,tempBlock->true_block)){
                    HashSetAdd(cur->df,tempBlock->true_block);
                }
                if(tempBlock->false_block && !HashSetFind(tempBlock->false_block->dom,cur) && !HashSetFind(cur->df,tempBlock->false_block)){
                    HashSetAdd(cur->df,tempBlock->false_block);
                }
            }
            temp = get_next_inst(temp);
        }
        printf("df:");
        HashSetFirst(cur->df);
        for(BasicBlock *key = HashSetNext(cur->df); key != NULL; key = HashSetNext(cur->df)){
            printf(" b%d",key->id);
        }
        printf("\n");
        curNode = get_next_inst(curNode);
    }
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

    clear_visited_flag(head->inst->Parent);
    //从头来计算
    InstNode *curNode = head;
    while(curNode != get_next_inst(tail)){
        BasicBlock *block = curNode->inst->Parent;
        if(block->visited == false){
            block->visited = true;
            BasicBlock *iDom = block->iDom;
            DomTreeNode *domTreeNode = createDomTreeNode(block,iDom);
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
            printf("b%d, DomNode : ",parent->id);
            if(parent->iDom != nullptr)
                printf("idom : b%d ",parent->iDom->id);
            HashSet *childSet = domTreeNode->children;
            HashSetFirst(childSet);
            for(DomTreeNode *childNode = HashSetNext(childSet); childNode != NULL; childNode = HashSetNext(childSet)){
                //printf("b%d ",childNode->block->id);
            }
            //printf("\n");
        }
        checkNode = get_next_inst(checkNode);
    }
}

void DomTreePrinter(DomTreeNode *root){
    if(HashSetSize(root->children) == 0){
        printf("opps\n");
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