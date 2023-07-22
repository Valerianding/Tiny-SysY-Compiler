//
// Created by Valerian on 2023/1/18.
#include "dominance.h"

//只针对BasicBlock类型

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


    clear_visited_flag(entry);
    HashSet *allNode = HashSetInit();
    HashSet *workList = HashSetInit();
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        //
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetAdd(allNode,block);
        block->visited = true;
        HashSetRemove(workList,block);
        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }
        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }

    HashSetDeinit(workList);

    HashSet *tempSet = HashSetInit();
    HashSetCopy(tempSet,allNode);
    HashSetFirst(allNode);
    for(BasicBlock *tempBlock = HashSetNext(allNode); tempBlock != NULL; tempBlock = HashSetNext(allNode)){
        if(tempBlock != entry){
            tempBlock->dom = HashSetInit();
            HashSetCopy(tempBlock->dom,tempSet);
        }
    }
    HashSetDeinit(tempSet);

    printf("right before all\n");

    //全集计算完毕
    bool changed = true;
    while(changed){
        changed = false;
        //对于除了entry外的每一个block
        InstNode *cur = currentFunction->entry->head_node;
        InstNode *end = currentFunction->tail->tail_node;

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
                HashSet *tempAllSet = HashSetInit();

                //让它为全集
                HashSetCopy(tempAllSet,allNode);

                //开始迭代
                //找到前驱节点
                HashSet *prevBlocks = get_prev_block(parent);

                HashSetFirst(prevBlocks);

                HashSet *newSet = NULL;
                //对于所有前驱节点
                printf(" prevBlocks:");
                for(BasicBlock *prevBlock = HashSetNext(prevBlocks); prevBlock != NULL; prevBlock = HashSetNext(prevBlocks)){
                    printf(" b%d",prevBlock->id);
                    HashSet *prevDom = prevBlock->dom;
                    newSet = HashSetIntersect(tempAllSet,prevDom);
                    assert(newSet != NULL);
                    HashSetDeinit(tempAllSet);
                    tempAllSet = newSet;
                }

                printf(" ");

                HashSetFirst(tempAllSet);

                //并自己
                HashSetAdd(newSet,parent);

                printf("newSet contain:");
                for(BasicBlock *key = HashSetNext(tempAllSet);key != NULL; key = HashSetNext(tempAllSet)){
                    printf(" b%d",key->id);
                }

                //判断跟现在的集合是否有差别
                changed = HashSetDifferent(tempAllSet,parent->dom);

                if(changed){
                    HashSetDeinit(parent->dom);
                    parent->dom = tempAllSet;
                }else{
                    HashSetDeinit(tempAllSet);
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
            if(HashSetFind(Y->dom,X) && Y->true_block != NULL){
                if(!HashSetFind(Y->true_block->dom,X) || Y->true_block == X){
                    HashSetAdd(X->df,Y->true_block);
                }
            }
            if(HashSetFind(Y->dom,X) && Y->false_block != NULL){
                if(!HashSetFind(Y->false_block->dom,X) || Y->false_block == X){
                    HashSetAdd(X->df,Y->false_block);
                }
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
    //printf("in DomTree!\n");
    //printf("entryBlock : %d",entry->id);
    //printf(" endBlock : %d",end->id);
    //printf("\n");
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
            block->domTreeNode->depth = 0;
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
            //printf("b%d ",parent->id);
            if(parent->iDom != nullptr){
                //printf("idom : b%d ",parent->iDom->id);
            }
            HashSet *childSet = domTreeNode->children;
            HashSetFirst(childSet);
            //printf("child: ");
            for(DomTreeNode *childNode = HashSetNext(childSet); childNode != NULL; childNode = HashSetNext(childSet)){
                //printf("b%d ",childNode->block->id);
            }
           // printf("\n");
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
        //printf("block:%d",block->id);
        DomTreePrinter(key);
    }
    return;
}

PostDomNode *createPostDomNode(BasicBlock *block, BasicBlock *ipDom){
    PostDomNode *postDomNode = (PostDomNode*) malloc(sizeof(PostDomNode));
    postDomNode->block = block;
    postDomNode->parent = ipDom;
    postDomNode->children = HashSetInit();
    postDomNode->flag = false;
    return postDomNode;
}

void calculatePostDominance(Function *currentFunction) {
    //还是先记录全集
    BasicBlock *entry = currentFunction->entry;

    clear_visited_flag(entry);

    BasicBlock *tail = currentFunction->tail;
    InstNode *exitNode = tail->tail_node;
    while (exitNode->inst->Opcode != Return) {
        exitNode = get_prev_inst(exitNode);
    }

    // tail 不一定是exit 需要找到exit
    BasicBlock *exit = exitNode->inst->Parent;

    HashSet *allBlocks = HashSetInit();
    HashSet *workList = HashSetInit();
    HashSetAdd(workList, entry);

    //bfs 一下
    while (HashSetSize(workList) != 0) {
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList, block);
        HashSetAdd(allBlocks, block);
        //当前基本块一定是没有遍历过的
        block->visited = true;

        //把内存开了
        block->pDom = HashSetInit();
        block->rdf = HashSetInit();

        if (block->true_block && block->true_block->visited == false) {
            HashSetAdd(workList, block->true_block);
        }
        if (block->false_block && block->false_block->visited == false) {
            HashSetAdd(workList, block->false_block);
        }
    }
    HashSetDeinit(workList);

    HashSetAdd(exit->pDom, exit);
    HashSet *tempSet = HashSetInit();
    HashSetCopy(tempSet, allBlocks);
    HashSetFirst(allBlocks);
    for (BasicBlock *tempBlock = HashSetNext(allBlocks); tempBlock != NULL; tempBlock = HashSetNext(allBlocks)) {
        //除了exit
        if (tempBlock != exit) {
            HashSetCopy(tempBlock->pDom, tempSet);
        }
    }


    bool changed = true;
    while (changed) {
        changed = false;
        HashSetFirst(allBlocks);
        for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)) {
            //
            if (block != exit) {
                HashSet *tempALlSet = HashSetInit();
                HashSetCopy(tempALlSet, tempSet);

                HashSet *newSet = NULL;
                //for each successor block p
                if (block->true_block) {
                    newSet = HashSetIntersect(tempALlSet, block->true_block->pDom);
                    assert(newSet != NULL);
                    HashSetDeinit(tempALlSet);
                    tempALlSet = newSet;
                }

                if (block->false_block) {
                    newSet = HashSetIntersect(tempALlSet, block->false_block->pDom);
                    assert(newSet != NULL);
                    HashSetDeinit(tempALlSet);
                    tempALlSet = newSet;
                }

                HashSetAdd(newSet, block);

                if (HashSetDifferent(newSet, block->pDom)) {
                    //如果不同 释放原来的

                    HashSetDeinit(block->pDom);
                    block->pDom = newSet;
                    changed |= true;


                   // printf("block %d", block->id);
                    //printf("pdom :");
                    HashSetFirst(newSet);
                    for (BasicBlock *block = HashSetNext(newSet); block != NULL; block = HashSetNext(newSet)) {
                        //printf("b%d ", block->id);
                    }
                    //printf("\n");
                } else {
                    HashSetDeinit(newSet);
                }
            }
        }
    }


    HashSetFirst(allBlocks);
    for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)) {
        //打印看看求的对不对
        //printf("block b%d : pdom : ", block->id);
        HashSetFirst(block->pDom);
        for (BasicBlock *pDomBlock = HashSetNext(block->pDom);
             pDomBlock != NULL; pDomBlock = HashSetNext(block->pDom)) {
            //printf("b%d", pDomBlock->id);
        }
        //printf("\n");
    }

    //好好好现在是对的了 我们继续求rdf
    HashSetFirst(allBlocks);
    for (BasicBlock *X = HashSetNext(allBlocks); X != NULL; X = HashSetNext(allBlocks)) {
        //
        HashSetFirst(tempSet);
        for (BasicBlock *Y = HashSetNext(tempSet); Y != NULL; Y = HashSetNext(tempSet)) {
            // 如果Y->pDom 含有X
            if (HashSetFind(Y->pDom, X)) {
                // Y 的preds里面的Z又没有X的就是X的reverse dominance froniter
                HashSetFirst(Y->preBlocks);
                for (BasicBlock *Z = HashSetNext(Y->preBlocks); Z != NULL; Z = HashSetNext(Y->preBlocks)) {
                    if (!HashSetFind(Z->pDom, X) || Z == X) {
                        // 那么就代表了 X的RDF里面
                        HashSetAdd(X->rdf, Z);
                    }
                }
            }
        }
    }


    HashSetFirst(allBlocks);
    for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)) {
        //
        //printf("block b%d rdfs:", block->id);
        HashSetFirst(block->rdf);
        for (BasicBlock *rdf = HashSetNext(block->rdf); rdf != NULL; rdf = HashSetNext(block->rdf)) {
            // 这样打印一下reverse dominance frontier
            //printf("b%d", rdf->id);
        }

        //printf("\n");
    }


    // 接下来我们再来求一下 post immediate dominanc
    HashSetFirst(allBlocks);
    for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)) {
        HashSet *pDomSet = HashSetInit();
        HashSetCopy(pDomSet, block->pDom);
        HashSetRemove(pDomSet, block);

        HashSetFirst(block->pDom);
        for (BasicBlock *s = HashSetNext(block->pDom); s != NULL; s = HashSetNext(block->pDom)) {
            if (s != block) {
                HashSetFirst(pDomSet);
                for (BasicBlock *t = HashSetNext(pDomSet); t != NULL; t = HashSetNext(pDomSet)) {
                    if (t != s && HashSetFind(s->pDom, t)) {
                        HashSetRemove(pDomSet, t);
                    }
                }
            }
        }


        //这下pDomSet应该只有一个了 我们为这个block里面ipDom写吧！！
        HashSetFirst(pDomSet);
        BasicBlock *ipDom = (BasicBlock *) HashSetNext(pDomSet);
        block->ipDom = ipDom;
    }



    //构建post Dominance Tree
    HashSetFirst(allBlocks);
    for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)) {
        // 构建postDominate Tree
        PostDomNode *postDomNode = createPostDomNode(block, block->ipDom);
        block->postDomNode = postDomNode;
    }


    //进行链接
    HashSetFirst(allBlocks);
    for (BasicBlock *block = HashSetNext(allBlocks); block != NULL; block = HashSetNext(allBlocks)){
        if (block->ipDom){
            // 打印一下看看是不是对的
           // printf("current block %d its pDom is %d\n",block->id,block->ipDom->id);
            PostDomNode *ipDomNode = block->ipDom->postDomNode;
            HashSetAdd(ipDomNode->children,block->postDomNode);
        }
    }
    HashSetDeinit(tempSet);
}

