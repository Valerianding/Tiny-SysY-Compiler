//
// Created by Valerian on 2023/3/31.
//

#include "dce.h"
const Opcode EssentialOpcodes[] = {GIVE_PARAM, FunBegin,FunEnd,Label,SysYMemset};
BasicBlock *findNearestMarkedPostDominator(PostDomNode *postDomNode){
    BasicBlock *parent = postDomNode->parent;
    InstNode *currNode = parent->head_node;
    while(currNode != parent->tail_node){
        if(currNode->inst->isCritical == true && currNode->inst->Opcode != br){
            return parent;
        }
        currNode = get_next_inst(currNode);
    }
    return findNearestMarkedPostDominator(parent->postDomNode);
}

bool isEmpty(BasicBlock *block){
    InstNode *inst = block->head_node;
    // 第一个InstNode 是Label
    // 最后一个是br 或者 br_label
    while(inst != block->tail_node){
        if(inst->inst->Opcode == Label || inst->inst->Opcode == Phi || inst->inst->Opcode == FunBegin){
            inst = get_next_inst(inst);
        }else{
            return false;
        }
    }
    return true;
}

void combine(BasicBlock *i, BasicBlock *j){
    //combine i and j
    assert(i->tail_node->inst->Opcode == br);

    InstNode *iOriginalTailNode = i->tail_node;


    i->tail_node = get_prev_inst(iOriginalTailNode);


    removeIns(iOriginalTailNode);

    InstNode *labelNode = j->head_node;

    removeIns(labelNode);

    InstNode *currNode = get_next_inst(labelNode);


    InstNode *iHeadNode = i->head_node;
    InstNode *jHeadNode = j->head_node;
    //printf("combine block %d, %d  each headnode is %d,  %d\n",i->id,j->id,iHeadNode->inst->i,jHeadNode->inst->i);


    //delete labelNode
    deleteIns(labelNode);


    InstNode *jTailNextNode = get_next_inst(j->tail_node);

    InstNode *nextLabelNode = get_next_inst(j->tail_node);

    //把j的所有的block放到前面
    //这里不能使用原来的j tailnode了
    while(currNode != nextLabelNode){

        currNode->inst->Parent = i;
        InstNode *tempNode = get_next_inst(currNode);

        removeIns(currNode);
        ins_insert_after(currNode,i->tail_node);

        i->tail_node = currNode;
        currNode = tempNode;
    }

    //还需要调整j->true block 和 j->false block里面的phi函数
    if(j->true_block){
        i->true_block = j->true_block;
        HashSetRemove(j->true_block->preBlocks,j);
        HashSetAdd(j->true_block->preBlocks,i);
        InstNode *jTrueNode = j->true_block->head_node;
        while(jTrueNode != j->true_block->tail_node){
            if(jTrueNode->inst->Opcode == Phi){
                HashSet *jTrueSet = jTrueNode->inst->user.value.pdata->pairSet;
                HashSetFirst(jTrueSet);
                for(pair *phiInfo = HashSetNext(jTrueSet); phiInfo != NULL; phiInfo = HashSetNext(jTrueSet)){
                    if(phiInfo->from == j){
                        phiInfo->from = i;
                    }
                }
            }
            jTrueNode = get_next_inst(jTrueNode);
        }
    }

    if(j->false_block){
        i->false_block = j->false_block;
        HashSetRemove(j->false_block->preBlocks,j);
        HashSetAdd(j->false_block->preBlocks,i);
        InstNode *jFalseNode = j->false_block->head_node;
        while(jFalseNode != j->false_block->tail_node){
            if(jFalseNode->inst->Opcode == Phi){
                HashSet *jFalseSet = jFalseNode->inst->user.value.pdata->pairSet;
                HashSetFirst(jFalseSet);
                for(pair *phiInfo = HashSetNext(jFalseSet); phiInfo != NULL; phiInfo = HashSetNext(jFalseSet)){
                    if(phiInfo->from == j){
                        phiInfo->from = i;
                    }
                }
            }
            jFalseNode = get_next_inst(jFalseNode);
        }
    }
}

