#include "graph_color.h"

//int tmp_num = 0;

//低度数的传送无关结点,元素为Node
HashSet *simplifyWorklist;
//有可能合并的传送指令,元素为MachineMove
HashSet *worklistMoves;
//低度数的传送有关的结点
HashSet *freezeWorklist;
//高度数的结点表
HashSet* spillWorklist;
//在本轮要被溢出的结点集合，初始为空
HashSet *spilledNodes;

//一个包含从图中删除的临时变量的栈
stack* selectStack;
//lsy 从图中删除的临时变量的set,没什么别的意思，方便确认stack中的元素
HashSet *selectSet;

//已合并的寄存器集合
HashSet * coalescedNodes;

//已经合并的传送指令集合
HashSet *coalescedMoves;
//src与dst相冲突的传送指令集合
HashSet *constrainedMoves;
//不再考虑合并的传送指令集合
HashSet *frozenMoves;
//还未做好合并准备的传送指令的集合
HashSet *activeMoves;

//图中冲突边的集合
HashSet *adjSet;

//lsy
HashSet *nodeSet;
HashMap *colorMap;

//获取活跃区间
PriorityQueue *interval_queue;
HashMap *intervalMap;

//lsy
HashSet* restore_spillNodes;

bool type_alloca(int type){
    if(type == Alloca || type == bitcast || type == MEMSET || type == MEMCPY)
        return true;
    return false;
}

//false就是不用分配寄存器
bool value_type(Value* uvalue){
    if(isImmIntType(uvalue->VTy) || isImmFloatType(uvalue->VTy)){
        return false;
    }
//    if(isGlobalArrayFloatType(uvalue->VTy)|| isGlobalArrayIntType(uvalue->VTy) || isGlobalVarFloatType(uvalue->VTy) ||
//       isGlobalVarIntType(uvalue->VTy)){
//        return false;
//    }
    if(isLocalArrayFloatType(uvalue->VTy) || isLocalArrayIntType(uvalue->VTy)){
        return false;
    }

//    if(uvalue->VTy->ID == tmp_mem)
//        return false;
    return true;
}

bool check_spilled(Value* value){
    HashSetFirst(restore_spillNodes);
    for(Node* node = HashSetNext(restore_spillNodes); node!=NULL; node = HashSetNext(restore_spillNodes)){
        if(node->value == value)
            return true;
    }
    return false;
}

//看当前溢出的有没有对应这个value的Node
Node *spill_contain_node(Value* value){
    HashSetFirst(restore_spillNodes);
    for(Node* node = HashSetNext(restore_spillNodes);node!=NULL; node = HashSetNext(restore_spillNodes)){
        if(node->value == value)
            return node;
    }
    return NULL;
}

//寻找当前nodeSet有没有对应Value的Node,如果有则返回，没有则新建
Node *get_Node_with_value(Value* value){
    //应该走不到这里来吧
//    assert(!isLocalVarFloat(value));
//    assert(!isImmFloatType(value->VTy));
//    assert(!isImmIntType(value->VTy));
//      assert(!isLocalArrayIntType(value->VTy));

    HashSetFirst(nodeSet);
    for(Node* node = HashSetNext(nodeSet);node!=NULL; node = HashSetNext(nodeSet)){
        if(node->value == value)
            return node;
    }
    //需要新建
    Node *node = (Node*) malloc(sizeof (Node));
    node->value = value;
    node->adjOpdSet = HashSetInit();
    node->moveSet = HashSetInit();
    node->type = USUAL;
    node->loopCounter = 0;
    node->degree = 0;
    HashSetAdd(nodeSet,node);
    return node;
}

void init(){
    simplifyWorklist = HashSetInit();
    worklistMoves = HashSetInit();
    freezeWorklist = HashSetInit();
    spillWorklist = HashSetInit();
    spilledNodes = HashSetInit();
    selectStack = stackInit();
    selectSet = HashSetInit();
    coalescedNodes = HashSetInit();
    coalescedMoves = HashSetInit();
    constrainedMoves = HashSetInit();
    frozenMoves = HashSetInit();
    activeMoves = HashSetInit();
    adjSet = HashSetInit();
    nodeSet = HashSetInit();
    colorMap = HashMapInit();
    interval_queue = PriorityQueueInit();
    intervalMap = HashMapInit();
}

void deinit(){
    HashSetDeinit(simplifyWorklist);
    HashSetDeinit(worklistMoves);
    HashSetDeinit(freezeWorklist);
    HashSetDeinit(spillWorklist);
    HashSetDeinit(spilledNodes);
    stackDeinit(selectStack);
    HashSetDeinit(selectSet);
    HashSetDeinit(coalescedNodes);
    HashSetDeinit(coalescedMoves);
    HashSetDeinit(constrainedMoves);
    HashSetDeinit(frozenMoves);
    HashSetDeinit(activeMoves);
    HashSetDeinit(adjSet);
    HashSetDeinit(nodeSet);
    HashMapDeinit(colorMap);
    PriorityQueueDeinit(interval_queue);
    HashMapDeinit(intervalMap);
}

