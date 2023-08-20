//
// Created by Valerian on 2023/7/27.
//

#include "sccp.h"
//finally we arrived at sparse conditional constant propagation!!
List *CFGWorkList; //BasicBlock *
List *SSAWorkList; //Instruction *
HashSet *ExecutedMap; // Executed Blocks
HashMap *LatticeMap; // Value * -> LatticeValue;
HashSet *ExecutedEdge; // Executed Edge


//我们共同分析Assign 和 Terminator Instruction
void EvaluateAssignAndTerminator(InstNode *instNode){

}

void EvaluatePhi(InstNode *phiNode){

}

void EvaluateOperands(InstNode *phiNode){

}

void EvaluateResult(InstNode *phiNode){

}

void SCCP(Function *currentFunction){

    //insert param Lattice Value to the map


    //
    BasicBlock *entry = currentFunction->entry;
    CFGWorkList = ListInit();
    ListPushBack(CFGWorkList,entry);
    while(ListSize(CFGWorkList) != 0 || ListSize(SSAWorkList) != 0){
        if(ListSize(CFGWorkList) != 0){
            ListFirst(CFGWorkList,false);
            BasicBlock *block = NULL;
            ListGetFront(CFGWorkList,(void *)&block);

            //remove edge
            ListPopFront(CFGWorkList);

            //assert(block != NULL);
            //TODO 按照EAC上的处理 目前我们是抄的别人的 --

            //if e is marked as unexecuted then:
            //mark e as executed

            //
            InstNode *blockHead = block->head_node;
            InstNode *blockTail = block->tail_node;
            while(blockHead != blockTail){
                if(blockHead->inst->Opcode == Phi){

                }
                blockHead = get_next_inst(blockHead);
            }

            //if no other edge entering n is marked as executed
            //TODO modify
            if(!HashSetFind(ExecutedMap,block)){
                HashSetAdd(ExecutedMap,block);

                //for all instructions
                InstNode *headNode = block->head_node;
                InstNode *tailNode = block->tail_node;
                InstNode *nextNode = get_next_inst(tailNode);
                while(headNode != nextNode){
                    EvaluateAssignAndTerminator(headNode);
                    headNode = get_next_inst(headNode);
                }
            }
        }else if(ListSize(SSAWorkList) != 0){
            ListFirst(SSAWorkList, false);
            InstNode *instNode = NULL;
            ListGetFront(SSAWorkList,(void *)instNode);

            //remove the edge
            ListPopFront(SSAWorkList);
            BasicBlock *block = instNode->inst->Parent;

            //if any edge entering C is marked as executed then:
            if(HashSetFind(ExecutedMap,block)){
                if(instNode->inst->Opcode == Phi){
                    EvaluatePhi(instNode);
                }else{
                    EvaluateAssignAndTerminator(instNode);
                }
            }
        }
    }
}