bool isEssentialOperator(InstNode *inst){
    int size = sizeof(EssentialOpcodes) / sizeof(Opcode);
    int i;
    for(i = 0; i < size; i++){
        if(inst->inst->Opcode == EssentialOpcodes[i]){
            return true;
        }
    }
    return false;
}



void Mark(Function *currentFunction){
    //
    HashSet *workList = HashSetInit();   //放的是instruction *类型

    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;

    InstNode *headNode = entry->head_node;
    assert(headNode->inst->Opcode == FunBegin);

    int paramNum = headNode->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;

    //
    InstNode *currNode = entry->head_node;
    while(currNode != tail->tail_node){
        // clear mark
        currNode->inst->isCritical = false;

        if(isCriticalOperator(currNode)){
            //printf("first add critical %d\n",currNode->inst->i);
            // mark as useful
            currNode->inst->isCritical = true;
            HashSetAdd(workList,currNode->inst);
        }
        currNode = get_next_inst(currNode);
    }


    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        //remove i from worklist
        Instruction *instNode = HashSetNext(workList);

        //printf("current at %d\n",instNode->i);
        HashSetRemove(workList,instNode);

        Value *insValue = ins_get_dest(instNode);

        Value *lhs = NULL;
        Value *rhs = NULL;
        if(insValue->NumUserOperands == (unsigned)1){
            lhs = ins_get_lhs(instNode);
        }
        if(insValue->NumUserOperands == (unsigned int)2){

            lhs = ins_get_lhs(instNode);
            rhs = ins_get_rhs(instNode);
        }


        //TODO 找到def 不能是store的全局 并且不能是GiveParam的第二个value
        if(instNode->Opcode == Call || instNode->Opcode == Alloca || instNode->Opcode == FunEnd){
            lhs = NULL;
            rhs = NULL;
        }

        if(instNode->Opcode == GIVE_PARAM){
            rhs = NULL;
        }

        //  // 找到对应的instruction TODO 修改部分
        if(lhs != NULL && !isImm(lhs) && !isGlobalVar(lhs) && !isGlobalArray(lhs) && !isParam(lhs,paramNum)){
            //printf("lhs : %s\n ",lhs->name);
            Instruction *defLhs = (Instruction*)lhs;
            assert(defLhs != NULL);
            if(defLhs->isCritical == false){
                defLhs->isCritical = true;
                HashSetAdd(workList,defLhs);
            }
        }

        if(rhs != NULL && !isImm(rhs) && !isGlobalVar(rhs) && !isGlobalArray(rhs) && !isParam(rhs,paramNum)){
            //printf("rhs : %s\n ",rhs->name);
            Instruction *defRhs = (Instruction*)rhs;
            assert(defRhs != NULL);
            if(defRhs->isCritical == false){
                defRhs->isCritical = true;
                HashSetAdd(workList,defRhs);
            }
        }

        // 如果是phi函数的话还需要special care about this
        if(instNode->Opcode == Phi){
            //如果是phi的话还需要特殊处理
            HashSetFirst(insValue->pdata->pairSet);
            for(pair *phiInfo = HashSetNext(insValue->pdata->pairSet); phiInfo != NULL; phiInfo = HashSetNext(insValue->pdata->pairSet)){
                Value* src = phiInfo->define;
                BasicBlock *from = phiInfo->from;
                // 解决空block导致phi不能正常function 将phi的前驱的tail node设置为
                from->tail_node->inst->isCritical = true;
                HashSetAdd(workList,from->tail_node->inst);
                // 有
                if(src != NULL && !isImm(src) && !isParam(src,paramNum)) {
                    //不是
                    Instruction *ins = (Instruction*)src;
                    if(ins->isCritical == false){
                        ins->isCritical = true;
                        //添加进workList
                        HashSetAdd(workList,ins);
                    }
                }
            }
        }

        // 我还是觉得br不能算
        if(instNode->Opcode != br_i1){
            //计算每条
            BasicBlock *block = instNode->Parent;

//            printf("block : %d ",block->id);
//            printf("rdfs:");
            HashSetFirst(block->rdf);
            for(BasicBlock *rdf = HashSetNext(block->rdf); rdf != NULL; rdf = HashSetNext(block->rdf)) {
                //printf("b%d", rdf->id);
                InstNode *rdfTail = rdf->tail_node;

                assert(rdfTail->inst->Opcode == br_i1);

                if (rdfTail->inst->isCritical == false) {
                    rdfTail->inst->isCritical = true;
                    HashSetAdd(workList, rdfTail->inst);
                }
            }
        }
    }
}