bool find_pair(Node* u,Node* v){
//    HashSetFirst(adjSet);
//    for(AdjPair* adjPair = HashSetNext(adjSet); adjPair!=NULL; adjPair = HashSetNext(adjSet)){
//        if((adjPair->u == u && adjPair->v == v) || (adjPair->v == u && adjPair->u == v))
//            return true;
//    }
//    return false;

//CHECK: 应该只看一边就行了
    if(HashSetFind(u->adjOpdSet,v))
        return true;
    return false;
}

void addEdge(Node* u, Node* v){
    if(!(find_pair(u,v) || u == v)){
        AdjPair *adjPair = (AdjPair*) malloc(sizeof (AdjPair));
        adjPair->u = u;
        adjPair->v = v;
        HashSetAdd(adjSet, adjPair);

        AdjPair *adj_pair = (AdjPair*) malloc(sizeof (AdjPair));
        adj_pair->u = v;
        adj_pair->v = u;
        HashSetAdd(adjSet,adj_pair);

        //我们应该都没有这种type
        if(u->type != PreColored){
            u->degree++;
            HashSetAdd(u->adjOpdSet,v);
//            }
        }
        if(v->type != PreColored){
            v->degree++;
            HashSetAdd(v->adjOpdSet,u);
        }
    }
}

void handle_def_(HashSet* live, Value* def, int loopDepth,InstNode* cur_node){
    if(check_spilled(def) || isLocalVarFloat(def))
        return;

    if(isLocalArrayFloatType(def->VTy) || isLocalArrayIntType(def->VTy))
        return;

    //如果是无用左值，不加入
    ///要把copyOperation排除
    if(def->use_list == NULL && cur_node->inst->Opcode!= CopyOperation)
        return;

    Node *node = get_Node_with_value(def);
    node->loopCounter = loopDepth;

    //if(node->type == PreColored || node->type == Virtual){
    if(node->type == USUAL){
        if(!HashSetFind(live,def))
            HashSetAdd(live,def);
        HashSetFirst(live);
        for(Value * v = HashSetNext(live); v!=NULL; v = HashSetNext(live)){
            Node *n = get_Node_with_value(v);

            addEdge(node,n);
        }
    }

    HashSetRemove(live, def);
}

void handle_use_(HashSet* live,Value* uvalue, int loopDepth,Function* cur_func){

    if(isImmIntType(uvalue->VTy) || isImmFloatType(uvalue->VTy) || isLocalVarFloat(uvalue)){
        return;
    }
//    if(isGlobalArrayFloatType(uvalue->VTy)|| isGlobalArrayIntType(uvalue->VTy) || isGlobalVarFloatType(uvalue->VTy) ||
//       isGlobalVarIntType(uvalue->VTy)){
//        return;
//    }
    if(isLocalArrayFloatType(uvalue->VTy) || isLocalArrayIntType(uvalue->VTy)){
        return;
    }

//    if(uvalue->VTy->ID == tmp_mem)
//        return;

    if(check_spilled(uvalue))
        return;

    Node *node = get_Node_with_value(uvalue);
    node->loopCounter = loopDepth;
//    if(node->type == PreColored || node->type == Virtual){
    if(node->type == USUAL && !HashSetFind(live,uvalue)){
        HashSetAdd(live,uvalue);

        //如果是参数，要马上与目前的live建立冲突关系, 因为没有def
        //8.13 全局变量也分配寄存器的话, 全局变量也没有def
        if(get_name_index(node->value) < ins_get_lhs(cur_func->entry->head_node->inst)->pdata->symtab_func_pdata.param_num ||
                isGlobalType(uvalue->VTy)){
            HashSetFirst(live);
            for(Value * v = HashSetNext(live); v!=NULL; v = HashSetNext(live)){
                Node *n = get_Node_with_value(v);

                addEdge(node,n);
            }
        }
    }
}

void addEdge_within_live(HashSet* live){
    HashSet *tmp_store = HashSetInit();

    HashSetFirst(live);
    for(Value* v = HashSetNext(live); v!=NULL; v = HashSetNext(live)){
        if(check_spilled(v) || isLocalVarFloat(v))
            continue;

        Node *new_node = get_Node_with_value(v);

        HashSetFirst(tmp_store);
        for(Node* node = HashSetNext(tmp_store); node!=NULL ;node = HashSetNext(tmp_store)){
            addEdge(node,new_node);
        }

        HashSetAdd(tmp_store, new_node);
    }
    HashSetDeinit(tmp_store);
}

