//
// Created by Valerian on 2023/3/31.
//

#include "DeadCodeElimination.h"
const Opcode EssentialOpcodes[] = {GIVE_PARAM, FunBegin,FunEnd};
BasicBlock *findNearestMarkedPostDominator(PostDomNode *postDomNode){
    bool marked = false;
    BasicBlock *parent = postDomNode->parent;
    InstNode *currNode = parent->head_node;
    while(currNode != parent->tail_node){
        if(currNode->inst->isCritical == true){
            return parent;
        }
        currNode = get_next_inst(currNode);
    }
    return findNearestMarkedPostDominator(parent->postDomNode);
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

    //
    InstNode *currNode = entry->head_node;
    while(currNode != tail->tail_node){
        // clear mark
        currNode->inst->isCritical = false;

        if(isCriticalOperator(currNode)){
            printf("first add critical %d\n",currNode->inst->i);
            // mark as useful
            currNode->inst->isCritical = true;
            HashSetAdd(workList,currNode->inst);
        }
        currNode = get_next_inst(currNode);
    }

    printf("after frist!\n");

    while(HashSetSize(workList) != 0){
        HashSetFirst(workList);
        //remove i from worklist
        Instruction *instNode = HashSetNext(workList);

        printf("current at %d\n",instNode->i);
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


        //TODO 找到def 不能是store的全局 并且不能是GiveParam的第二个value、
        //并且putint


        if(instNode->Opcode == Call || instNode->Opcode == Alloca){
            lhs = NULL;
            rhs = NULL;
        }

        if(instNode->Opcode == GIVE_PARAM){
            rhs = NULL;
        }

        //  // 找到对应的instruction TODO 修改部分
        if(lhs != NULL && !isImm(lhs) && !isGlobalVar(lhs) && !isGlobalArray(lhs)){
            printf("lhs : %s\n ",lhs->name);
            Instruction *defLhs = (Instruction*)lhs;
            assert(defLhs != NULL);
            if(defLhs->isCritical == false){
                defLhs->isCritical = true;
                HashSetAdd(workList,defLhs);
            }
        }

        if(rhs != NULL && !isImm(rhs) && !isGlobalVar(rhs) && !isGlobalArray(rhs)){
            printf("rhs : %s\n ",rhs->name);
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
                // 有
                if(!isImm(src)) {
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

        //除了br之外的每一条语句我们都需要去找reverseBlock
        if(instNode->Opcode != br){
            //计算每条
            BasicBlock *block = instNode->Parent;

            printf("block : %d\n",block->id);
            HashSetFirst(block->rdf);
            for(BasicBlock *rdf = HashSetNext(block->rdf); rdf != NULL; rdf = HashSetNext(block->rdf)){

                InstNode *rdfTail = rdf->tail_node;

                assert(rdfTail->inst->Opcode == br_i1);

                if(rdfTail->inst->isCritical == false){
                    rdfTail->inst->isCritical = true;
                    HashSetAdd(workList,rdfTail->inst);
                }
            }
        }
    }
}

void Sweep(Function *currentFunction) {
    BasicBlock *entry = currentFunction->entry;
    BasicBlock *tail = currentFunction->tail;
    //
    InstNode *currNode = entry->head_node;
    while (currNode != tail->tail_node) {
        if (currNode->inst->isCritical == false && !isEssentialOperator(currNode)) {
            if (currNode->inst->Opcode == br_i1) {
                //rewrite i with a jump to i's nearest marked postDominator
                BasicBlock *block = currNode->inst->Parent;

                // 找到它的post Dominator
                PostDomNode *postDomNode = block->postDomNode;

                BasicBlock *markedPostDominator = findNearestMarkedPostDominator(postDomNode);

                //修改它的后继节点
                block->true_block = markedPostDominator;
                //

                // TODO 修改marked postDominator的前驱节点  暂时不修改后面需要的时候重新计算

                // rewrite this branch with a jump instruction
                InstNode *branchNode = block->tail_node;

                //先delete吧，这样也释放了内存
                deleteIns(branchNode);

                Instruction *jumpIns = ins_new_zero_operator(br);
                InstNode *jumpNode = new_inst_node(jumpIns);
                jumpIns->Parent = block;

                Value *insValue = ins_get_dest(jumpIns);

                //跳转到这个位置
                insValue->pdata->instruction_pdata.true_goto_location = markedPostDominator->id;
                block->tail_node = jumpNode;

                InstNode *jumpPrev = get_prev_inst(jumpNode);
                ins_insert_after(jumpNode,jumpPrev);


                // TODO 解决掉后面可能会引起的phi函数的冲突问题
            } else if (currNode->inst->Opcode == br) {
                // br 不变
            } else {
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
}

//
void postOrderTraversal(Vector *vector, BasicBlock *block){
    if(block->true_block){
        //判断是不是back-edge
        if(!HashSetFind(block->dom,block->true_block)){
            postOrderTraversal(vector,block->true_block);
        }
    }
    if(block->false_block){
        if(!HashSetFind(block->dom,block->false_block)){
            postOrderTraversal(vector,block->false_block);
        }
    }
    VectorPushBack(vector,block);
}


void Clean(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    bool changed = true;
    while(changed){
        changed = false;
        // compute postorder

        Vector *vector = VectorInit(10);

        postOrderTraversal(vector,entry);


        unsigned size = VectorSize(vector);

        printf("vector is %u\n",size);


        BasicBlock *temp = NULL;

        for(unsigned i = 0; i < size; i++){
            VectorGet(vector,i,(void *)&temp);
            printf("%d is b%d\n",i,temp->id);
        }

        //changed = OnePass(vector);

    }
}

bool isEmpty(BasicBlock *block){
    InstNode *inst = block->head_node;

    // 第一个InstNode 是Label
    // 最后一个是br 或者 br_label
    while(inst != block->tail_node){

        inst = get_next_inst(inst);
    }
}

void combine(BasicBlock *i, BasicBlock *j){
    //combine i and j
    assert(i->tail_node->inst->Opcode == br);

    //删除最后一个br Node
    deleteIns(i->tail_node);

    InstNode *labelNode = j->head_node;

    InstNode *currNode = labelNode;
    //把j的所有的block放到前面
    while(currNode != get_next_inst(j->tail_node)){
        //
        currNode->inst->Parent = i;
        currNode = get_next_inst(currNode);
    }

    //delete LabelNode
    deleteIns(labelNode);

    //block i 的尾节点
    i->tail_node = j->tail_node;
}

void dealWithPhi(BasicBlock *i,BasicBlock *j){
    // 如果i中有phi函数那么 j中的phi函数必须要换一种形式
    InstNode *currNode = i->head_node;
    while(currNode != i->tail_node){
        if(currNode->inst->Opcode == Phi){
            Value *alloc = currNode->inst->user.value.alias;
            assert(alloc != NULL);
            //找到j里面对应的Alloca
            InstNode *nextNode = j->head_node;
            while(nextNode != j->tail_node){
                if(nextNode->inst->Opcode == Phi){
                    Value *jAlloc = nextNode->inst->user.value.alias;
                    HashSet *jSet = nextNode->inst->user.value.pdata->pairSet;
                    if(jAlloc == alloc){
                        //将这个block的phiSet 里的信息全部转移
                        HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
                        HashSetFirst(phiSet);
                        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                            HashSetAdd(jSet,phiInfo);
                        }
                        break;
                    }
                }
                nextNode = get_next_inst(nextNode);
            }
        }
    }
}

void correctWithPhi(BasicBlock *i, BasicBlock *j){
    //
    InstNode *currNode = j->head_node;
    while(currNode != j->tail_node){
        //
        if(currNode->inst->Opcode == Phi){
            // remove i 的info
            HashSet *phiSet = currNode->inst->user.value.pdata->pairSet;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                if(phiInfo->from == i){
                    //remove this one
                    HashSetRemove(phiSet,phiInfo);
                }


                //添加到j的后继节点上面去
                if(j->true_block){
                    //insert phi

                }

                if(j->false_block){
                    //insert phi
                }
            }
        }
        currNode = get_next_inst(currNode);
    }

    //TODO 可能存在的phi函数的出现在一个block中间的情况？
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

bool OnePass(Vector* vector) {
    bool changed = false;
    //for each block i in postorder
    BasicBlock *block = NULL;
    unsigned size = VectorSize(vector);
    for (unsigned i = 0; i < size; i++) {
        VectorGet(vector, i, (void *) &block);
        assert(block != NULL);

        // if i ends in a conditional branch
        if (block->tail_node->inst->Opcode == br_i1) {
            Value *insValue = ins_get_dest(block->tail_node->inst);
            int trueLocation = insValue->pdata->instruction_pdata.true_goto_location;
            int falseLocation = insValue->pdata->instruction_pdata.false_goto_location;
            if (trueLocation == falseLocation) {
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
            }
        }

        //if i ends in a jump to j then
        if (block->tail_node->inst->Opcode == br) {
            //if i is empty then
            if (isEmpty(block)) {
                changed = true;
                //replace transfers to i with transfers to j
                BasicBlock *j = block->true_block;

                //i的前驱加在j上
                HashSetFirst(block->preBlocks);
                for (BasicBlock *iPrevBlock = HashSetNext(block->preBlocks);
                     iPrevBlock != NULL; iPrevBlock = HashSetNext(block->preBlocks)) {
                    // 前面block
                    if (iPrevBlock->true_block == block) {
                        iPrevBlock->true_block = j;
                        // TODO 还有一些需要完善的信息
                        InstNode *instNode = iPrevBlock->tail_node;
                        //修改trueLocation
                        instNode->inst->user.value.pdata->instruction_pdata.true_goto_location = j->id;
                    }else if (iPrevBlock->false_block == block) {
                        iPrevBlock->false_block = j;
                        InstNode *instNode = iPrevBlock->tail_node;
                        //TODO 还要维护很多信息
                        instNode->inst->user.value.pdata->instruction_pdata.false_goto_location = j->id;
                    }
                    // j add 前驱
                    HashSetAdd(j->preBlocks,iPrevBlock);
                }

                //block里面的phi函数全部移动到j里面去
                dealWithPhi(block,j);


                //删除block的所有inst
                removeBlock(block);
            }

            //if j has only one predecessor
            BasicBlock *j = block->true_block;
            if (HashSetSize(j->preBlocks) == 1) {
                changed = true;
                //combine i and j
                combine(block, j);
            }

            //if j is empty and ends in a conditional branch then
            if (isEmpty(j) && j->tail_node->inst->Opcode == br_i1) {
                changed = true;

                Instruction *branchIns = ins_new_zero_operator(br_i1);
                InstNode *branchNode = new_inst_node(branchIns);
                branchIns->user.value.pdata->instruction_pdata.true_goto_location = j->true_block->id;
                branchIns->user.value.pdata->instruction_pdata.false_goto_location = j->false_block->id;

                ins_insert_before(branchNode,block->tail_node);

                deleteIns(block->tail_node);
                block->tail_node = branchNode;


                correctWithPhi(block,j);
            }
        }
    }
    return changed;
}


void reconstructCFG(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;

    //主要是对CFG的prevBlocks集合进行重新计算
    HashSet *workList = HashSetInit();
    HashSetAdd(workList, entry);

    while(HashSetSize(workList) != 0){
        //
        HashSetFirst(workList);
        BasicBlock *block = HashSetNext(workList);
        block->visited = true;

    }
}