void removeUnreachable(Function *currentFunction){
    HashSet* workList = HashSetInit();

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    clear_visited_flag(entry);
    HashSetAdd(workList, entry);


    // 先识别无用的Block
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *next = (BasicBlock *)HashSetNext(workList);
        HashSetRemove(workList,next);
        assert(next->visited == false);
        next->visited = true;

        //clean all prev
        HashSetClean(next->preBlocks);

        if(next->true_block && next->true_block->visited == false){
            HashSetAdd(workList,next->true_block);
        }

        if(next->false_block && next->false_block->visited == false){
            HashSetAdd(workList, next->false_block);
        }
    }

    //然后remove Block里面的所有信息
    InstNode *headNode = entry->head_node;
    InstNode *tailNode = tail->tail_node;
    //然后reconstruct CFG 主要是前后信息需要更新

    InstNode *currNode = headNode;
    while(currNode != get_next_inst(tailNode)){
        BasicBlock *parent = currNode->inst->Parent;
        if(parent->visited == false){
            //
            //printf("block %d is unreachable!\n",parent->id);
            assert(parent->head_node == currNode);
            InstNode *deleteNode = currNode;
            while(deleteNode != get_next_inst(parent->tail_node)){
                InstNode *tempNode = deleteNode;
                deleteNode = get_next_inst(deleteNode);
                deleteIns(tempNode);
            }
            currNode = deleteNode;
            assert(currNode->inst->Opcode == Label);

        }else{
            currNode = get_next_inst(currNode);
        }
    }

    clear_visited_flag(entry);
    HashSetClean(workList);
    //now reconstruct prev
    HashSetAdd(workList, entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);

        block->visited = true;
        if(block->true_block){
            HashSetAdd(block->true_block->preBlocks,block);
            if(block->true_block->visited == false){
                HashSetAdd(workList,block->true_block);
            }
        }
        if(block->false_block){
            HashSetAdd(block->false_block->preBlocks,block);
            if(block->false_block->visited == false){
                HashSetAdd(workList,block->false_block);
            }
        }
    }

    clear_visited_flag(entry);
}