void dealSDefUse(HashSet* live, InstNode* ins, BasicBlock* cur_block){
    Value *value0,*value1,*value2;
    int opNum ;
    int loopDepth = cur_block->domTreeNode->loopNest;
//    if(HashSetSize(live) > 140)
//        printf("j");
    switch (ins->inst->Opcode) {
        //def = 1, use = 2
        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def_(live,value0,loopDepth,ins);
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            handle_use_(live,value2,loopDepth,cur_block->Parent);
            break;
        case Call:
            if(!returnValueNotUsed(ins)){
                value0=&ins->inst->user.value;
                handle_def_(live,value0,loopDepth,ins);
            }
            break;
        case Return:
            opNum=ins->inst->user.value.NumUserOperands;
            if(opNum!=0){
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                handle_use_(live,value1,loopDepth,cur_block->Parent);
            }
            break;
        case Store:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            handle_use_(live,value2,loopDepth,cur_block->Parent);
            break;
        case Load:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def_(live,value0,loopDepth,ins);
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            break;
        case GIVE_PARAM:
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            break;
        case LESS:
        case GREAT:
        case LESSEQ:
        case GREATEQ:
        case EQ:
        case NOTEQ:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def_(live,value0,loopDepth,ins);
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            handle_use_(live,value2,loopDepth,cur_block->Parent);
            break;
        case br_i1:
            //各个out变量之间需要建边
            addEdge_within_live(live);
            value1=user_get_operand_use(&ins->inst->user,0)->Val; //真值
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            break;
        case br:
        case FunEnd:
            //各个out变量之间需要建边
            addEdge_within_live(live);
            break;
//        case Label:
//            break;
        case XOR:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def_(live,value0,loopDepth,ins);
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            break;
        case GEP:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            value2= user_get_operand_use(&ins->inst->user,1)->Val;
            handle_def_(live,value0,loopDepth,ins);
            //TODO address处理
            if(isGlobalArray(value1) || isAddress(value1)){
                handle_use_(live,value1,loopDepth,cur_block->Parent);
            }
            handle_use_(live,value2,loopDepth,cur_block->Parent);
            break;
        case CopyOperation:
            value0=ins->inst->user.value.alias;
            value1= user_get_operand_use(&ins->inst->user,0)->Val;
            handle_def_(live,value0,loopDepth,ins);
            handle_use_(live,value1,loopDepth,cur_block->Parent);
            break;
        case fptosi:
        case sitofp:
            value0=&ins->inst->user.value;
            value1=user_get_operand_use(&ins->inst->user,0)->Val;
            if(value0->name!=NULL && value1->name!=NULL){
                handle_def_(live,value0,loopDepth,ins);
                handle_use_(live,value1,loopDepth,cur_block->Parent);
            }
            break;
        default:
            break;
    }

}

//使用liveness的结果来构造冲突图和位矩阵。并且初始化worklistMoves，使之包含程序中所有传送指令
void build(Function* func){
    ///1. 遍历程序中的基本块
    InstNode *currNode = get_next_inst(func->entry->head_node);
    clear_visited_flag(currNode->inst->Parent);

    while(currNode != func->tail->tail_node){
        BasicBlock *currNodeParent = currNode->inst->Parent;
        if(currNodeParent->visited == false){
            //!!! 使用copy的out, 不然不能连跑多轮!!
            HashSet *live = HashSetInit();
            HashSetFirst(currNodeParent->out);
            for(Value* v_live = HashSetNext(currNodeParent->out); v_live!=NULL; v_live = HashSetNext(currNodeParent->out)){
                if(!check_spilled(v_live) && !isLocalVarFloat(v_live))
                    HashSetAdd(live,v_live);
            }
           // HashSet *live = currNodeParent->out;

            //进到了一个block的第一条ir,一定是label
            InstNode *first_node = currNodeParent->head_node;
            InstNode *curr_node = currNodeParent->tail_node;
            /// 2. 逆序遍历ir
            while (curr_node != first_node){
                if(curr_node->inst->Opcode == CopyOperation && ins_get_lhs(curr_node->inst)->name!=NULL){
                    //为什么要live <---live\use(I)
                    //mov dst,src不应是直接冲突关系，而是潜在可合并关系
                    value_live_range *range = HashMapGet(intervalMap, ins_get_lhs(curr_node->inst));
                    //如果range为空就证明这是一个已经溢出的结点 emmm好像也不是, 但是range!=NULL还是留下吧
                    if(range && (ins_get_lhs(curr_node->inst)->VTy->ID == Int || ins_get_lhs(curr_node->inst)->VTy->ID == Float || curr_node->inst->i == range->end) &&
                      !check_spilled(ins_get_dest(curr_node->inst)->alias) && !check_spilled(ins_get_lhs(curr_node->inst)) &&
                      !isLocalVarFloat(ins_get_lhs(curr_node->inst)) && !isLocalVarFloat(ins_get_dest(curr_node->inst)->alias)){

                        //lsy, 比书上加一个, 如果已经有冲突关系，就不加入了
                        //其实不用这样，后面constrainedMoves应该能解决了
                        Node *src = get_Node_with_value(ins_get_lhs(curr_node->inst));
                        Node *dst = get_Node_with_value(ins_get_dest(curr_node->inst)->alias);
//                        if(!HashSetFind(src->adjOpdSet,dst)){
                            HashSetRemove(live, ins_get_lhs(curr_node->inst));
                            MachineMove *mv = (MachineMove*) malloc(sizeof (MachineMove));

                            mv->src = src;
                            mv->dst = dst;
                            HashSetAdd(mv->src->moveSet,mv);
                            HashSetAdd(mv->dst->moveSet,mv);
                            HashSetAdd(worklistMoves,mv);
//                        }
                    }
                }
                dealSDefUse(live,curr_node,currNodeParent);
                curr_node = get_prev_inst(curr_node);
            }

            currNodeParent->visited = true;
        }
        currNode = get_next_inst(currNode);
    }
}