bool Sweep(Function *currentFunction) {
    bool changed = false;
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    //
    InstNode *currNode = entry->head_node;
    while (currNode != tail->tail_node) {
        if (currNode->inst->isCritical == false && !isEssentialOperator(currNode)) {
            if (currNode->inst->Opcode == br_i1) {
                changed = true;
                InstNode *nextNode = get_next_inst(currNode);

                //rewrite i with a jump to i's nearest marked postDominator
                BasicBlock *block = currNode->inst->Parent;

                // 找到它的post Dominator
                PostDomNode *postDomNode = block->postDomNode;

                BasicBlock *markedPostDominator = findNearestMarkedPostDominator(postDomNode);

                //修改它的后继节点
                block->true_block = markedPostDominator;
                block->false_block = NULL;

                // TODO 修改marked postDominator的前驱节点  暂时不修改后面需要的时候重新计算
                // rewrite this branch with a jump instruction
                InstNode *branchNode = block->tail_node;

                Instruction *jumpIns = ins_new_zero_operator(br);
                InstNode *jumpNode = new_inst_node(jumpIns);
                jumpIns->Parent = block;
                Value *insValue = ins_get_dest(jumpIns);

                //跳转到这个位置
                insValue->pdata->instruction_pdata.true_goto_location = markedPostDominator->id;
                InstNode *jumpPrev = get_prev_inst(branchNode);
                ins_insert_after(jumpNode,jumpPrev);

                block->tail_node = jumpNode;

                //先delete吧，这样也释放了内存
                deleteIns(branchNode);
                currNode = nextNode;
                // TODO 解决掉后面可能会引起的phi函数的冲突问题
            } else if (currNode->inst->Opcode == br) {
                // br 不变
                assert(false);
            } else {
                changed = true;
                // 除了br不变的话其他的
                // TODO 解决delete_inst相关的问题
                InstNode *nextNode = get_next_inst(currNode);
                deleteIns(currNode);
                currNode = nextNode;
            }
        } else {
            currNode = get_next_inst(currNode);
        }
    }
    return changed;
}

// 深度优先就是后序遍历的逆
void depthFirstTraversal(Vector *vector, BasicBlock *block){
    block->visited = true;
    VectorPushBack(vector,block);
    if(block->true_block && block->true_block->visited == false){
        depthFirstTraversal(vector,block->true_block);
    }
    if(block->false_block && block->false_block->visited == false){
        depthFirstTraversal(vector,block->false_block);
    }
}