void cleanAll(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    HashSet *workList = HashSetInit();
    clear_visited_flag(entry);

    // workList 是的一个HashSet
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);

        assert(block->visited == false);
        block->visited = true;

        HashSetClean(block->pDom);
        HashSetClean(block->dom);
        HashSetClean(block->df);
        HashSetClean(block->rdf);


        DomTreeNode *domTreeNode = block->domTreeNode;
        if(domTreeNode != NULL){
            HashSetDeinit(domTreeNode->children);
            free(domTreeNode);
        }
        // 再其它函数里面进行清理
        block->domTreeNode = NULL;

        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
}

void cleanDominance(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    HashSet *workList = HashSetInit();
    clear_visited_flag(entry);

    // workList 是的一个HashSet
    HashSetAdd(workList,entry);
    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);

        assert(block->visited == false);
        block->visited = true;

        HashSetClean(block->dom);
        HashSetClean(block->df);

        DomTreeNode *domTreeNode = block->domTreeNode;
        if(domTreeNode != NULL){
            HashSetDeinit(domTreeNode->children);
            free(domTreeNode);
        }
        // 再其它函数里面进行清理
        block->domTreeNode = NULL;

        if(block->true_block && block->true_block->visited == false){
            HashSetAdd(workList,block->true_block);
        }

        if(block->false_block && block->false_block->visited == false){
            HashSetAdd(workList,block->false_block);
        }
    }
}

void cleanPostDominance(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;

    clear_visited_flag(entry);

    HashSet *workList = HashSetInit();

    HashSetAdd(workList,entry);

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        HashSetRemove(workList,block);
        block->visited = true;

    }
}

void dominanceAnalysis(Function *currentFunction){
    cleanDominance(currentFunction);

    print_function_info(currentFunction);

    clear_visited_flag(currentFunction->entry);

    removeUnreachable(currentFunction);

    calculate_dominance(currentFunction);

    clear_visited_flag(currentFunction->entry);

    calculate_dominance_frontier(currentFunction);

    calculate_iDominator(currentFunction);

    calculate_DomTree(currentFunction);
}

void postDominanceAnalysis(Function *currentFunction){

    BasicBlock *entry = currentFunction->entry;

    assert(Optimize == true);

    clear_visited_flag(entry);

    removeUnreachable(currentFunction);

    clear_visited_flag(entry);

    calculatePostDominance(currentFunction);

    clear_visited_flag(entry);
}