//x.vMovSet ∩ (activeVMovSet ∪ workListVMovSet)
HashSet *MoveRelated(Node* node){
    HashSet *canCoalesceSet = HashSetInit();
    HashSetFirst(node->moveSet);
    for(MachineMove *move= HashSetNext(node->moveSet); move!=NULL; move = HashSetNext(node->moveSet)){
        if(HashSetFind(activeMoves,move) || HashSetFind(worklistMoves,move))
            HashSetAdd(canCoalesceSet,move);
    }

    return canCoalesceSet;
}

//获取有效冲突
// x.adjOpdSet \ (selectStack u coalescedNodeSet)
//     * 对于o, 除在selectStackList(冲突图中已删除的结点list), 和已合并的mov的src(dst在其他工作表中)
HashSet *adjacent(Node* node){
    HashSet *validConflictOpdSet = HashSetInit();
    HashSetFirst(node->adjOpdSet);
    for(Node* n = HashSetNext(node->adjOpdSet); n!=NULL; n = HashSetNext(node->adjOpdSet)){
        if(!HashSetFind(selectSet,n) && !HashSetFind(coalescedNodes,n))
            HashSetAdd(validConflictOpdSet,n);
    }
    return validConflictOpdSet;
}

void decrementDegree(Node* adj){
    adj->degree--;
  //  printf("--degree : %s: %d\n",adj->value->name,adj->degree);
    if(adj->degree == K-1){
        //EnableMoves
        HashSet *set = MoveRelated(adj);
        HashSetFirst(set);
        for(MachineMove* move = HashSetNext(set); move!=NULL; move = HashSetNext(set)){
            if(HashSetFind(activeMoves,move)){
                // 未做好合并准备的集合如果包含mv, 就挪到workListVMovSet中
                HashSetRemove(activeMoves,move);
                HashSetAdd(worklistMoves,move);
            }
        }
        HashSet *adj_set = adjacent(adj);
        // 对于每个实际邻接冲突adj
        HashSetFirst(adj_set);
        for(Node* n = HashSetNext(adj_set); n!=NULL; n = HashSetNext(adj_set)){
            HashSet *move_set = MoveRelated(n);
            HashSetFirst(move_set);
            for(MachineMove* move = HashSetNext(move_set); move!=NULL; move = HashSetNext(move_set)){
                if(HashSetFind(activeMoves,move)){
                    // 未做好合并准备的集合如果包含mv, 就挪到workListVMovSet中
                    HashSetRemove(activeMoves,move);
                    HashSetAdd(worklistMoves,move);
                }
            }
        }
        //TODO 书上是remove,参考的北航代码是add??
        HashSetRemove(spillWorklist,adj);
        if(HashSetSize(MoveRelated(adj)) > 0)
            HashSetAdd(freezeWorklist,adj);
        else
            HashSetAdd(simplifyWorklist,adj);
    }
}

int get_usage(Node* node){
    int usage = 0;
    Use * use = node->value->use_list;
    while (use != NULL){
        usage++;
        use = use->Next;
    }
    return usage;
}

Node *selectSimplify(){
    HashSetFirst(simplifyWorklist);
    Node *choice = HashSetNext(simplifyWorklist);
    int degree = choice->degree;
    for(Node* node = HashSetNext(simplifyWorklist); node!=NULL; node = HashSetNext(simplifyWorklist)){
        int h = node->degree;
        if(h > degree){
            degree = h;
            choice = node;
        } else if(h == degree){
            //相等则计算usage
            if(get_usage(node) > get_usage(choice)){
                choice = node;
            }
        }
    }
    return choice;
}

void simplify(){
    // 从度数低的结点集中随机选择一个从图中删除放到 selectStack 里
    HashSetFirst(simplifyWorklist);
    Node *node = HashSetNext(simplifyWorklist);
    //改成选择一个度数最低的放
   // Node *node = selectSimplify();

    HashSetRemove(simplifyWorklist,node);
    stackPush(selectStack,node);
    HashSetAdd(selectSet,node);

    HashSetFirst(node->adjOpdSet);
    for(Node* adj = HashSetNext(node->adjOpdSet); adj!=NULL; adj = HashSetNext(node->adjOpdSet)){
        // 对于 x 的邻接冲突结点adj, 如果不是已经被删除的或者已合并的
        if(!(HashSetFind(selectSet,adj) || HashSetFind(coalescedNodes,adj)))
            decrementDegree(adj);
    }
}

