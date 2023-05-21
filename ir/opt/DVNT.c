//
// Created by Valerian on 2023/5/19.
//

/*
 * a hash map key is home value is the expression
 * a hash map key is variable value is the number
 */

#include "DVNT.h"

Opcode valueOperator[] = {Call,Add,Sub, Div,Mul,Mod};
unsigned int value_number_seed = 1;
bool isValueAbleOperator(InstNode *instNode){

}

bool DVNT(Function *currentFunction){
    BasicBlock *entry = currentFunction->entry;
    HashMap *table = HashMapInit();
    HashMap *var2num = HashMapInit();
    bool changed = DVNT_EACH(entry,table,var2num,currentFunction);
    return changed;
}

// 需要lhsValueNumber和rhsValueNumber保持一个相对位置
// 常数我们可以用负数直接代替，所以我们不希望我们产生的hashValueNumber是一个负数
unsigned long getHashValueNumber(Opcode opcode,unsigned int lhsValueNumber, unsigned int rhsValueNumber){
  const unsigned int p1 = 131, p2 = 137, p3 = 139;
  unsigned int h1 = 0, h2 = 0, h3 = 0;
  for(int i = 0; i < 31; i++){
      if(opcode && (1 << i)){
          h1 ^= (p1 << i);
      }
  }
  for(int i = 0; i < 31; i++){
      if(lhsValueNumber && (1 << i)){
          h2 ^= (p2 << i);
      }
  }
  for(int i = 0; i < 31; i++){
      if(rhsValueNumber && (1 << i)){
          h3 ^= (p3 << i);
      }
  }
  unsigned hashValue =  ((h1 << 16) | (h2 << 8) | h3);

  while(hashValue == opcode || hashValue == lhsValueNumber || hashValue == rhsValueNumber){
      h1 += p1;
      h2 += p2;
      h3 += p3;
      hashValue = ((h1 << 16) | (h2 << 8) | h3);
  }
  return hashValue;
}