// 重新写一版
bool OnePass(Vector* vector) {
    bool changed = false;
    //for each block i in postorder
    BasicBlock *block = NULL;
    int size = (int)VectorSize(vector);
    for (int i = size - 1; i >= 0; i--) {
        VectorGet(vector, i, (void *) &block);
        if(block->visited == false){
            block->visited = true;
        }
        bool processed = false;
        assert(block != NULL);
        //printf("block %d\n",block->id);
        // if i ends in a conditional branch
        if (block->tail_node->inst->Opcode == br_i1) {
            Value *insValue = ins_get_dest(block->tail_node->inst);
            int trueLocation = insValue->pdata->instruction_pdata.true_goto_location;
            int falseLocation = insValue->pdata->instruction_pdata.false_goto_location;
            if (trueLocation == falseLocation) {
                printf("same branch!\n");

                changed = true;
                // replace it with a jump
                Instruction *jumpIns = ins_new_zero_operator(br);

                //后面再Mark和Sweep一次
                //jumpIns->isCritical = true;
                jumpIns->Parent = block;
                InstNode *jumpNode = new_inst_node(jumpIns);
                jumpIns->user.value.pdata->instruction_pdata.true_goto_location = trueLocation;
                ins_insert_before(jumpNode, block->tail_node);


                //删除这个inst
                deleteIns(block->tail_node);
                block->tail_node = jumpNode;
                block->false_block = NULL;

                processed = true;
            }
        }

        //if i ends in a jump to j then
        if (block->tail_node->inst->Opcode == br) {
            //if i is empty then
            if (isEmpty(block) && block->head_node->inst->Opcode != FunBegin) {
                //replace transfers to i with transfers to j
                BasicBlock *j = block->true_block;
                //首先去判断是否符合条件
                //跳过Label
                bool removeAble = true;
                InstNode *jNode = get_next_inst(j->head_node);
                int jPhiCount = 0;
                while(jNode != j->tail_node){
                    if(jNode->inst->Opcode == Phi){
                        jPhiCount++;
                        HashSet *jSet = jNode->inst->user.value.pdata->pairSet;
                        HashSetFirst(jSet);
                        for(pair *phiInfo = HashSetNext(jSet); phiInfo != NULL; phiInfo = HashSetNext(jSet)){
                            if(HashSetFind(block->preBlocks,phiInfo->from)){
                                removeAble = false;
                            }
                        }

                        //暴力一点如果j里面有多个phi函数我们直接unremoveable
                        if(jPhiCount > 1){
                            removeAble = false;
                        }

                        //另外如果j的phi包含一个不含phi函数的空基本快的前驱，如果这个基本块还有多个前驱的话，也是unremovable
                        bool iHasPhi = false;
                        Value *iPhi = NULL;
                        InstNode *iNode = block->head_node;
                        while(iNode != block->tail_node){
                            if(iNode->inst->Opcode == Phi){
                                iPhi = ins_get_dest(iNode->inst);
                                iHasPhi = true;
                                break;
                            }
                            iNode = get_next_inst(iNode);
                        }

                        if(iHasPhi == false && HashSetSize(block->preBlocks) > 1){
                            removeAble = false;
                        }

                        //如果j只有一个phi函数 但是不引用i的phi函数代表i的phi函数支配j
                        //这种情况我们也不remove
                        if(jPhiCount == 1 && iHasPhi){
                            assert(iPhi != NULL);
                            InstNode *jPhiNode = j->head_node;
                            while(jPhiNode->inst->Opcode != Phi){
                                jPhiNode = get_next_inst(jPhiNode);
                            }

                            bool condition = false;
                            Value *jPhiValue = ins_get_dest(jPhiNode->inst);
                            HashSet *jPhiSet = jPhiValue->pdata->pairSet;
                            HashSetFirst(jPhiSet);
                            for(pair *jInfo = HashSetNext(jPhiSet); jInfo != NULL; jInfo = HashSetNext(jPhiSet)){
                                if(jInfo->define == iPhi){
                                    condition = true;
                                }
                            }

                            if(condition == false){
                                removeAble = false;
                            }
                        }
                    }else{
                        break;
                    }
                    jNode = get_next_inst(jNode);
                }

                //
                if(removeAble == true && processed == false){
                    processed = true;
                    //printf("remove empty!\n");
                    //符合先决条件
                    //如果block里面有phi函数
                    bool iHasPhi = false;
                    InstNode *iNode = block->head_node;
                    while(iNode != block->tail_node){
                        if(iNode->inst->Opcode == Phi){
                            iHasPhi = true;
                            Value *iPhi = ins_get_dest(iNode->inst);
                            bool usedByJ = false;
                            //如果block里面有phi函数
                            InstNode *jNode = j->head_node;
                            while(jNode != j->tail_node){
                                if(jNode->inst->Opcode == Phi){
                                    HashSet *jSet = jNode->inst->user.value.pdata->pairSet;
                                    HashSetFirst(jSet);
                                    for(pair *phiInfo = HashSetNext(jSet); phiInfo != NULL; phiInfo = HashSetNext(jSet)){
                                        if(phiInfo->define == iPhi){
                                            usedByJ = true;
                                        }
                                    }
                                    //
                                    if(usedByJ){
                                        HashSetFirst(jSet);
                                        for(pair *phiInfo = HashSetNext(jSet); phiInfo != NULL; phiInfo = HashSetNext(jSet)){
                                            if(phiInfo->define == iPhi){
                                                HashSetRemove(jSet,phiInfo);
                                            }
                                        }

                                        //i的信息全部加在j上面去
                                        HashSet *iSet = iNode->inst->user.value.pdata->pairSet;
                                        HashSetFirst(iSet);
                                        for(pair *phiInfo = HashSetNext(iSet); phiInfo != NULL; phiInfo = HashSetNext(iSet)){
                                            HashSetAdd(jSet,phiInfo);
                                        }
                                    }
                                }
                                jNode = get_next_inst(jNode);
                            }
                            //如果i本来有phi函数出了外层循环还是没有usedbyJ的话
                            if(!usedByJ){
                                //那么我们期待j只有一个前驱了 所以我们
                                assert(HashSetSize(j->preBlocks) == 1);
                                InstNode *nextNode = get_next_inst(iNode);
                                removeIns(iNode);
                                ins_insert_after(j->head_node,iNode);
                                iNode->inst->Parent = j;
                                iNode = nextNode;
                            }else{
                                //到的这里是j里面有被i使用的
                                //我们期待所有的i里面的phi都能被j使用 默认都已经处理掉了所以我们
                                iNode = get_next_inst(iNode);
                            }
                        }else{
                            iNode = get_next_inst(iNode);
                        }
                    }

                    if(iHasPhi == false){
                        //i如果没有phi函数并且后面有phi函数的话，需要将来自这个block的信息修改一下
                        InstNode *jNode = j->head_node;
                        HashSetFirst(block->preBlocks);
                        BasicBlock *iPre = HashSetNext(block->preBlocks);
                        while(jNode != j->tail_node){
                            if(jNode->inst->Opcode == Phi){
                                //只有如果后面是phi函数的时候需要检查
                                assert(HashSetSize(block->preBlocks) == 1);

                                HashSet *jSet = jNode->inst->user.value.pdata->pairSet;
                                HashSetFirst(jSet);
                                for(pair *phiInfo = HashSetNext(jSet); phiInfo != NULL; phiInfo = HashSetNext(jSet)){
                                    if(phiInfo->from == block){
                                        phiInfo->from = iPre;
                                    }
                                }
                            }
                            jNode = get_next_inst(jNode);
                        }
                    }

                    HashSetFirst(block->preBlocks);
                    for(BasicBlock *preBlock = HashSetNext(block->preBlocks); preBlock != NULL; preBlock = HashSetNext(block->preBlocks)){
                        //
                        if(preBlock->true_block == block){
                            preBlock->true_block = j;
                            InstNode *preTail = preBlock->tail_node;
                            preTail->inst->user.value.pdata->instruction_pdata.true_goto_location = j->id;

                        }else if(preBlock->false_block == block){
                            preBlock->false_block = j;
                            InstNode *preTail = preBlock->tail_node;
                            preTail->inst ->user.value.pdata->instruction_pdata.false_goto_location = j->id;

                        }
                        HashSetAdd(j->preBlocks,preBlock);
                    }

                    removeBlock(block);
                    HashSetRemove(j->preBlocks,block);
                }else{
                    //removeAble
                    changed = false;
                }
            }

            //if j has only one predecessor
            BasicBlock *j = block->true_block;
            if (HashSetSize(j->preBlocks) == 1 && processed == false) {
                changed = true;
                processed = true;
                HashSetFirst(j->preBlocks);
                BasicBlock *jPrev = HashSetNext(j->preBlocks);
                //printf("block is %d, jPrev is %d\n",block->id,jPrev->id);

                assert(jPrev == block);


                //printf("combine blocks! suc: %d\n",j->id);

                if(j->true_block){
                    //printf("suc true: %d, ",j->true_block->id);
                }

                if(j->false_block){
                    //printf("suc false: %d, ",j->false_block->id);
                }

                //printf("\n");

                HashSetFirst(j->preBlocks);

                //combine i and j
                combine(block, j);
            }

            //if j is empty and ends in a conditional branch then
            if (isEmpty(j) && j->tail_node->inst->Opcode == br_i1 && processed == false) {

                //当前基本块有并且只能有一个phi函数！！ 其他情况都有点怪 我都assert false

                int countPhi = 0;
                InstNode *jNode = j->head_node;
                InstNode *jTailNode = j->tail_node;
                while(jNode != jTailNode){
                    if(jNode->inst->Opcode == Phi){
                        countPhi++;
                    }
                    jNode = get_next_inst(jNode);
                }

                bool removeAble = true;
                //并且j的后续基本块中应该不能包含phi函数
                if(j->true_block){
                    InstNode *jTrueNode = j->true_block->head_node;
                    InstNode *jTrueTail = j->true_block->tail_node;
                    while(jTrueNode != jTrueTail){
                        if(jTrueNode->inst->Opcode == Phi){
                            removeAble = false;
                        }
                        jTrueNode = get_next_inst(jTrueNode);
                    }
                }

                if(j->false_block){
                    InstNode *jFalseNode = j->false_block->head_node;
                    InstNode *jFalseTail = j->true_block->tail_node;
                    while(jFalseNode != jFalseTail){
                        if(jFalseNode->inst->Opcode == Phi){
                            removeAble = false;
                        }
                        jFalseNode = get_next_inst(jFalseNode);
                    }
                }

                // TODO 解决如果包含phi函数的问题
                // TODO为什么
                if(countPhi <= 1 && removeAble){
                    changed = true;
                    processed = true;
                    assert(false);
                    printf("hoist a branch!\n");

                    InstNode *jNode = j->head_node;
                    InstNode *jTailNode = j->tail_node;

                    while(jNode != jTailNode){
                        if(jNode->inst->Opcode == Phi){
                            assert(countPhi == 1);
                            //将phi函数还原
                            HashSet *jSet = jNode->inst->user.value.pdata->pairSet;
                            HashSetFirst(jSet);
                            for(pair *phiInfo = HashSetNext(jSet); phiInfo != NULL; phiInfo = HashSetNext(jSet)){
                                Value *define = phiInfo->define;
                                BasicBlock *from = phiInfo->from;

                                Instruction *branch = ins_new_unary_operator(br_i1, define);
                                branch->user.value.pdata->instruction_pdata.true_goto_location = j->true_block->id;
                                branch->user.value.pdata->instruction_pdata.false_goto_location = j->false_block->id;
                                InstNode *branchNode = new_inst_node(branch);

                                //
                                InstNode *fromTail = from->tail_node;
                                ins_insert_after(branchNode,fromTail);

                                //
                                from->true_block = j->true_block;
                                from->false_block = j->false_block;

                                //
                                HashSetRemove(j->true_block->preBlocks,j);
                                HashSetAdd(j->true_block->preBlocks,from);

                                //
                                HashSetRemove(j->false_block->preBlocks,j);
                                HashSetAdd(j->false_block->preBlocks,from);
                            }
                        }
                        jNode = get_next_inst(jNode);
                    }
                    removeBlock(j);
                }else{
                    assert(false);
                }
            }
        }
    }
    return changed;
}


void Clean(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    bool changed = true;
    while(changed){
         changed = false;
        // compute postorder
        Vector *vector = VectorInit(10);

        clear_visited_flag(entry);

        // DepthFirst的逆
        depthFirstTraversal(vector,entry);

        int size = (int)VectorSize(vector);

        printf("vector size is %u\n",size);

        BasicBlock *temp = NULL;

//        for(int i = size - 1; i >= 0; i--){
//            VectorGet(vector,i,(void *)&temp);
//            printf("%d is b%d\n",i,temp->id);
//        }

        printf("before one pass!\n");

        clear_visited_flag(currentFunction->entry);

        changed = OnePass(vector);

        printf("after one pass!\n");

        removeUnreachable(currentFunction);

        renameVariables(currentFunction);

        VectorDeinit(vector);
    }
}

void removeBlock(BasicBlock *block){
    InstNode *currNode = block->head_node;
    InstNode *prevNode = NULL;
    while(currNode != get_next_inst(block->tail_node)){
        prevNode = currNode;
        currNode = get_next_inst(currNode);
        deleteIns(prevNode);
    }
}