//当 x 需要被染色, x 并不与 move 相关, x 的度 <= k - 1
//低度数传送有关结点集 freezeWorkSet 删除 x , 且低度数传送无关结点集 simplifyWorkSet 添加 x
void addWorkList(Node* node){
    if(node->type!=PreColored && HashSetSize(MoveRelated(node)) == 0 && node->degree < K){
        HashSetRemove(freezeWorklist,node);
        HashSetAdd(simplifyWorklist,node);
    }
}

//TODO 是什么意思呢
//合并预着色寄存器的启发式函数
bool OK(Node* t,Node* r){
    return t->degree < K || t->type == PreColored || find_pair(t,r);
}

bool adjOK(Node* v,Node* u){
    HashSet *set = adjacent(v);
    HashSetFirst(set);
    for(Node* node = HashSetNext(set); node!=NULL; node = HashSetNext(set)){
        if(!OK(node,u))
            return false;
    }
    return true;
}

Node *getAlias(Node* node){
    while (HashSetFind(coalescedNodes, node))
        node = node->alias;
    return node;
}

bool conservative(HashSet* adjacent1, HashSet* adjacent2){
    //合并两个set
    HashSet *set = HashSetInit();
    HashSetFirst(adjacent1);
    for(Node* node = HashSetNext(adjacent1); node!=NULL; node = HashSetNext(adjacent1)){
        HashSetAdd(set,node);
    }
    HashSetFirst(adjacent2);
    for(Node* node = HashSetNext(adjacent2); node!=NULL; node = HashSetNext(adjacent2)){
        if(!HashSetFind(set,node))
            HashSetAdd(set,node);
    }

    int cnt = 0;
    HashSetFirst(set);
    for(Node* node = HashSetNext(set); node!=NULL; node = HashSetNext(set)){
        if(node->degree >= K)
            cnt++;
    }
    return cnt < K;
}

//合并move u <- v
//1. u 预着色, v 是虚拟寄存器, 且 v 的冲突邻接点均满足: 要么为低度数结点, 要么预着色, 要么已经与 u 邻接
//2. u, v 都不是预着色, 且两者的邻接冲突结点的高结点个数加起来也不超过 K - 1 个
void combine_(Node* u,Node* v){
    if(HashSetFind(freezeWorklist,v))
        HashSetRemove(freezeWorklist,v);
    else
        HashSetRemove(spillWorklist,v);
    // 合并 move u, v, 将v加入 coalescedNodeSet
    HashSetAdd(coalescedNodes,v);
    v->alias = u;
    //u.movSet.addAll(v.movSet)
    HashSetFirst(v->moveSet);
    for(MachineMove* move = HashSetNext(v->moveSet); move!=NULL; move = HashSetNext(v->moveSet)){
        if(!HashSetFind(u->moveSet,move))
            HashSetAdd(u->moveSet,move);
    }
    //TODO enableMoves怎么说，感觉怪怪的
    HashSetFirst(v->adjOpdSet);
    for(Node* node = HashSetNext(v->adjOpdSet); node!=NULL; node = HashSetNext(v->adjOpdSet)){
        if(!HashSetFind(selectSet,node) && !HashSetFind(coalescedNodes,node)){
            addEdge(node,u);
            decrementDegree(node);
        }
    }
    if(u->degree >= K && HashSetFind(freezeWorklist,u)){
        HashSetRemove(freezeWorklist,u);
        HashSetAdd(spillWorklist,u);
    }
}

//合并阶段只考虑worklistMoves中的传送指令
void coalesce(){
    //随便取出一个move
    HashSetFirst(worklistMoves);
    MachineMove *m = HashSetNext(worklistMoves);
    //u <--- v
    Node *u = getAlias(m->dst);
    Node *v = getAlias(m->src);
    if(v->type == PreColored){              //TODO 我们好像根本不存在预着色了的情况
        // 冲突图是无向图, 这里避免把m归到受限一类而尽可能让v不是预着色的
        Node *tmp = u;
        u = v;
        v = tmp;
    }
    HashSetRemove(worklistMoves,m);
    if(u == v){
        HashSetAdd(coalescedMoves,m);
        addWorkList(u);
    } else if(v->type == PreColored || find_pair(u,v)){
        // 两边都是预着色则不可能合并
        HashSetAdd(constrainedMoves,m);
        addWorkList(u);
        addWorkList(v);
    } else if((u->type == PreColored && adjOK(v,u))
             || (u->type!=PreColored && conservative(adjacent(u), adjacent(v)))){
        HashSetAdd(coalescedMoves,m);
        combine_(u,v);
        addWorkList(u);
    } else
        HashSetAdd(activeMoves,m);
}