bool DVNT_EACH(BasicBlock *block, HashMap *table, HashMap *var2num, Function *currentFunction) {

    bool changed = false;
    //allocate a new scope for B
    //我们使用一个hash set记录一下
    HashSet *newScope = HashSetInit();


    //for each phi function in b
    InstNode *phiNode = block->head_node;
    InstNode *tailNode = block->tail_node;
    while (phiNode != tailNode) {
        if (phiNode->inst->Opcode == Phi) {
            HashSet *phiSet = phiNode->inst->user.value.pdata->pairSet;
            // TODO 目前只考虑了变量和meaningless的情况
            // 需要针对立即数、和redundant

            // 应该不存在redundant 的情况
            HashSetFirst(phiSet);
            bool isMeaningLess = true;
            unsigned int currDefineValueNumber = 0;
            unsigned int prevDefineValueNumber = 0;
            //undefine, 1, 1也需要优化
            for (pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)) {
                Value *define = phiInfo->define;
                if (define != NULL) {
                    //找Value Number
                    if(isImm(define)){
                        //如果是是立即数
                        switch (define->VTy->ID) {
                            case Int:
                                currDefineValueNumber = -(unsigned int)define->pdata->var_pdata.iVal;
                                break;
                            case Float:
                                currDefineValueNumber = -(unsigned int)define->pdata->var_pdata.fVal;
                                break;
                            default:
                                assert(false);
                        }
                        //
                    }else{
                        unsigned int *pValueNumber = HashMapGet(var2num,define);
                        if(pValueNumber == NULL){
                            isMeaningLess = false;
                            //回边
                            break;
                        }else{
                            //得到这个valueNumber的valueNumber
                            currDefineValueNumber = *(pValueNumber);
                        }
                    }

                    if(prevDefineValueNumber != 0 && prevDefineValueNumber != currDefineValueNumber){
                            //代表了不一样了 需要 assgin new value number了
                            isMeaningLess = false;
                            break;
                    }else{
                        //代表还是一样的，我们需要继续比较
                        continue;
                    }
                }
            }


            Value *phiValue = ins_get_dest(phiNode->inst);
            if (isMeaningLess) {
                //当前的phi Value
                //value number必须是malloc出来的
                unsigned int *pValueNumber = (unsigned int *) malloc(sizeof(unsigned int));
                *pValueNumber = currDefineValueNumber;

                //put it into var2num
                HashMapPut(var2num,phiValue,pValueNumber);


                Value *replace = NULL;
                HashSetFirst(phiSet);
                for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
                    if(phiInfo->define != NULL){
                        //
                        replace = phiInfo->define;
                        break;
                    }
                }

                //valuereplace结果
                //value_replaceAll(phiValue,replace);

                //修改所有的phiFunction里面的参数

                InstNode *funcHead = currentFunction->entry->head_node;
                InstNode *funcTail = currentFunction->tail->tail_node;
                while(funcHead != funcTail){
                    if(funcHead->inst->Opcode == Phi){
                       HashSet *phiSet = funcHead->inst->user.value.pdata->pairSet;
                       HashSetFirst(phiSet);
                       for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){

                       }
                    }
                    funcHead = get_next_inst(funcHead);
                }

                //remove this instruction!!
                InstNode *tempNode = get_next_inst(phiNode);
                deleteIns(phiNode);
                phiNode = tempNode;
            }else{
                //create a new value number for this phi

            }
        }else{
            phiNode = get_next_inst(phiNode);
        }
    }


    //for each assignment of the form "x <-y op z"
    InstNode *currNode = block->head_node;
    while (currNode != tailNode) {
        if(isValueAbleOperator(currNode)){
            switch (currNode->inst->Opcode) {
                case Call: {
                    //无论call的是什么都需要给左边新建一个value number
                    Value *dest = ins_get_dest(currNode->inst);
                    unsigned int *pValue_number = (unsigned int *)malloc(sizeof(unsigned int));
                    *pValue_number = value_number_seed;
                    //每个应该是自己独有的
                    //对于call而言我们直接
                    value_number_seed++;
                    HashMapPut(var2num, dest, pValue_number);
                    break;
                }
                default: {
                    // 进入这里面就只有几个了
                    //其余的情况
                    Value *lhs = ins_get_lhs(currNode->inst);
                    Value *rhs = ins_get_rhs(currNode->inst);

                    //
                    unsigned int *pLhsNumber = NULL;
                    unsigned int *pRhsNumber = NULL;
                    unsigned int LhsNumber;
                    unsigned int RhsNumber;
                    if(lhs != NULL) {
                        if(!isImm(lhs)){
                            pLhsNumber = HashMapGet(var2num,lhs);
                            assert(pLhsNumber != NULL);
                            LhsNumber = *pLhsNumber;
                        }else{
                            if(isImmInt(lhs)){
                                LhsNumber = lhs->pdata->var_pdata.iVal;
                            }else if(isImmFloat(lhs)){
                                LhsNumber = lhs->pdata->var_pdata.fVal;
                            }else{
                                assert(false);
                            }
                        }
                    }

                    if(rhs != NULL){
                        if(!isImm(rhs)){
                            pRhsNumber = HashMapGet(var2num, rhs);
                            assert(pRhsNumber != NULL);
                            RhsNumber = *pRhsNumber;
                        }else{
                            if(!isImmInt(rhs)){
                                RhsNumber = rhs->pdata->var_pdata.iVal;
                            }else if(isImmFloat(rhs)){
                                RhsNumber = rhs->pdata->var_pdata.fVal;
                            }
                        }
                    }


                    //然后去table里面查有没有
                    //table 里面

                    //
                    Value *replace = NULL;
                    HashMapFirst(table);
                    for (Pair *pair = HashMapNext(table); pair != NULL; pair = HashMapNext(table)) {
                        Expression *expression = pair->value;
                        //go through all the choices to find if we have the same expression
                        switch (currNode->inst->Opcode) {
                            case Add:{
                                if((expression->op == currNode->inst->Opcode)  &&
                                   ((expression->rhsValueNumber == RhsNumber && expression->lhsValueNumber == LhsNumber)
                                    || expression->rhsValueNumber == LhsNumber && expression->lhsValueNumber == RhsNumber)){
                                    //
                                    replace = pair->key;

                                }
                                break;
                            }
                            case Sub:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                }
                                break;
                            }
                            case Mod:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                }
                                break;
                            }
                            case Div:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                }
                                break;
                            }
                            case Mul:{
                                if((expression->op == currNode->inst->Opcode)  &&
                                   ((expression->rhsValueNumber == RhsNumber && expression->lhsValueNumber == LhsNumber)
                                    || expression->rhsValueNumber == LhsNumber && expression->lhsValueNumber == RhsNumber)){

                                    //
                                    replace = pair->key;

                                }
                                break;
                            }
                            default:{
                                assert(false);
                                break;
                            }
                        }
                    }

                    //break 到这里
                    if(replace != NULL){
                        //

                    }else{
                        Expression *newExpression =  (Expression *)malloc(sizeof(Expression));

                    }

                }
            }
        }else{
            //DO NOT MOV
            currNode = get_next_inst(currNode);
        }
    }



    //for each successor s for b adjust the phi function inputs in S


    //for each child c of B in the dominator tree
    //不存在回边不需要额外考虑
    struct _DomNode *domTreeNode = block->domTreeNode;
    HashSetFirst(domTreeNode->children);
    for(DomTreeNode *child = HashSetNext(domTreeNode->children); child != NULL; child = HashSetNext(domTreeNode->children)){
        changed |= DVNT_EACH(child->block,table,var2num,currentFunction);
    }

    //deallocate the scope for B
    HashSetFirst(newScope);
    for(Value *pushed = HashSetNext(newScope); pushed != NULL; pushed = HashSetNext(newScope)){
        HashMapRemove(table,pushed);
    }
    return changed;
}