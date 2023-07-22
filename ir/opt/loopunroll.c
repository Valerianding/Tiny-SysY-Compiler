////
//// 原来那个逻辑有点死，remake
//// Created by 12167 on 2023/7/21.
////
//#include "loopunroll.h"
//int tmp_index = 0;
//
//extern struct _InstNode *instruction_list;
//
////检查init、modifier、end是否都是int常数
//bool check_idc(Value* initValue,Value* step,Value* end)
//{
//    if(initValue->VTy->ID==Int && end->VTy->ID==Int && step->VTy->ID==Int)
//        return true;
//    return false;
//}
//
////计算一次迭代的ir总数
//int cal_ir_cnt(HashSet *loopBody)
//{
//    int ir_cnt=0;
//    for(BasicBlock *body = HashSetNext(loopBody); body != NULL; body = HashSetNext(loopBody)){
//        InstNode *currNode = body->head_node;
//        InstNode *bodyTail = body->tail_node;
//        while(currNode != bodyTail){
//            if(!hasNoDestOperator(currNode)){
//                ir_cnt++;
//            }
//            currNode = get_next_inst(currNode);
//        }
//    }
//    return ir_cnt;
//}
//
////常数情况下计算迭代次数
//int cal_times(int init,Instruction *ins_modifier,Instruction *ins_end_cond,Value* v_step,Value* v_end)
//{
//    int times=0;
//    int step=v_step->pdata->var_pdata.iVal;
//    int end=v_end->pdata->var_pdata.iVal;
//    double val;bool tag;
//    switch (ins_modifier->Opcode) {
//        case Add:
//            switch (ins_end_cond->Opcode) {
//                case EQ: times = (init == end) ? 1 : 0; break;
//                case NOTEQ: times = ((end - init) % step) == 0 ? (end - init / step) : -1; break;
//                case LESSEQ: case GREATEQ:
//                    times = (end - init) / step + 1; break;
//                case LESS: case GREAT:
//                    times = ((end - init) % step == 0)? (end - init) / step : (end - init) / step + 1; break;
//            }
//            break;
//        case Sub:
//            switch (ins_end_cond->Opcode) {
//                case EQ: times = (init == end) ? 1 : 0; break;
//                case NOTEQ: times = ((init - end) % step) == 0 ? (init - end / step) : -1; break;
//                case LESSEQ: case GREATEQ:
//                    times = (init - end) / step + 1; break;
//                case LESS: case GREAT:
//                    times = ((init - end) % step == 0)? (init - end) / step : (init - end) / step + 1; break;
//            }
//            break;
//        case Mul:
//            val = log((double)end / (double)init) / log((double)step);
//            tag = init * pow(step, val) == end;
//            switch (ins_end_cond->Opcode) {
//                case EQ: times = (init == end) ? 1 : 0; break;
//                case NOTEQ: times = tag ? (int)val : -1; break;
//                case LESSEQ: case GREATEQ:
//                    times = (int)val + 1; break;
//                case LESS: case GREAT:
//                    times = tag ? (int) val : (int) val + 1; break;
//            }
//            break;
//        case Div:
//            val = log((double)init / (double)end) / log((double)step);
//            tag = end * pow(step, val) == init;
//            switch (ins_end_cond->Opcode) {
//                case EQ: times = (init == end) ? 1 : 0; break;
//                case NOTEQ: times = tag ? (int)val : -1; break;
//                case LESSEQ: case GREATEQ:
//                    times = (int)val + 1; break;
//                case LESS: case GREAT:
//                    times = tag ? (int) val : (int) val + 1; break;
//            }
//            break;
//    }
//    return times;
//}
//
////如果返回NULL代表确定的一次不走或wrong，返回int类型常数1表示一定走的这种
////TODO 插入位置得改
//InstNode *get_mod(Value* v_init,Instruction *ins_modifier,Instruction *ins_end_cond,Value *v_step,Value *v_end,BasicBlock *block,InstNode* pos){
//    Value *v_mod = (Value*) malloc(sizeof(Value));
//    Value *v_update_modifier = (Value*) malloc(sizeof(Value));
//    value_init_int(v_update_modifier,update_modifier);
//    //1. 计算times   2.times % update_modifier
//    //value_init(v_mod);
//    InstNode *node_sub = NULL,*node_get_times = NULL,*node_get_mod = NULL,*node_plus = NULL;
//    Value *plus = (Value*) malloc(sizeof(Value));
//    InstNode *v_div = NULL;
//    switch (ins_modifier->Opcode) {
//        case Add:
//            switch (ins_end_cond->Opcode) {
//
//                case EQ:
//                    value_init_int(v_mod,1);
//                    v_mod = (v_init == v_end) ? v_mod : NULL;
//                    break;
//                case NOTEQ:  //TODO 我目前就默认没有wrong的情况了
//                    node_sub = auto_binary_node_insert_after(Sub,v_end,v_init,++tmp_index,block,pos);
//                    node_get_times = auto_binary_node_insert_after(Div, ins_get_dest(node_sub->inst),v_step,++tmp_index,block,node_sub);
//                    node_get_mod = auto_binary_node_insert_after(Mod,ins_get_dest(node_get_times->inst),v_update_modifier,++tmp_index,block,node_get_times);
//                    break;
//                case LESSEQ: case GREATEQ:
//                    v_sub = auto_binary_node_insert(Sub,v_end,v_init,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_sub,v_step,++tmp_index,block);
//                    value_init_int(plus,1);
//                    v_plus = auto_binary_node_insert(Add,v_get_times,plus,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_plus,v_update_modifier,++tmp_index,block); break;
//                case LESS: case GREAT:
//                    //TODO 我哭死了，你怎么还要分基本块啊
//                    //times = ((end - init) % step == 0)? (end - init) / step : (end - init) / step + 1;
//                    break;
//            }
//            break;
//        case Sub:
//            switch (ins_end_cond->Opcode) {
//                case EQ: value_init_int(v_mod,1);
//                    v_mod = (v_init == v_end) ? v_mod : NULL; break;
//                case NOTEQ: v_sub = auto_binary_node_insert(Sub,v_init,v_end,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_sub,v_step,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_get_times,v_update_modifier,++tmp_index,block); break;
//                case LESSEQ: case GREATEQ:
//                    v_sub = auto_binary_node_insert(Sub,v_init,v_end,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_sub,v_step,++tmp_index,block);
//                    value_init_int(plus,1);
//                    v_plus = auto_binary_node_insert(Add,v_get_times,plus,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_plus,v_update_modifier,++tmp_index,block); break;
//                case LESS: case GREAT:
//                    //TODO times = ((init - end) % step == 0)? (init - end) / step : (init - end) / step + 1;
//                    break;
//            }
//            break;
//        case Mul:
//            //TODO 不知道要不要注意到小数点
//            //val = log((double)end / (double)init) / log((double)step);
//            //tag = init * pow(step, val) == end;
//            switch (ins_end_cond->Opcode) {
//                case EQ: value_init_int(v_mod,1);
//                    v_mod = (v_init == v_end) ? v_mod : NULL; break;
//                case NOTEQ:
//                    v_div = auto_binary_node_insert(Div,v_end,v_init,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_div,v_step,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_get_times,v_update_modifier,++tmp_index,block); break;
//                case LESSEQ: case GREATEQ:
//                    v_div = auto_binary_node_insert(Div,v_end,v_init,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_div,v_step,++tmp_index,block);
//                    value_init_int(plus,1);
//                    v_plus = auto_binary_node_insert(Add,v_get_times,plus,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_plus,v_update_modifier,++tmp_index,block); break;
//                case LESS: case GREAT:
//                    //times = tag ? (int) val : (int) val + 1
//                    break;
//            }
//            break;
//        case Div:
////            val = log((double)init / (double)end) / log((double)step);
////            tag = end * pow(step, val) == init;
//            switch (ins_end_cond->Opcode) {
//                case EQ: value_init_int(v_mod,1);
//                    v_mod = (v_init == v_end) ? v_mod : NULL; break;
//                case NOTEQ:
//                    v_div = auto_binary_node_insert(Div,v_init,v_end,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_div,v_step,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_get_times,v_update_modifier,++tmp_index,block); break;
//                case LESSEQ: case GREATEQ:
//                    v_div = auto_binary_node_insert(Div,v_init,v_end,++tmp_index,block);
//                    v_get_times = auto_binary_node_insert(Div,v_div,v_step,++tmp_index,block);
//                    value_init_int(plus,1);
//                    v_plus = auto_binary_node_insert(Add,v_get_times,plus,++tmp_index,block);
//                    v_mod = auto_binary_node_insert(Mod,v_plus,v_update_modifier,++tmp_index,block); break;
//                case LESS: case GREAT:
//                    //times = tag ? (int) val : (int) val + 1
//                    break;
//            }
//            break;
//    }
//    return v_mod;
//}
//
//void adjust_blocks(BasicBlock* head, BasicBlock* new_pre_block){
//    //将Head的前驱对应的后继全部指向new_pre_block
//    HashSet *preBlocks = head->preBlocks;
//    HashSet *set = HashSetInit();
//    HashSetFirst(preBlocks);
//    for(BasicBlock *block = HashSetNext(preBlocks); block != NULL ;block = HashSetNext(preBlocks)){
//        if(block->true_block && block->true_block == head){
//            block->true_block = new_pre_block;
//        }
//        else if(block->false_block && block->false_block == head){
//            block->false_block = new_pre_block;
//        }
//        HashSetAdd(set , block);
//    }
//    new_pre_block->preBlocks = set;
//    new_pre_block->true_block = head;
//}
//
//void LOOP_UNROLL_EACH(Loop* loop)
//{
//    //TODO 目前不做基本块间
//    if(HashSetSize(loop->loopBody) > 2)
//        return;
//
//    Instruction *ins_end_cond=(Instruction*)loop->end_cond;
//    Instruction *ins_modifier=(Instruction*)loop->modifier;
//    Value *v_step=NULL,*v_end=NULL;
//    if(ins_get_lhs(ins_end_cond)==loop->inductionVariable)
//        v_end= ins_get_rhs(ins_end_cond);
//    else
//        v_end= ins_get_lhs(ins_end_cond);
//    if(ins_get_lhs(ins_modifier)==loop->inductionVariable)
//        v_step= ins_get_rhs(ins_modifier);
//    else
//        v_step= ins_get_lhs(ins_modifier);
//
//    //步长为0就return
//    if(v_step->VTy->ID == Int && v_step->pdata->var_pdata.iVal==0)
//        return;
//
//    int cnt=cal_ir_cnt(loop->loopBody);
//    //如果是常数，就能计算迭代次数，然后进行一个判断,TODO 非常数就不判断了吗
//    Value *times = (Value*) malloc(sizeof (Value));
//    if(check_idc(loop->initValue,v_step,v_end)){
//        value_init_int(times, cal_times(loop->initValue->pdata->var_pdata.iVal,ins_modifier,ins_end_cond,v_step,v_end));
//        //超出一定长度的循环，不进行展开
//        if((long)times*cnt>loop_unroll_up_lines)
//            return;
//    }
//
//    //一个map用来保存header中变量与最新值的对应关系
//    //value*----pair的hashset
//    HashMap *v_new_valueMap=HashMapInit();
//
//    //记录其他中间量与最新值的对应关系
//    //value*-----value*
//    HashMap *other_new_valueMap=HashMapInit();
//
//    //扫head
//    //head块保存一下初始中map中变量与最新更新值的对应关系
//    //%4( %2) = phi i32[%14 , %13], [%2 , %0]
//    //保存一个%4------(13:%14, 0:%2)
//    InstNode *head_currNode = get_next_inst(loop->head->head_node);
//    InstNode *head_bodyTail = loop->head->tail_node;
//
//    //head块中phi一定在最前面
//    while(head_currNode != head_bodyTail && head_currNode->inst->Opcode==Phi){
//        Value *variable=&head_currNode->inst->user.value;
//        HashSet *map_phi_set=HashSetInit();
//        HashMapPut(v_new_valueMap,variable,map_phi_set);
//
//        HashSet *phiSet = head_currNode->inst->user.value.pdata->pairSet;
//        HashSetFirst(phiSet);
//        for(pair *phiInfo = HashSetNext(phiSet); phiInfo != NULL; phiInfo = HashSetNext(phiSet)){
//            //直接装进去
//            HashSetAdd(map_phi_set,phiInfo);
//        }
//
//        head_currNode = get_next_inst(head_currNode);
//    }
//
//    //循环展开
//    ///思路:
//    //1. 如果是常数: 可以直接计算出有无余数, 无余数的话不管；
//    // 有余数或不是常数：在head之前建一个新block,将head的preBlocks的后继都指向新Block，在这个block中计算余数(非常数,记得判断一下一次循环都走不了的情况)并跑完余数的次数(可能不止花费一个基本块)
//    //2. 归纳变量从无余数的起点开始跑
//
//    //1. 如果是常数,可以直接算出有没有余数
//    //TODO 新块的head,tail
//    BasicBlock *new_pre_block = NULL;
//    BasicBlock *mod_block = NULL;
//    if(check_idc(loop->initValue,v_step,v_end)){
//        int mod_num = times->pdata->var_pdata.iVal % update_modifier;       //余数是迭代次数对update_modifier取余
//        if(mod_num != 0){               //无余数的情况不处理
//            //来新block
//            new_pre_block = bb_create();
//            new_pre_block->Parent = ins_end_cond->Parent->Parent;
//
//            InstNode *pos = loop->head->head_node;
//            Instruction *label = ins_new_zero_operator(Label);
//            InstNode *node_label = new_inst_node(label);
//            ins_insert_before(node_label,pos);
//            new_pre_block->head_node = node_label;
//
//            //调整下前驱后继块
//            adjust_blocks(loop->head,new_pre_block);
//            //将要copy的内容copy mod_num次
//            for(int i=0; i < mod_num ;i++){
//                //TODO 调用copy内容的函数
//            }
//        }
//    } else {
//        //用ir算余数
//        //肯定有新block
//        //icmp 0,%v_mod,mod的block在new_pre_block之前
//        mod_block = bb_create();
//        new_pre_block = bb_create();
//        new_pre_block->Parent = ins_end_cond->Parent->Parent;
//        mod_block->Parent = ins_end_cond->Parent->Parent;
//        adjust_blocks(loop->head,new_pre_block);
//        adjust_blocks(new_pre_block,mod_block);
//
//        //给mod_block建一条head
//        InstNode *pos = loop->head->head_node;
//        Instruction *label = ins_new_zero_operator(Label);
//        InstNode *node_label = new_inst_node(label);
//        ins_insert_before(node_label,pos);
//        mod_block->head_node = node_label;         //然后算余数的ir都在这条ir后面
//
//        InstNode *node_mod = get_mod(loop->initValue,ins_modifier,ins_end_cond,v_step,v_end,mod_block, node_label);
//        //br到下一个block循环mod次
//        Instruction *ins_br = ins_new_zero_operator(br);
//        ins_br->Parent = mod_block;
//        InstNode *node_br = new_inst_node(ins_br);
//        ins_insert_after(node_br,node_mod);
//        mod_block->tail_node = node_br;
//
//        //给new_pre_block建一条head
//        Instruction *label2 = ins_new_zero_operator(Label);
//        label2->Parent = new_pre_block;
//        InstNode *node_label2 = new_inst_node(label2);
//        ins_insert_after(node_label2,node_br);
//
//        //包装一个phi，初始值为0，每次加1,就专职用来计数这个补余数的小循环
//        Value *v_zero=(Value*) malloc(sizeof (Value));
//        value_init_int(v_zero,0);
//        Instruction *ins_phi = ins_new_zero_operator(Phi);
//        HashSet * set = HashSetInit();
//        pair *pair1 = (pair*) malloc(sizeof (pair));
//        pair1->from = mod_block;
//        pair1->define = v_zero;
//        HashSetAdd(set,pair1);
//        ins_get_value_with_name_and_index(ins_phi,++tmp_index)->pdata->pairSet = set;
//        InstNode *node_phi = new_inst_node(ins_phi);
//        ins_insert_after(node_phi,node_label2);
//        //TODO 生成一条icmp,在new_pre_block中
//        InstNode *node_icmp = auto_binary_node_insert_after(LESS, ins_get_dest(ins_phi), ins_get_dest(node_mod->inst),++tmp_index,new_pre_block,node_phi);
//        //生成br_i1
//        Instruction *ins_bri1 = ins_new_unary_operator(br_i1, ins_get_dest(node_icmp->inst));
//        ins_bri1->Parent = new_pre_block;
//        InstNode *node_br_i1 = new_inst_node(ins_bri1);
//        ins_insert_after(node_br_i1,node_icmp);
//
//        //TODO 基本块内循环 还要再来个block来装循环体
//        BasicBlock *loop_pre_block = bb_create();
//        bb_add_prev(new_pre_block, loop_pre_block);
//        new_pre_block->true_block = loop_pre_block;
//        new_pre_block->false_block = loop->head;
//
//        //label
//        //TODO 调用copy内容的函数 一次
//        //br
//
//        //调用copy内容的函数
//        //v_init++;
//        //看看v_init还需不需要调整
//    }
//}
//
////进行dfs，从最里层一步步展开
//void dfsLoop(Loop *loop){
//    HashSetFirst(loop->child);
//    for(Loop *childLoop = HashSetNext(loop->child); childLoop != NULL; childLoop = HashSetNext(loop->child)){
//        /*内层循环先处理*/
//        dfsLoop(childLoop);
//    }
//    LOOP_UNROLL_EACH(loop);
//}
//
//void loop_unroll(Function *currentFunction)
//{
//    HashSetFirst(currentFunction->loops);
//    //遍历每个loop
//    for(Loop *root = HashSetNext(currentFunction->loops); root != NULL; root = HashSetNext(currentFunction->loops)){
//        dfsLoop(root);
//    }
//}