void freezeMoves(Node* node){
    HashSet *set = MoveRelated(node);
    HashSetFirst(set);
    for(MachineMove *move= HashSetNext(set); move!=NULL; move = HashSetNext(set)){
        Node *v;
        if(getAlias(move->src) == getAlias(node))
            v = getAlias(move->dst);
        else
            v = getAlias(move->src);

        if(HashSetFind(activeMoves,move))
            HashSetRemove(activeMoves,move);
        else
            HashSetRemove(worklistMoves,move);

        HashSetAdd(frozenMoves,move);
        // 如果 v 不是传送相关的低度数结点, 则将 v 从低度数传送有关结点集 freezeWorkSet 挪到低度数传送无关结点集 simplifyWorkSet
        if(HashSetSize(MoveRelated(v)) == 0 && v->degree < K){
            HashSetRemove(freezeWorklist,v);
            HashSetAdd(simplifyWorklist,v);
        }
    }
}

//启发式溢出结点
double heuristicVal(Node* node){
    double num = (node->degree << 10) / pow(_HEURISTIC_BASE,node->loopCounter);
    if(isGlobalType(node->value->VTy))
        num = num * 1.52;
    return num;
}

//冻结的启发式函数
double customHeuristic(Node* node) {
    int usage = get_usage(node);

    double loopCounterWeight = 1.0;  // 可以根据需要调整权重
    double degreeWeight = 1.45;  // 可以根据需要调整权重
    double usageWeight = 1.0;  // 可以根据需要调整权重

    double heuristicVal = (pow(node->loopCounter, loopCounterWeight) * pow(node->degree, degreeWeight))
                          * pow(usage, usageWeight);

    return heuristicVal;
}

Node *selectFroze(){
    HashSetFirst(freezeWorklist);
    Node *choice = HashSetNext(freezeWorklist);
    double max = customHeuristic(choice);
    for(Node* node = HashSetNext(freezeWorklist); node!=NULL; node = HashSetNext(freezeWorklist)){
        double h = customHeuristic(node);
        if(h > max){
            max = h;
            choice = node;
        }
    }
    return choice;
}

void freeze(){
    Node *node = selectFroze();
    HashSetRemove(freezeWorklist,node);
    HashSetAdd(simplifyWorklist,node);
    freezeMoves(node);
}

HashMap *cal_interval_len(PriorityQueue* queue){
    HashMap *lenMap = HashMapInit();
    while (PriorityQueueSize(queue) > 0){
        value_live_range *range;
        PriorityQueueTop(queue,(void**)&range);
        PriorityQueuePop(queue);

        if(isLocalVarFloat(range->value) || spill_contain_node(range->value))
            continue;
        int *len = malloc(4);
        *len= range->end - range->start;
        HashMapPut(lenMap, get_Node_with_value(range->value),len);
    }
    return lenMap;
}

void get_interval_set(PriorityQueue* queue){
    while (PriorityQueueSize(queue) > 0){
        value_live_range *range;
        PriorityQueueTop(queue,(void**)&range);
        PriorityQueuePop(queue);
        HashMapPut(intervalMap, range->value,range);
    }
}

void selectSpill(Function* cur_func){
    interval_queue = PriorityQueueInit();
    build_live_interval(cur_func->ToPoBlocks,interval_queue);
    HashMap *lenMap = cal_interval_len(interval_queue);

    //从高度数结点集(spillWorkSet)中启发式选取结点 x , 挪到低度数结点集(simplifyWorkSet)中
    // 冻结 x 及其相关 move
    HashSetFirst(spillWorklist);
    Node *n = HashSetNext(spillWorklist);
    double max = heuristicVal(n);
//    if(n->type == Virtual) {
    if(n->type == USUAL) {
        int *len = HashMapGet(lenMap,n);
        //如果全局能不溢出尽量不溢出, 可能有点问题
        if(*len<5)
            max = 0;
    }
    for(Node* node = HashSetNext(spillWorklist); node!=NULL; node = HashSetNext(spillWorklist)){
        double h = heuristicVal(node);

        if(node->type == USUAL) {
            int *len = HashMapGet(lenMap,node);
            if(*len<5)
                h = 0;
        }

        if(h > max){
            n = node;
            max = h;
        }
    }

    HashSetAdd(simplifyWorklist,n);
    freezeMoves(n);
    HashSetRemove(spillWorklist,n);
}

void regAllocIteration(Function *func){
    while(HashSetSize(simplifyWorklist) + HashSetSize(worklistMoves) + HashSetSize(freezeWorklist) + HashSetSize(spillWorklist) > 0 ){
        if(HashSetSize(simplifyWorklist) > 0)
            simplify();
        else if(HashSetSize(worklistMoves) > 0)
            coalesce();
        else if(HashSetSize(freezeWorklist) > 0)
            freeze();
        else if(HashSetSize(spillWorklist) > 0)
            selectSpill(func);
    }
}

