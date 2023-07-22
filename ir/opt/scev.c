//
// Created by Valerian on 2023/7/18.
//

#include "scev.h"
HashMap * addRec;
void analysisAddRec(Loop *loop){

    //这个函数就做分析phi 出来AddRec这件事请

    //这个函数主要分析Loop里面的AddRec

    //首先我们需要定位到Phi函数

    //理论上来说应该是一个迭代的过程
    BasicBlock *loopEntry = loop->head;

    //不是2根本分析不了
    if(HashSetSize(loopEntry->preBlocks) != 2) return;

    InstNode *entryHead = loopEntry->head_node;
    InstNode *entryTail = loopEntry->tail_node;
    while(entryHead != entryTail){
        if(entryHead->inst->Opcode == Phi){
            //
            Value *phiDest = ins_get_dest(entryHead->inst);

            HashSet *phiSet = phiDest->pdata->pairSet;
            assert(HashSetSize(phiSet) != 2);

            Value *initValue = NULL;
            Value *addRecValue = NULL;
            HashSetFirst(phiSet);
            for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                BasicBlock *from  = phiInfo->from;


                if(!HashSetFind(loop->loopBody,from)){
                    //找到来自非循环的基本块
                    //so we get the initValue
                    initValue = phiInfo->define;
                }else{
                    //来自循环内部我们去看看这个变量是否是Add出来的一个变量
                    addRecValue = phiInfo->define;
                }
            }

            //OK now we get the possible initValue &  addRevValue
            assert(addRecValue != NULL);
            assert(initValue != NULL);

            //it is within a loop so the addRecValue can't be a param and thus we can find the instruction
            Instruction *addRecIns = (Instruction *)addRecValue;

            //TODO 我们不分析减法？
            if(addRecIns->Opcode != Add){
                //get next instNode
                entryHead = get_next_inst(entryHead);
                continue;
            }else{
                Value *addRecLhs = ins_get_lhs(addRecIns);
                Value *addRecRhs = ins_get_rhs(addRecIns);


                //see if the add is the phi + some addRec(already known addRec / constant / loop-invariant variable)
                if(addRecLhs != phiDest && addRecRhs != phiDest){
                    //ops we are dead

                    entryHead = get_next_inst(entryHead);
                    continue;
                }

                Value *other = (addRecLhs == phiDest) ? addRecRhs : addRecLhs;

                //
                if(isImmInt(other)){
                    //if so we know we find a addRecExpression

                }else if(isLoopInvariant(loop,other)){


                }
            }
        }else{
            entryHead = get_next_inst(entryHead);
        }
    }
}