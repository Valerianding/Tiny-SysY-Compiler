//
// Created by Valerian on 2023/5/19.
//

/*
 * a hash map key is home value is the expression
 * a hash map key is variable value is the number
 */

#include "dvnt.h"

const Opcode valueOperator[] = {Load,Call,Add,Sub, Div,Mul,Mod};
unsigned int value_number_seed = 1;
bool isValueAbleOperator(InstNode *instNode){
    int n = sizeof(valueOperator) / sizeof(Opcode);
    for (int i = 0; i < n; ++i) {
        if(instNode->inst->Opcode == valueOperator[i]){
            return true;
        }
    }
    return false;
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

//TODO ERROR
unsigned long getHashValueNumber(Opcode opcode,unsigned int lhsValueNumber, unsigned int rhsValueNumber){
    unsigned long long hash_value = (unsigned long long)(opcode << 32 | lhsValueNumber << 16 | rhsValueNumber);
    unsigned int hash = 5381;
    for(int i = 0; i < 8; i ++){
        hash = ((hash << 5) + hash) + ((unsigned char *)&hash_value)[i];
    }
    return hash;
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
            // TODO 应该不存在redundant 的情况
            HashSetFirst(phiSet);
            bool isMeaningLess = true;
            unsigned int currDefineValueNumber = 0;
            unsigned int prevDefineValueNumber = 0;
            int time = 0;
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

                    if(time != 0 && prevDefineValueNumber != currDefineValueNumber){
                            //代表了不一样了 需要 assgin new value number了
                            isMeaningLess = false;
                            break;
                    }else{
                        time++;
                        prevDefineValueNumber = currDefineValueNumber;
                        //代表还是一样的，我们需要继续比较
                        continue;
                    }
                }
            }


            Value *phiValue = ins_get_dest(phiNode->inst);
            if (isMeaningLess) {
                changed = true;
                //当前的phi Value
                //value number必须是malloc出来的
                unsigned int *pValueNumber = (unsigned int *) malloc(sizeof(unsigned int));
                *pValueNumber = currDefineValueNumber;

                //put it into var2num
                HashMapPut(var2num,phiValue,pValueNumber);


                //TODO 这里需要修改部分
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
                valueReplaceAll(phiValue,replace,currentFunction);


                //理论上来说的

                //remove this instruction!!
                InstNode *tempNode = get_next_inst(phiNode);
                deleteIns(phiNode);
                phiNode = tempNode;
            }else{
                //create a new value number for this phi

                unsigned int *pNewValueNumber = (unsigned int*)malloc(sizeof(unsigned int));
                *pNewValueNumber = value_number_seed;
                value_number_seed++;
                //put it into the var2num table
                HashMapPut(var2num,phiValue,pNewValueNumber);
                phiNode = get_next_inst(phiNode);
            }
        }else{
            phiNode = get_next_inst(phiNode);
        }
    }


    //for each assignment of the form "x <-y op z"
    InstNode *currNode = block->head_node;
    while (currNode != tailNode) {
        if(isValueAbleOperator(currNode)){
            bool useless = false;
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
                case Load:{
                    //如果是load我们也需要分配一个
                    Value *dest = ins_get_dest(currNode->inst);
                    unsigned int *pValue_number = (unsigned int *)malloc(sizeof(unsigned int));
                    *pValue_number = value_number_seed;
                    value_number_seed++;
                    HashMapPut(var2num,dest,pValue_number);
                    break;
                }
                default: {
                    // 进入这里面就只有几个了
                    //其余的情况
                    Value *lhs = ins_get_lhs(currNode->inst);
                    Value *rhs = ins_get_rhs(currNode->inst);

                    InstNode *headNode = currentFunction->entry->head_node;
                    int paramNum = headNode->inst->user.use_list[0].Val->pdata->symtab_func_pdata.param_num;

                    unsigned int *pLhsNumber = NULL;
                    unsigned int *pRhsNumber = NULL;
                    unsigned int LhsNumber;
                    unsigned int RhsNumber;
                    if(lhs != NULL) {
                        if(!isImm(lhs)){
                            //还有可能是参数所以无法取出来，对于参数而言我们也是var_num_seed去存
                            if(isParam(lhs,paramNum)){
                                //assign a new ValueNumber to this
                                unsigned int *pValueNumber = (unsigned int*)malloc(sizeof(unsigned int));
                                *pValueNumber = value_number_seed;
                                value_number_seed++;
                                HashMapPut(var2num,lhs,pValueNumber);
                            }
                            pLhsNumber = HashMapGet(var2num,lhs);
                            assert(pLhsNumber != NULL);
                            LhsNumber = *pLhsNumber;
                        }else{
                            if(isImmInt(lhs)){
                                LhsNumber = (unsigned int)lhs->pdata->var_pdata.iVal;
                            }else if(isImmFloat(lhs)){
                                LhsNumber = (unsigned int)lhs->pdata->var_pdata.fVal;
                            }else{
                                assert(false);
                            }
                        }
                    }

                    if(rhs != NULL){
                        if(!isImm(rhs)){
                            if(isParam(rhs,paramNum)){
                                unsigned int *pValueNumber = (unsigned int*)malloc(sizeof(unsigned int));
                                *pValueNumber = value_number_seed;
                                value_number_seed++;
                                HashMapPut(var2num,rhs,pValueNumber);
                            }
                            pRhsNumber = HashMapGet(var2num, rhs);
                            assert(pRhsNumber != NULL);
                            RhsNumber = *pRhsNumber;
                        }else{
                            if(isImmInt(rhs)){
                                RhsNumber = (unsigned int)rhs->pdata->var_pdata.iVal;
                            }else if(isImmFloat(rhs)){
                                RhsNumber = (unsigned int)rhs->pdata->var_pdata.fVal;
                            }
                        }
                    }

                    //然后去table里面查有没有
                    //table里面
                    Value *replace = NULL;
                    Value *dest = ins_get_dest(currNode->inst);
                    HashMapFirst(table);
                    for (Pair *pair = HashMapNext(table); pair != NULL; pair = HashMapNext(table)) {
                        Expression *expression = pair->value;
                        //go through all the choices to find if we have the same expression
                        switch (currNode->inst->Opcode) {
                            case Add:{
                                if((expression->op == currNode->inst->Opcode)  &&
                                   ((expression->rhsValueNumber == RhsNumber && expression->lhsValueNumber == LhsNumber)
                                    || expression->rhsValueNumber == LhsNumber && expression->lhsValueNumber == RhsNumber)){
                                    replace = pair->key;
                                    if(replace->VTy->ID == dest->VTy->ID){
                                        changed = true;
                                    }else{
                                        replace = NULL;
                                    }
                                }
                                break;
                            }
                            case Sub:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                    if(replace->VTy->ID == dest->VTy->ID){
                                        changed = true;
                                    }else{
                                        replace = NULL;
                                    }
                                }
                                break;
                            }
                            case Mod:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                    if(replace->VTy->ID == dest->VTy->ID){
                                        changed = true;
                                    }else{
                                        replace = NULL;
                                    }
                                }
                                break;
                            }
                            case Div:{
                                if((expression->op == currNode->inst->Opcode) && expression->lhsValueNumber == LhsNumber && expression->rhsValueNumber == RhsNumber){
                                    replace = pair->key;
                                    if(replace->VTy->ID == dest->VTy->ID){
                                        changed = true;
                                    }else{
                                        replace = NULL;
                                    }
                                }
                                break;
                            }
                            case Mul:{
                                if((expression->op == currNode->inst->Opcode)  &&
                                   ((expression->rhsValueNumber == RhsNumber && expression->lhsValueNumber == LhsNumber)
                                    || expression->rhsValueNumber == LhsNumber && expression->lhsValueNumber == RhsNumber)){
                                    replace = pair->key;
                                    if(replace->VTy->ID == dest->VTy->ID){
                                        changed = true;
                                    }else{
                                        replace = NULL;
                                    }
                                }
                                break;
                            }
                            default:{
                                assert(false);
                            }
                        }
                    }

                    //break 到这里
                    if(replace != NULL){
                        useless = true;
                        changed = true;
                        valueReplaceAll(dest,replace,currentFunction);

                        //现在的dest也需要构建一个value number
                        //找到replace的value number

                        //一定不能是一个常数
                        unsigned int  *pReplaceValueNumber = HashMapGet(var2num,replace);
                        assert(pReplaceValueNumber != NULL);
                        unsigned int replaceValueNumber = *pReplaceValueNumber;

                        unsigned int *pDestValueNumber = (unsigned int*) malloc(sizeof(unsigned int));
                        *pDestValueNumber = replaceValueNumber;
                        // var2put
                        HashMapPut(var2num,dest,pDestValueNumber);
                    }else{

                        //不存在，我们需要new hash
                        Expression *newExpression =  (Expression *)malloc(sizeof(Expression));
                        unsigned int hashValueNumber = getHashValueNumber(currNode->inst->Opcode,LhsNumber,RhsNumber);
                        unsigned int *pHashValueNumber = (unsigned int *)malloc(sizeof(unsigned int));
                        *pHashValueNumber = hashValueNumber;
                        HashMapPut(var2num,dest,pHashValueNumber);


                        newExpression->op = currNode->inst->Opcode;
                        newExpression->lhsValueNumber = LhsNumber;
                        newExpression->rhsValueNumber = RhsNumber;
                        printf("Lhs value number %d Rhs value number %d dest value number is %d\n",LhsNumber,RhsNumber,hashValueNumber);
                        HashMapPut(table,dest,newExpression);

                        //记录当前基本块新产生的
                        HashSetAdd(newScope,dest);
                    }
                }
            }
            if(useless) {
                InstNode *tempNode = get_next_inst(currNode);
                deleteIns(currNode);
                currNode = tempNode;
            }else{
                currNode = get_next_inst(currNode);
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
        //先释放内存再清空
        Expression *expression = HashMapGet(table,pushed);
        free(expression);
        expression = NULL;
        HashMapRemove(table,pushed);
    }
    return changed;
}