bool hasReg(Node* node){
    return node->type == Allocated || node->type == PreColored;
}

void preAssignColors(){
    printf("begin\n");
    while(stackSize(selectStack) > 0){
        Node *toBeColored;
        stackTop(selectStack,(void**)&toBeColored);
        stackPop(selectStack);
        assert(toBeColored->type!=PreColored && toBeColored->type!=Allocated);

        for(int i=3;i<=12;i++){
            myreg[i] = 0;
        }
        myreg[14] = 0;

        // 把待分配颜色的结点的邻接结点的颜色去除
        //TODO 好像没有必要调整alloca的type
        HashSetFirst(toBeColored->adjOpdSet);
        for(Node* node = HashSetNext(toBeColored->adjOpdSet); node!=NULL; node = HashSetNext(toBeColored->adjOpdSet)){
            Node *a = getAlias(node);
            if(hasReg(a)){
                myreg[a->reg] = 1;
                //HashMapRemove(colorMap,a);
//            } else if(a->type == Virtual){
            } else if(a->type == USUAL){
                int *reg;
                reg = HashMapGet(colorMap,a);
                if(reg != NULL){
                    myreg[*reg] = 1;
                   // HashMapRemove(colorMap,a);
                }
            }
        }

        value_register *reg = (value_register*) malloc(sizeof (value_register));
        reg->reg = get_an_availabel_register();
        if(reg->reg == -1){
            HashSetAdd(spilledNodes,toBeColored);
            printf("spilled %s\n",toBeColored->value->name);
        }
        else {
            toBeColored->reg = reg->reg;
            HashMapPut(colorMap,toBeColored,reg);
        }
    }
}

void assignColors(){
    preAssignColors();

    if(HashSetSize(spilledNodes) > 0)
        return;

    HashSetFirst(coalescedNodes);
    for(Node* node = HashSetNext(coalescedNodes); node!=NULL; node = HashSetNext(coalescedNodes)){
        Node *a = getAlias(node);
        assert(a->type != Allocated);                    //TODO 为什么要这样
//        if(a->type == PreColored)
//            HashMapPut(colorMap,node,a);            //TODO preColored的寄存器没给，有这种情况吗
//        else
            HashMapPut(colorMap,node, HashMapGet(colorMap,a));
    }
}

InstNode *get_node_by_ins(Instruction* ins, InstNode* begin){
    while (begin != NULL){
        if(ins_get_dest(begin->inst) == ins_get_dest(ins))
            return begin;
        begin = get_next_inst(begin);
    }
    return NULL;
}

//直接在ir上加load和store的方法
//void rewriteProgram(Function* func){
//    //遍历每个block
//    InstNode *cur_node = func->entry->head_node;
//    InstNode *last_node = func->tail->tail_node;
//    while(cur_node != last_node){
//        Value *def = ins_get_lhs(cur_node->inst);
//        if(HashSetFind(spilledNodes, get_Node_with_value(def))){
//            //即如果一条指令的def是溢出结点
//            //新建一个结点
//            //创建一条store语句
//            //创造一个内存地址
//            Value *mem = (Value*) malloc(sizeof (Value));
//            mem->VTy->ID = tmp_mem;
//            mem->name = (char*) malloc(4);
//            mem->alias = def;
//            strcpy(mem->name,"mem");
//            Instruction *ins_store = ins_new_binary_operator(Store,def,mem);
//            InstNode *node_store = new_inst_node(ins_store);
//            ins_insert_after(node_store,cur_node);
//
//            //处理这个结点的所有use, 每个use前加一句load
//            Use* use=def->use_list;
//            while(use != NULL){
//                Instruction *ins_load = ins_new_unary_operator(Load, mem);
//                Value *new_left = ins_get_value_with_name_and_index(ins_load,tmp_num++);
//                InstNode *node_load = new_inst_node(ins_load);
//                Instruction *ins = (Instruction*)use->Parent;
//                ins_insert_before(node_load, get_node_by_ins(ins,cur_node));
//
//                Use *use_store = use;
//                use = use->Next;
//                use_set_value(use_store,new_left);
//            }
//        }
//
//        cur_node = get_next_inst(cur_node);
//    }
//
//
//}

//标记的做法
void rewriteLabel(Function* func){
    //删去溢出结点,再进行一次图着色
    HashSetFirst(spilledNodes);
    for(Node* node = HashSetNext(spilledNodes); node!=NULL; node = HashSetNext(spilledNodes)){

        HashSetAdd(restore_spillNodes, node);
    }

    //遍历每个block
    InstNode *cur_node = get_next_inst(func->entry->head_node);
    InstNode *last_node = func->tail->tail_node;
    while(cur_node != last_node){
        Value *v,*vl,*vr;
        v= ins_get_dest(cur_node->inst);
        vl= ins_get_lhs(cur_node->inst);
        vr= ins_get_rhs(cur_node->inst);
        if(v!=NULL && !isLocalVarFloat(v)  && (cur_node->inst->Opcode!=CopyOperation || (cur_node->inst->Opcode == CopyOperation && !isLocalVarFloat(v->alias)))) {
            if(HashSetFind(spilledNodes, get_Node_with_value(v)) || (cur_node->inst->Opcode == CopyOperation &&
                    HashSetFind(spilledNodes, get_Node_with_value(v->alias)))){
                cur_node->inst->_reg_[0] = -1;
            }
        }

        if(vl!=NULL && !isLocalVarFloat(vl)){
            if(HashSetFind(spilledNodes, get_Node_with_value(vl))){
                cur_node->inst->_reg_[1] = 101;
            }
        }
        if(vr!=NULL && !isLocalVarFloat(vr)){
            if(HashSetFind(spilledNodes, get_Node_with_value(vr))){
                cur_node->inst->_reg_[2] = 100;
            }
        }


        cur_node = get_next_inst(cur_node);
    }
}

void labelRegister(Function* func){
    //遍历每个block
    InstNode *cur_node = get_next_inst(func->entry->head_node);
    InstNode *last_node = func->tail->tail_node;
    while(cur_node != last_node){
        Value *v,*vl,*vr;
        v= ins_get_dest(cur_node->inst);
        vl= ins_get_lhs(cur_node->inst);
        vr= ins_get_rhs(cur_node->inst);

        if(cur_node->inst->_reg_[0] == 0 && v!=NULL && v->name!=NULL && !type_alloca(cur_node->inst->Opcode) && (cur_node->inst->Opcode!=Call || (cur_node->inst->Opcode == Call && !returnValueNotUsed(cur_node)))){
            int reg ;
            if(cur_node->inst->Opcode == CopyOperation && !isLocalVarFloat(v->alias)){
                reg = ((value_register*)HashMapGet(colorMap, get_Node_with_value(v->alias)))->reg;
                cur_node->inst->_reg_[0] = reg;
            }
            else if(cur_node->inst->Opcode != CopyOperation && !isLocalVarFloat(v) && v->use_list){
                reg = ((value_register*)HashMapGet(colorMap, get_Node_with_value(v)))->reg;
                cur_node->inst->_reg_[0] = reg;
            }
        }
        if(cur_node->inst->_reg_[1] == 0 && vl!=NULL && vl->name!=NULL && !isLocalVarFloat(vl) && vl->VTy->ID!=Int && vl->VTy->ID!=Float &&  cur_node->inst->Opcode != Call && !type_alloca(cur_node->inst->Opcode) &&
                value_type(vl)){
            int reg ;
            reg = ((value_register*)HashMapGet(colorMap, get_Node_with_value(vl)))->reg;
            cur_node->inst->_reg_[1] = reg;
        }
        if(cur_node->inst->_reg_[2] == 0 && vr!=NULL && vr->name!=NULL && !isLocalVarFloat(vr) && vr->VTy->ID!=Int && vr->VTy->ID!=Float &&  !type_alloca(cur_node->inst->Opcode) && cur_node->inst->Opcode != GIVE_PARAM &&
                value_type(vr)){
            int reg;
            reg = ((value_register*)HashMapGet(colorMap, get_Node_with_value(vr)))->reg;
            cur_node->inst->_reg_[2] = reg;
        }

        cur_node = get_next_inst(cur_node);
    }
}

void putInLineScanReg(Function* func){
    HashMapFirst(colorMap);
    for(Pair* p = HashMapNext(colorMap); p!=NULL; p = HashMapNext(colorMap)){
        Node *node = p->key;
        HashMapPut(func->lineScanReg, node->value, p->value);
    }
}

void reg_alloca_(Function *start){
    for(Function *curFunction = start;curFunction!=NULL;curFunction=curFunction->Next){
        restore_spillNodes = HashSetInit();
        curFunction->lineScanReg = HashMapInit();
        while (true){
            init();

            //先获取活跃区间
            build_live_interval(curFunction->ToPoBlocks,interval_queue);
            get_interval_set(interval_queue);

            build(curFunction);

            //makeWorkList
            HashSetFirst(nodeSet);
            for(Node* node = HashSetNext(nodeSet); node!=NULL; node = HashSetNext(nodeSet)){
                if(node->degree >= K){
                    HashSetAdd(spillWorklist,node);              //加入高度数结点表
                } else if(HashSetSize(MoveRelated(node)) > 0){
                    HashSetAdd(freezeWorklist,node);                 //加入低度数传送有关结点
                } else {
                    HashSetAdd(simplifyWorklist,node);            //直接简化
                }
            }

            regAllocIteration(curFunction);
            assignColors();

            if(HashSetSize(spilledNodes) > 0) {
                //rewriteProgram(curFunction);
                rewriteLabel(curFunction);
            } else{
                labelRegister(curFunction);
                putInLineScanReg(curFunction);
                break;
            }
        }
    }
    deinit();
}
