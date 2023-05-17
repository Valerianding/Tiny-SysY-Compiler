//
// Created by ljf on 23-2-25.
//
#include <unistd.h>
#include "arm.h"

//ri
int regi=0;
//si
int regs=0;
int func_call_func;
extern InstNode *one_param[];
extern InstNode *params[];
extern HashMap *global_hashmap;
int give_count=0;
int globalvar_num;
char globalvar_message[10000];

//存放打开文件的FILE指针
FILE *fp;
//记录需要保护的寄存器，1表示需要保护
int reg_save[16];
//记录r0-r3在FuncBegin的时候str时候的偏移量
int param_off[4];

void printf_stmfd_rlist(){
//    printf();
//    fprintf(fp,);
    int k=0,n=0;
    for(int i=4;i<13;i++){
        if(reg_save[i]==1){
            n=i;
            k+=1;
        }
    }
    if(k==0&&func_call_func==0){
//        都不需要保存
        return;
    }
    if(k==0&&func_call_func>0){
//        只需要保存lr
        printf("\tstr\tlr,[sp,#-4]!\n");
        fprintf(fp,"\tstr\tlr,[sp,#-4]!\n");
        return;
    }
    if(k!=0&&func_call_func==0){
//        不需要保存lr
        if(k==1){
            printf("\tstr\tr%d,[sp,#-4]!\n",n);
            fprintf(fp,"\tstr\tr%d,[sp,#-4]!\n",n);
            return;
        }
        int start = -1;
        int end = -1;

        printf("\tstmfd\tsp!,{");
        fprintf(fp,"\tstmfd\tsp!,{");
        for (int i = 4; i < 13; i++) {
            if (reg_save[i] == 1) {
                if (start == -1) {
                    start = i;
                    end = i;
                } else if (i == end + 1) {
                    end = i;
                } else {
                    if (start == end) {
                        printf("r%d,", start);
                        fprintf(fp,"r%d,", start);
                    } else {
                        printf("r%d-r%d,", start, end);
                        fprintf(fp,"r%d-r%d,", start, end);
                    }
                    start = i;
                    end = i;
                }
            }
        }

        if (start != -1) {
            if (start == end) {
                printf("r%d", start);
                fprintf(fp,"r%d", start);
            } else {
                printf("r%d-r%d", start, end);
                fprintf(fp,"r%d-r%d", start, end);
            }
        }
        printf("}\n");
        fprintf(fp,"}\n");
        return;
    }
//    既要保存现场，又要保存lr
    int start = -1;
    int end = -1;

    printf("\tstmfd\tsp!,{");
    fprintf(fp,"\tstmfd\tsp!,{");
    for (int i = 4; i < 13; i++) {
        if (reg_save[i] == 1) {
            if (start == -1) {
                start = i;
                end = i;
            } else if (i == end + 1) {
                end = i;
            } else {
                if (start == end) {
                    printf("r%d,", start);
                    fprintf(fp,"r%d,", start);
                } else {
                    printf("r%d-r%d,", start, end);
                    fprintf(fp,"r%d-r%d,", start, end);
                }
                start = i;
                end = i;
            }
        }
    }

    if (start != -1) {
        if (start == end) {
            printf("r%d", start);
            fprintf(fp,"r%d", start);
        } else {
            printf("r%d-r%d", start, end);
            fprintf(fp,"r%d-r%d", start, end);
        }
    }
    printf(",lr}\n");
    fprintf(fp,",lr}\n");
    return;
}
void printf_ldmfd_rlist(){
//    printf();
//    fprintf(fp,);
    int k=0,n=0;
    for(int i=4;i<13;i++){
        if(reg_save[i]==1){
            n=i;
            k+=1;
        }
    }
    if(k==0&&func_call_func==0){
//        都不需要恢复
        return;
    }
    if(k==0&&func_call_func>0){
//        只需要恢复lr
        printf("\tldr\tlr,[sp],#4\n");
        fprintf(fp,"\tldr\tlr,[sp],#4\n");
        return;
    }
    if(k!=0&&func_call_func==0){
//        不需要恢复lr
        if(k==1){
            printf("\tldr\tr%d,[sp],#4\n",n);
            fprintf(fp,"\tldr\tr%d,[sp],#4\n",n);
            return;
        }
        int start = -1;
        int end = -1;

        printf("\tldmfd\tsp!,{");
        fprintf(fp,"\tldmfd\tsp!,{");
        for (int i = 4; i < 13; i++) {
            if (reg_save[i] == 1) {
                if (start == -1) {
                    start = i;
                    end = i;
                } else if (i == end + 1) {
                    end = i;
                } else {
                    if (start == end) {
                        printf("r%d,", start);
                        fprintf(fp,"r%d,", start);
                    } else {
                        printf("r%d-r%d,", start, end);
                        fprintf(fp,"r%d-r%d,", start, end);
                    }
                    start = i;
                    end = i;
                }
            }
        }

        if (start != -1) {
            if (start == end) {
                printf("r%d", start);
                fprintf(fp,"r%d", start);
            } else {
                printf("r%d-r%d", start, end);
                fprintf(fp,"r%d-r%d", start, end);
            }
        }
        printf("}\n");
        fprintf(fp,"}\n");
        return;
    }
//    既要保存现场，又要保存lr
    int start = -1;
    int end = -1;

    printf("\tldmfd\tsp!,{");
    fprintf(fp,"\tldmfd\tsp!,{");
    for (int i = 4; i < 13; i++) {
        if (reg_save[i] == 1) {
            if (start == -1) {
                start = i;
                end = i;
            } else if (i == end + 1) {
                end = i;
            } else {
                if (start == end) {
                    printf("r%d,", start);
                    fprintf(fp,"r%d,", start);
                } else {
                    printf("r%d-r%d,", start, end);
                    fprintf(fp,"r%d-r%d,", start, end);
                }
                start = i;
                end = i;
            }
        }
    }

    if (start != -1) {
        if (start == end) {
            printf("r%d", start);
            fprintf(fp,"r%d", start);
        } else {
            printf("r%d-r%d", start, end);
            fprintf(fp,"r%d-r%d", start, end);
        }
    }
    printf(",lr}\n");
    fprintf(fp,",lr}\n");
    return;
}
void handle_reg_save(int reg){
    int reg_abs=abs(reg);
    int reg_dest;
    if(reg_abs>100){
        reg_dest=reg_abs-100;
    }else{
        reg_dest=reg_abs;
    }
    if(reg_dest<=3){
//        不用保存
    }
    if(reg_dest>=13){
//        不用保存
    }
    else{
        reg_save[reg_dest]=1;
    }
    return;
}
//返回当前维需要计算的数组的偏移量（乘加的乘数值）
int array_suffix(Value*array,int which_dimension){
//    总的维数
    int sum_dimension= array->pdata->symtab_array_pdata.dimention_figure;
//    每维具体的值在dimension数组里面可以取到
//    如果直接传入是GEP的value0的ival，那么得到的维数是当前的维数,比如说在计算第0维
//    所以在这里面计算计算的时候，需要i=which_dimension,i<sum_dimension
//    最后一维的边界也是在这里面进行计算的
    int result=1;
    for(int i=which_dimension+1;i<sum_dimension;i++){
        result*= array->pdata->symtab_array_pdata.dimentions[i];
    }
    return result;
}
bool imm_is_valid(unsigned int imm){
    int i;
    for (i = 0; i <= 30; i += 2) {
        unsigned int rotated = (imm >> i) | (imm << (32 - i));
        if (rotated <= 255 && (rotated & ~0xff) == 0)
            return true;
    }
    return false;
}
void arm_open_file(char filename[]){
//    int len= strlen(argv);
//    char filepath[256];
//    char *prefix= strncpy(malloc(len-2),argv,len-3);
//    prefix[len-3]='\0';
//    snprintf(filepath, sizeof(filepath),"../test/%s.s",prefix);
//    free(prefix);
//    FILE *myopen= fopen(filepath,"w");
////    fprintf(myopen,"haha");
//    if(myopen==NULL){
//        perror("open file error");
//    }
//    return myopen;
    char new_ext[] = ".s";
    char *dot_ptr = strrchr(filename, '.');
    if(dot_ptr) {
        int basename_len = dot_ptr - filename;  // 计算基本文件名的长度
        char new_filename[basename_len + strlen(new_ext) + 1];  // 为新文件名分配足够的空间

        strncpy(new_filename, filename, basename_len);  // 复制基本文件名
        new_filename[basename_len] = '\0';  // 在基本文件名后添加空字符
        strcat(new_filename, new_ext);  // 连接新的扩展名
//        打开文件
        fp= fopen(new_filename,"w");
        printf("new_filename %s\n",new_filename);
    } else {
        printf("输入文件名有误\n");
    }
    return;
}
void arm_close_file(){
    fclose(fp);
    fp=NULL;
    return;
}
int get_value_offset_sp(HashMap *hashMap,Value*value){
    offset *node= HashMapGet(hashMap, value);
    if(node!=NULL) {
        return node->offset_sp;
    }
    if(node==NULL&&(isImmFloatType(value->VTy)|| isImmIntType(value->VTy))){
//        printf("this is imm,can't find in stack!!!");
    }
    return -1;
}

void FuncBegin_hashmap_add(HashMap*hashMap,Value *value,char *name,int *local_stack){
// 这种存储方法好像是错的，因为后面用到的同一个value，这里给他分配了不同地址的mykey_node()，所以回得到不的key
// hashmap不会释放value*对应的内存
    char param_name[5]="%3";
//    if(value->VTy->ID==ArrayTyID){
////        是数组，offset_sp需要加更多
//    }
//    是参数,不用处理,这里是用来计算局部变量的栈开多大，再加上param_num*4就可以得到总的栈帧的大小了。
//    这里和之前的设计不同的是，r0-r3转递过来的参数也是当局部变量来处理的，但是超出r3的其他变量会在父函数give_param翻译的时候就直接str并且改变sp
//    所以说这里不需要为多余的参数再开辟空间，所以说这里是需要处理r0-r3这几个参数的就是对应%0-%3需要开辟局部变量空间
    if(value->name!=NULL && strcmp(value->name,name)<0 && strlen(value->name)<= strlen(name)){
        if(strcmp(value->name,param_name)<0&& strlen(value->name)<= strlen(param_name)){
//            为r0-r3对应的参数
            if(!HashMapContain(hashMap, value)){
                int x= atoi((value->name)+1);
                offset *node=offset_node();
                node->memory=1;
                node->regr=-1;
                node->regs=-1;
                node->offset_sp=*local_stack;
                param_off[x]=node->offset_sp;
                (*local_stack)+=4;
            }

        }
//        free(key);重复释放的原因有可能在这里
    }
    else if(isImmIntType(value->VTy)|| isImmFloatType(value->VTy)){
//        立即数也是不用处理的;
    }
//    全局变量也是不用处理的，因为全局变量的值并不会存在栈帧里面，
//    全变量使用global_hashmap来进行记录，所以说全局变量不需要添加
    else if(isGlobalVarFloatType(value->VTy)){

//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }

    }else if(isGlobalVarIntType(value->VTy)){

//        if(!HashMapContain(global_hashmap,key)){
//            LCPTLabel *lcptLabel=(LCPTLabel*) malloc(sizeof(LCPTLabel));
//            sprintf(lcptLabel->LCPI,".LCPI%d_%d",func_num,in_func_num++);
//            HashMapPut(global_hashmap,key,lcptLabel);
//        }

    }else if(isGlobalArrayIntType(value->VTy)){
        ;
    } else if(isGlobalArrayIntType(value->VTy)){
        ;
    }
    else{
//        数组只在alloca指令里面开辟，其他的不用管
        if(!HashMapContain(hashMap, value)){
            offset *node=offset_node();
            node->memory=1;
            node->regs=-1;
            node->regr=-1;
            node->offset_sp=*local_stack;
            (*local_stack)+=4;
//            数组只在alloca指令里面开辟，其他的不用管
//            if(isLocalArrayIntType(value->VTy)||isLocalArrayFloatType(value->VTy)||isGlobalArrayIntType(value->VTy)||isGlobalArrayFloatType(value->VTy)){
////        是数组，offset_sp需要加更多
//                int x= get_array_total_occupy(value->alias,0);
//                (*local_stack)=(*local_stack)+x-4;
//            }
//            printf("funcBeginhaspmapsize:%d name:%s  keyname:%s address%p\n",HashMapSize(hashMap),name,value->name,value);
            HashMapPut(hashMap,value,node);
        }
    }
    return;
}
void FuncBegin_hashmap_alloca_add(HashMap*hashMap,Value *value,int *local_stack){
    if(!HashMapContain(hashMap, value)){
//        offset *node=offset_node();
//        node->memory=1;
//        node->regs=-1;
//        node->regr=-1;
//        node->offset_sp=*local_stack;
//        (*local_stack)+=4;
//        HashMapPut(hashMap,value,node);
        if(isLocalArrayIntType(value->VTy)||isLocalArrayFloatType(value->VTy)||isGlobalArrayIntType(value->VTy)||isGlobalArrayFloatType(value->VTy)){
//           全局数组好像不应该在这里处理的
            int size_array= get_array_total_occupy(value->alias,0);
            offset *node=offset_node();
            node->offset_sp=(*local_stack);
            (*local_stack)+=size_array;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,value,node);
        } else{
//            这个应该是处理其他的情况，比如说是unknown的情况
            offset *node=offset_node();
            node->offset_sp=(*local_stack);
            (*local_stack)+=4;
            node->memory=true;
            node->regs=-1;
            node->regr=-1;
            HashMapPut(hashMap,value,node);
        }
    }
    return;
}
void FuncBegin_hashmap_bitcast_add(HashMap*hashMap,Value *value0,Value *value1,int *local_stack){
//    Value *contain=value1;
//    while (HashMapContain(hashMap,contain)){
//
//    }
    if(!HashMapContain(hashMap,value0)){
        if(HashMapContain(hashMap,value1)){
            offset *node= HashMapGet(hashMap,value1);
            HashMapPut(hashMap,value0,node);
        }
    }
    return;

//    offset *node=offset_node();
//    node->offset_sp=(*local_stack);
//    (*local_stack)+=4;
//    node->memory=true;
//    node->regs=-1;
//    node->regr=-1;
//    HashMapPut(hashMap,value0,node);
}

int get_siezof_sp(HashMap*hashMap){
    return HashMapSize(hashMap);
}

int get_value_pdata_inspdata_true(Value*value){
    return value->pdata->instruction_pdata.true_goto_location;
}

int get_value_pdata_inspdata_false(Value*value){
    return value->pdata->instruction_pdata.false_goto_location;
}
void usage_of_global_variables(){
//    HashMap *tmp=HashMapInit();
    HashMapFirst(global_hashmap);
    Pair * ptr_pair;
    while ((ptr_pair=HashMapNext(global_hashmap))!=NULL){
//        直接遍历会得到一个顺序相反的情况，按这个顺序再存一边就可以了
        Value *key=(Value*)ptr_pair->key;
        LCPTLabel *lcptLabel=(LCPTLabel*)ptr_pair->value;
        printf("%s:\n\t.long\t%s\n",lcptLabel->LCPI,key->name+1);
        fprintf(fp,"%s:\n\t.long\t%s\n",lcptLabel->LCPI,key->name+1);
        if(lcptLabel!=NULL){
            free(lcptLabel);
        }

        lcptLabel=NULL;
//        HashMapPut(tmp,key,lcptLabel);
    }
//    HashMapFirst(tmp);
//    while ((ptr_pair= HashMapNext(tmp))!=NULL){
//        Value *key=(Value*)ptr_pair->key;
//        LCPTLabel *lcptLabel=(LCPTLabel*)ptr_pair->value;
//        printf("%s:\n\t.long\t%s\n",lcptLabel->LCPI,key->name+1);
//        free(lcptLabel);
//        lcptLabel=NULL;
//    }
//    HashMapDeinit(tmp);
//    tmp=NULL;

    return;
}
void arm_translate_ins(InstNode *ins){
//    global_hashmap=HashMapInit();
    InstNode *head;
    HashMap *hashMap;
    int stack_size=0;
    for(;ins!=NULL;ins=get_next_inst(ins)) {
        if(ins->inst->Opcode==FunBegin){
//  这个逻辑有问题，现在的设计是再FunBegin里面执行栈帧开辟的sub指令，大小就是局部变量个数加上参数个数*4
//  然后在sub指令之后执行str r0,r1,r2,r3就是保存这几个寄存器的值，保存的位置就是在sp+局部变量个数*4+参数序号(从0开始)*4
//  执行完这两个之后，返回局部变量开辟的栈帧大小。返回之后进行
//  真正栈帧的开辟，参数的存储位置从局部变量位置之上开始存储。
//  所以说当前这个逻辑和FuncBegin函数的实现逻辑都得大改。
            global_hashmap=HashMapInit();
            regi=0;
            regs=0;
            stack_size=0;
            func_call_func=0;
            ins= arm_trans_FunBegin(ins,&stack_size);
            head=ins;
            int  param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
//            int local_var_num=(stack_size-param_num*4)/4;
            int local_var_num=(stack_size)/4;
//            在进入函数时将offset初始化好，以供load指令使用
            int k=0;
            for(int i=4;i<13;i++){
                if(reg_save[i]==1){
                    k++;
                }
            }
            hashMap=offset_init(ins,&local_var_num,k);
            int hashsize= HashMapSize(hashMap)*4;
//            printf("hashsize=%d\n",hashsize);
            ins= get_next_inst(ins);
        }
        ins=_arm_translate_ins(ins,head,hashMap,stack_size);
        if(ins->inst->Opcode==Return){
            offset_free(hashMap);
//            将全局变量的使用打印
            usage_of_global_variables();
            HashMapDeinit(global_hashmap);
            global_hashmap=NULL;
            hashMap=NULL;
        }
    }
//    HashMapDeinit(hashMap);
    printf("%s\n",globalvar_message);
    fprintf(fp,"%s\n",globalvar_message);
    return;
}
InstNode * arm_trans_CopyOperation(InstNode*ins,HashMap*hashMap){
//    默认左值的type默认和value1的type是一样的,需要考虑value0和value1是否在寄存器或者说是内存里面，这个可以后面在完善的，现在先把样例过了
    Value *value0=ins->inst->user.value.alias;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int x0= get_value_offset_sp(hashMap,value0);
    int x1= get_value_offset_sp(hashMap,value1);
    if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
        int x=value1->pdata->var_pdata.iVal;
        printf("\tmov\tr%d,#%d\n",dest_reg_abs,x);
        fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,x);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
        return ins;
    }
    if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
        int x=value1->pdata->var_pdata.iVal;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",x);
        printf("\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
        fprintf(fp,"\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
        return ins;

    }
    if(isImmFloatType(value1->VTy)){
        float y=value1->pdata->var_pdata.fVal;
        int x=*(int*)&y;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",x);
        printf("\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
        fprintf(fp,"\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
        return ins;
        ;
    }
    if(isLocalVarIntType(value1->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            if(dest_reg_abs!=(left_reg-100)){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            }

        } else{
            if(dest_reg_abs!=left_reg){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            }
        }

        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
        return ins;
    }
    if(isLocalVarFloatType(value1->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            if(dest_reg_abs!=(left_reg-100)){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            }

        } else{
            if(dest_reg_abs!=left_reg){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            }
        }

        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
        return ins;
    }
//    两个吧unknown的情况,好像这样翻译应该是有点问题的
//    offset *node= HashMapGet(hashMap,value1);
//    if(node->memory){
//        printf("    ldr r0,[sp,#%d]\n",x1);
//        printf("    str r0,[sp,#%d]\n",x0);
//    }
    if(left_reg>100){
        int x= get_value_offset_sp(hashMap,value1);
        printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
        fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
        if(dest_reg_abs!=(left_reg-100)){
            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
        }

    } else{
        if(dest_reg_abs!=left_reg){
            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
        }
    }

    if(dest_reg<0){
        int x= get_value_offset_sp(hashMap,value0);
        printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
    }else{
        ;
    }

//    printf("CopyOperation\n");
    return ins;
}

//更新寄存器的话，add、sub、mul、div、less等是每个都要改的


InstNode * arm_trans_Add(InstNode *ins,HashMap*hashMap){

    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tadd\tr%d,r1,#%d\n",dest_reg_abs,x2);
            fprintf(fp,"\tadd\tr%d,r1,#%d\n",dest_reg_abs,x2);
//            printf("    add r%d,#%d,#%d\n",result_regri,x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tadd\tr%d,r1,#%d\n",dest_reg_abs,x2);
            fprintf(fp,"\tadd\tr%d,r1,#%d\n",dest_reg_abs,x2);
//            printf("    add r%d,r1,#%d\n",result_regri,x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tadd\tr%d,r2,#%d\n",dest_reg_abs,x1);
            fprintf(fp,"\tadd\tr%d,r2,#%d\n",dest_reg_abs,x1);
//            printf("    add r%d,r2,#%d\n",result_regri,x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tadd\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tadd\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    add r%d,r1,r2\n",result_regri);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
               if(dest_reg>0){
                    ;
               } else{
                    int x= get_value_offset_sp(hashMap,value0);
                   printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                   fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
               }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tfcvt.f32.s32\ts1,rs\n");
            fprintf(fp,"\tfcvt.f32.s32\ts1,rs\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,rs\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,rs\n");

        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
//        判断结果（左值类型）
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");

        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
            }else{
                ;
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tadd\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                fprintf(fp,"\tadd\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
            }else{
                printf("\tadd\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                fprintf(fp,"\tadd\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
            }

        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
            }else{
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
            }else{
                printf("\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
            }
        }


        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
//        int x3= get_value_offset_sp(hashMap,value1);
//        int x4= get_value_offset_sp(hashMap,value2);
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
        } else{
            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    return  ins;
}

InstNode * arm_trans_Sub(InstNode *ins,HashMap*hashMap){

    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tsub\tr%d,r1,#%d\n",dest_reg_abs,x2);
            fprintf(fp,"\tsub\tr%d,r1,#%d\n",dest_reg_abs,x2);
//            printf("    sub r%d,#%d,#%d\n",result_regri,x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tsub\tr%d,r1,#%d\n",dest_reg_abs,x2);
            fprintf(fp,"\tsub\tr%d,r1,#%d\n",dest_reg_abs,x2);
//            printf("    sub r%d,r1,#%d\n",result_regri,x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tsub\tr%d,r2,#%d\n",dest_reg_abs,x1);
            fprintf(fp,"\tsub\tr%d,r2,#%d\n",dest_reg_abs,x1);
//            printf("    sub r%d,r2,#%d\n",result_regri,x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tsub\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tsub\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    sub r%d,r1,r2\n",result_regri);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tfcvt.f32.s32\ts1,rs\n");
            fprintf(fp,"\tfcvt.f32.s32\ts1,rs\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,rs\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,rs\n");

        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
//        判断结果（左值类型）
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");

        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
            }else{
                ;
                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                fprintf(fp,"\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
            }else{
                printf("\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                fprintf(fp,"\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
            }

        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
            }else{
                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tsub\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tsub\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
            }else{
                printf("\tsub\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tsub\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
            }
        }


        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
        } else{
            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvsub.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvsub.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    return  ins;

}

InstNode * arm_trans_Mul(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tmul\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tmul\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    mul r%d,#%d,#%d\n",result_regri,x1,x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tmul\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tmul\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    mul r%d,r1,#%d\n",result_regri,x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tmul\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tmul\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    mul r%d,r2,#%d\n",result_regri,x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tmul\tr%d,r1,r2\n",dest_reg_abs);
            fprintf(fp,"\tmul\tr%d,r1,r2\n",dest_reg_abs);
//            printf("    mul r%d,r1,r2\n",result_regri);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tfcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tfcvt.f32.s32\ts1,s1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
//        判断结果（左值类型）
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");

        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
            }else{
                ;
                printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
            }else{
                printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
            }

        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
            }else{
                printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
            }else{
                printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
            }
        }


        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg-100);
        } else{
            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg,right_reg);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvmul.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvmul.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    return  ins;
}

InstNode * arm_trans_Div(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            printf("\tmov\tr1,#%d\n",x2);
            fprintf(fp,"\tmov\tr1,#%d\n",x2);
            printf("\tbl __aeabi_idiv\n");
            fprintf(fp,"\tbl __aeabi_idiv\n");
            printf("\tmov\tr%d,r0\n",dest_reg_abs);
            fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            printf("\tbl __aeabi_idiv\n");
            fprintf(fp,"\tbl __aeabi_idiv\n");
            printf("\tmov\tr%d,r0\n",dest_reg_abs);
            fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr2);
            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
            printf("\tbl __aeabi_idiv\n");
            fprintf(fp,"\tbl __aeabi_idiv\n");
            printf("\tmov\tr%d,r0\n",dest_reg_abs);
            fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            printf("\tldr\tr1,=%s\n",arr2);
            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
            printf("\tbl __aeabi_idiv\n");
            fprintf(fp,"\tbl __aeabi_idiv\n");
            printf("\tmov\tr%d,r0\n",dest_reg_abs);
            fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tfcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tfcvt.f32.s32\ts1,s1\n");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");

        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
//        判断结果（左值类型）
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");

        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }else{
                ;
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }

        }
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x2);
            fprintf(fp,"\tmov\tr1,#%d\n",x2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr0,=%s\n",arr2);
            fprintf(fp,"\tldr\tr0,=%s\n",arr2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
            }
        }

        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        printf("\tvadd.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvadd.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            printf("\tmov\tr0,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
        } else{
            printf("\tmov\tr0,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }
        printf("\tbl\t__aeabi_idiv\n");
        fprintf(fp,"\tbl\t__aeabi_idiv\n");
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            ;
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2");
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");
        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvdiv.f32\ts0,s1,s2\n");
        fprintf(fp,"\tvdiv.f32\ts0,s1,s2\n");

        if(isLocalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
            printf("\tvcvt.s32.f32\ts0,s0\n");
            fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    return  ins;
}


//现在的话，那个加减乘除法的基本已经接上和改完了

InstNode * arm_trans_Module(InstNode *ins,HashMap*hashMap){
//    这个比较简单，只会存在两种情况，就是int=int1 % int2和float=int1 % int2,右边出现非int都是错误的
// 1 ****************************************
// 将int1放在r0，将int2放在r1，然后执行bl	__aeabi_idivmod(PLT)
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
    if(isImmIntType(value1->VTy)&& isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            printf("\tmov\tr1,#%d\n",x2);
            fprintf(fp,"\tmov\tr1,#%d\n",x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            printf("\tmov\tr1,#%d\n",x2);
            fprintf(fp,"\tmov\tr1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr2);
            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            printf("\tldr\tr1,=%s\n",arr2);
            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }else{
                ;
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr0,=%s\n",arr1);
            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
            }
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }else{
                ;
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if((imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x2);
            fprintf(fp,"\tmov\tr1,#%d\n",x2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr1,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",left_reg-100);
            } else{
                printf("\tmov\tr1,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",left_reg);
            }
        }else{
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr2);
            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr1,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",left_reg-100);
            } else{
                printf("\tmov\tr1,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",left_reg);
            }
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }else{
                ;
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            printf("\tmov\tr0,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
        } else{
            printf("\tmov\tr0,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }else{
                ;
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }else if(isLocalArrayIntType(value0->VTy)){
            ;
        }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            ;
        }
    }

//    printf("arm_trans_Module\n");
    return ins;
}

InstNode * arm_trans_Call(InstNode *ins,HashMap*hashMap){
//    现在的话，参数传递是在call指令bl之前来处理的，
//    所以说现在是先调用一下get_param_list(),传入call的value1，和give_count这个变量的地址，然后就可以使用0ne_param里面的ir了

    int operandNum=ins->inst->user.value.NumUserOperands;

//    现在这个call简单修复了一下，就是返回值为Unkonwn的话，
//    将相当于使void没有返回值，这个时候是不需要进行将r0移到左值里
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int param_num_=value1->pdata->symtab_func_pdata.param_num;
    get_param_list(value1,&give_count);
//    for(int i=0;i<param_num_;i++){
//        InstNode *temp=one_param[i];
//        printf("\n");
//    }
    arm_trans_GIVE_PARAM(hashMap,param_num_);

//    printf("CALL\n");
    printf("\tbl\t%s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    fprintf(fp,"\tbl\t%s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
//    这里还需要调整sp,去掉压入栈的参数，这里可以使用add直接调整sp，也可以使用mov sp,fp直接调整
    if(param_num_>4){
        int x=param_num_-4;
//        printf("\tadd\tsp,sp,#%d\n",x*4);
//        fprintf(fp,"\tadd\tsp,sp,#%d\n",x*4);
        printf("\tmov\tsp,r11\n");
        fprintf(fp,"\tmov\tsp,r11\n");
    }
//    printf("operandNum=%d\n",operandNum);下面这个判断好像是有点问题的
//    如果说返回值为void的情况是不需要mov r%d,r0的
//    if(operandNum==2){
//
//    }else if(operandNum==1){
////        call函数的返回值为void，不需要处理
//    }

    //        call函数的返回值不为void,还要将r0转移到左值
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    if(dest_reg_abs!=0){
        printf("\tmov\tr%d,r0\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
    }
    // 这个也是默认放回值类型和左值类型是一致的
    // 如果用一个float型去接受int型的结果，后面应该会有Copy指令的
    if(isLocalVarIntType(value0->VTy)){

        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            ;
        }
    } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
//        printf("\tvmov\ts0,r%d\n",dest_reg_abs);
//        printf("\tvcvt.f32.s32\ts0,s0\n");
//        printf("\tvmov\tr%d,s0\n",dest_reg_abs);
        if(dest_reg>0){
            ;
        } else{
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }
    }else if(isLocalArrayIntType(value0->VTy)){
        ;
    }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
    }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
//        printf("\tvmov\ts0,r%d\n",dest_reg_abs);
//        printf("\tvcvt.f32.s32\ts0,s0\n");
//        printf("\tvmov\tr%d,s0\n",dest_reg_abs);
    }else if(isGlobalArrayIntType(value0->VTy)){
        ;
    }


    return ins;
}

InstNode * arm_trans_FunBegin(InstNode *ins,int *stakc_size){
    memset(reg_save,0, sizeof(reg_save));
    memset(param_off,-1, sizeof(param_off));

    printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    fprintf(fp,"%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    InstNode *tmp=ins;
//        这里好像是如果main中调用了其他的函数，就需要将r11,sp压栈,所以说需要查找一下有没有call
//        这个好像所有函数都是一样的，如果它调用了其他的函数的话，就需要保存r11(也称为fp)和sp，那这样的话
//        main函数好像和其他的函数就没有什么区别了
//    if(!strcmp("main", user_get_operand_use(&ins->inst->user,0)->Val->name)){
//        printf("    push {r11,lr}\n");
////        printf("    mov r1,sp\n");
//    }


//只要存在bl指令，就需要保存lr
    for(;tmp!=NULL && tmp->inst->Opcode!= Return;tmp= get_next_inst(tmp)){
//        数组初始化的时候会bl
        if(tmp->inst->Opcode==Alloca){
            Value *value0=&tmp->inst->user.value;
            if(isLocalArrayIntType(value0->VTy)|| isLocalArrayFloatType(value0->VTy)|| isGlobalArrayIntType(value0->VTy)||isGlobalArrayFloatType(value0->VTy)){
                func_call_func=1;
            }
        }
//        bl __aeabi_idiv
        if(tmp->inst->Opcode==Div){
            Value *value0 = &tmp->inst->user.value;
            Value *value1 = user_get_operand_use(&tmp->inst->user, 0)->Val;
            Value *value2 = user_get_operand_use(&tmp->inst->user, 1)->Val;
            if(isLocalVarIntType(value1->VTy)|| isGlobalVarIntType(value1->VTy)|| isImmIntType(value1->VTy) ||
                    isLocalArrayIntType(value1->VTy) || isGlobalArrayIntType(value1->VTy)){
                if(isLocalVarIntType(value2->VTy)|| isGlobalVarIntType(value2->VTy)|| isImmIntType(value2->VTy) ||
                   isLocalArrayIntType(value2->VTy) || isGlobalArrayIntType(value2->VTy)){
                    func_call_func=1;
                }
            }
        }
//        bl__aeabi_divimod
        if(tmp->inst->Opcode==Mod){
            func_call_func=1;
        }
        if(tmp->inst->Opcode==Call){
            func_call_func+=1;
            break;
        }

    }
//    lr的保存也放在保护寄存器的地方来实现
//    if(func_call_func>0){
//        printf("\tstmfd\tsp!,{r11,lr}\n");
//        fprintf(fp,"\tstmfd\tsp!,{r11,lr}\n");
//    }

    HashMap *hashMap=HashMapInit();
//    HashMapSetHash(hashMap,HashKey);
//    HashMapSetCompare(hashMap,CompareKey);
//    HashMapSetCleanKey(hashMap,CleanKey);
//    HashMapSetCleanValue(hashMap,CleanValue);

    int local_stack=0;
//    int x= get_siezof_sp(hashMap);
//    printf("    sub sp,sp,#%d\n",4*x);
    //    在函数开始的时候要进行参数传递的str的处理
    int param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
    char name[20];
    sprintf(name, "%c", '%');
    sprintf(name + 1, "%d", param_num);
    InstNode *temp=ins;
    for (; ins != NULL && ins->inst->Opcode != Return; ins = get_next_inst(ins)) {
        Value *value0, *value1, *value2;
        int reg0,reg1,reg2;
        int operandNum;
        switch (ins->inst->Opcode) {
            case Alloca:
                value0 = &ins->inst->user.value;
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_alloca_add(hashMap,value0,&local_stack);
                reg0=ins->inst->_reg_[0];
                handle_reg_save(reg0);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case Add:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case Sub:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case Mul:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case Div:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case Mod:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case Call:
                operandNum=ins->inst->user.value.NumUserOperands;
                if(operandNum==2){
                    value0 = &ins->inst->user.value;
                    value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                    if(value0->VTy!=Unknown){
                        FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                    }

                    FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                }else if(operandNum==1){
                    value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                    FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                }

                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case Store:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case Load:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case GIVE_PARAM:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg1);
                break;
            case LESS:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case GREAT:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case LESSEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case GREATEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case EQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case NOTEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
            case XOR:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case zext:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case bitcast:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            case GEP:
//                value0代表存放位置，value1代表相对起始位置，value2代表偏移量
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                GEP指令已经进行了改变，所以说GEP操作的是数组，
//                alloc指令应该是再最前面的，也就如何GEP操作数组的时候对应了array的类型，不会被重新开辟
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
//            case MEMCPY:
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
//                break;
            case GLOBAL_VAR:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                reg2=ins->inst->_reg_[2];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                handle_reg_save(reg2);
                break;
//            case MEMSET:
//                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack);
//                break;
            case CopyOperation:
                value0=ins->inst->user.value.alias;
                value1= user_get_operand_use(&ins->inst->user,0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                reg0=ins->inst->_reg_[0];
                reg1=ins->inst->_reg_[1];
                handle_reg_save(reg0);
                handle_reg_save(reg1);
                break;
            default:
                break;
        }

    }
    if(ins->inst->Opcode==Return){
        int operandNum = ins->inst->user.value.NumUserOperands;
        if(operandNum == 0){
//            表示无返回值
            ;
//        if(user_get_operand_use(&ins->inst->user,0)==NULL){
        }else{
            Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
            FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
            int reg1=ins->inst->_reg_[1];
            handle_reg_save(reg1);
        }

    }
//    *stakc_size=mystack+param_num*4;
//    int x= HashMapSize(hashMap)*4;
//    printf("FuncBeginhashmapsize=%d\n",x+param_num*4);

//    *stakc_size=local_stack+param_num*4;
//    r0-r3参数已经当局部变量来处理了，这里不需要另外添加
//    还需要加上的就是需要保存的寄存器的个数的栈的大小，
//    因为偏移量是从0开始加起的，所以说保存的寄存器应该是在局部变量的顶部
//    也就是说进入函数后，先stmfd sp!,{}再sub
//    所以说这里保护寄存器的栈帧开辟也就不需要在sub的时候进行了。
    *stakc_size=local_stack;
//    在sub之前，是需要先保护被破坏的寄存器,lr的保存也放在这里面吧
    printf_stmfd_rlist();

    if((*stakc_size)!=0){
//        这里还需要赋值fp
        printf("\tsub\tsp,sp,#%d\n",*stakc_size);
        fprintf(fp,"\tsub\tsp,sp,#%d\n",*stakc_size);
//        调整fp帧指针,在一个函数中fp只需要在FunBegin的时候调整一次就可以了
//        具体实现的时候，需要将fp改为其对应的具体的寄存器
        printf("\tmov\tr11,sp\n");
        fprintf(fp,"\tmov\tr11,sp\n");

    }

    if(param_num>0){
//        存在参数的传递
//        int local_stack=mystack;
        for(int j=0;j<param_num && j<4;j++){
//            printf("\tstr\tr%d,[sp,#%d]\n",j,local_stack+j*4);
//            fprintf(fp,"\tstr\tr%d,[sp,#%d]\n",j,local_stack+j*4);
            if(param_off[j]!=-1){
//                如果传递过来的参数没有被用到的话，就不需要存了
                printf("\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
            }
        }
    } else{
        ;
    }

//    printf("localstack=%d\n",local_stack);
//    int myx= HashMapSize(hashMap)*4;
//    printf("hashmapsize=%d\n",myx);

    HashMapDeinit(hashMap);

    ins=temp;


    return ins;
}

InstNode * arm_trans_Return(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size){
//涉及到数组的时候，栈帧的大小并不等于HashMapSize(hashmap)*4
//    if(!strcmp("main", user_get_operand_use(&head->inst->user,0)->Val->name)){
//        int x1= stack_size;
//
//        printf("    add sp,sp,#%d\n",x1);
////        printf("    mov r11,sp\n");
//        if(func_call_func>0){
//            printf("    pop {r11,lr}\n");
//        }
//        printf("    bx lr\n");
//        return ins;
//    }
    int operandNum=ins->inst->user.value.NumUserOperands;
    if(operandNum!=0){
        Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
        int left_reg= ins->inst->_reg_[1];
        if(isImmIntType(value1->VTy)){
            if(imm_is_valid(value1->pdata->var_pdata.iVal)){
                printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
            } else{
                int x=value1->pdata->var_pdata.iVal;
                char arr[12]="0x";
                sprintf(arr+2,"%0x",x);
                printf("\tldr\tr0,=%s\n",arr);
                fprintf(fp,"\tldr\tr0,=%s\n",arr);
            }
        } else if(isImmFloatType(value1->VTy)){
            float  x=value1->pdata->var_pdata.fVal;
            int xx=*(int*)&x;
            char arr[12]="0x";
            sprintf(arr+2,"%0x",x);
            printf("\tldr\tr0,=%s\n",arr);
            fprintf(fp,"\tldr\tr0,=%s\n",arr);
        } else if(isLocalVarIntType(value1->VTy)){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            }
        } else if(isLocalVarFloatType(value1->VTy)){
            ;
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            }
        }
    }
    int x1= stack_size;
    if(x1!=0){
        printf("\tadd\tsp,sp,#%d\n",x1);
        fprintf(fp,"\tadd\tsp,sp,#%d\n",x1);
    }
//    恢复被保护的寄存器
    printf_ldmfd_rlist();

//    if(func_call_func>0){
//        printf("\tldmfd\tsp!,{r11,lr}\n");
//        fprintf(fp,"\tldmfd\tsp!,{r11,lr}\n");
//    }
    printf("\tbx\tlr\n");
    fprintf(fp,"\tbx\tlr\n");
    return ins;
}

InstNode * arm_trans_Alloca(InstNode *ins,HashMap*hashMap){
//    在汇编中，alloca不需要翻译,但是栈帧分配的时候需要用到。
// 这个现在暂定是用来进行数组的初始话操作。bl memset来进行处理，就不需要调用多次store
    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    if(isLocalArrayIntType(value0->VTy)|| isLocalArrayFloatType(value0->VTy)|| isGlobalArrayIntType(value0->VTy)||isGlobalArrayFloatType(value0->VTy)){
//        printf("%s\n",value0->alias->name);
        int x=get_value_offset_sp(hashMap,value0);
        printf("\tadd\tr0,r11,#%d\n",x);
        fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
        printf("\tmov\tr1,#0\n");
        fprintf(fp,"\tmov\tr1,#0\n");
        x= get_array_total_occupy(value0->alias,0);
        printf("\tmov\tr2,#%d\n",x);
        fprintf(fp,"\tmov\tr2,#%d\n",x);
        printf("\tbl\tmemset\n");
        fprintf(fp,"\tbl\tmemset\n");
    }
    return ins;
}

InstNode * arm_trans_GIVE_PARAM(HashMap*hashMap,int param_num){
//  这个是用来标定参数传递的，这个可不仅仅是一个标定作用，
//  这个是在处理数组传参（传数组首地址），和地址指针的时候需要用到。
//要考虑吧普通的参数传递和数组地址参数传递，还要考虑参数个数的问题。如果传递的是float型的参数呢？使用IEEE754放在通用寄存器中
//    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    计算参数的传递个数



//     注意现在的参数传递的逻辑是从params[0]~param[param_num-1]为有序的参数
//     而且这个时候还需要调整正确参数的传递的顺序，对于立即数型的参数传递还需要mov r0等的这样的操作
//     因为之后的话，应该是会考虑到把那个立即数型的变量不会加到栈帧的开辟里面的

    int num=param_num;
    InstNode *tmp=NULL;
    if(num<=4){
        for(int i=0;i<num;i++){
            tmp=one_param[i];
            int left_reg= tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            //                对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr%d,=%s\n",i,arr);
                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                } else if(isImmFloatType(value1->VTy)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr%d,=%s\n",i,arr);
                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                }
            }else{
//              变量的情况，全局变量应该不用传参，需要传参的只是局部变量和立即数
                if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                        fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                }else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                    这个需要另外处理
                    int x= get_value_offset_sp(hashMap,value1);
                    printf("\tadd\tr%d,r11,#%d\n",i,x);
                    fprintf(fp,"\tadd\tr%d,r11,#%d\n",i,x);
                }
            }
//            直接在这个地方判断类型，然后加上add ri,sp,#%d好像就可以了

        }
    }else{
        int i;
        for(i=1;i<4;++i){
            tmp=one_param[i];
            int left_reg= tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr%d,=%s\n",i,arr);
                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                } else if(isImmFloatType(value1->VTy)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr%d,=%s\n",i,arr);
                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                }
            }else{
//                变量的情况，全局变量应该不用传参，需要传参的只是局部变量和立即数
                if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                        fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                }else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                    这个需要另外处理
                    int x= get_value_offset_sp(hashMap,value1);
                    printf("\tadd\tr%d,r11,#%d\n",i,x);
                    fprintf(fp,"\tadd\tr%d,r11,#%d\n",i,x);
                }
            }
        }
        for (int j = num-4; j > 0; j--) {
            tmp=one_param[i++];
            int left_reg=tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr0,=%s\n",arr);
                    fprintf(fp,"\tldr\tr0,=%s\n",arr);
                } else if(isImmFloatType(value1->VTy)){
                    char arr[12]="0x";
                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                    printf("\tldr\tr0,=%s\n",arr);
                    fprintf(fp,"\tldr\tr0,=%s\n",arr);
                }
            }
            else if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                    fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
            }else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                    这个需要另外处理
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tadd\tr0,r11,#%d\n",x);
                fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
            }
//                这个的传递顺序好像有点问题的，感觉如果give_param 是按照参数列表的顺序的话，
//                应该是str r0,[sp,#-%d],(num-4-i+1)*4;因为最后一个参数（就是参数列表里面最大的参数应该是放在sp-4的位置）
//                所以说这个后面翻译的时候是需要改的。

//            printf("\tstr\tr0,[sp,#-%d]\n",j*4);
//            fprintf(fp,"\tstr\tr0,[sp,#-%d]\n",j*4);
//          参数个数多于4个的话，就是直接往下存，并改变sp，在bl结束之后，还需要sub sp来将这些值取消掉
            printf("\tstr\tr0,[sp,#-4]!\n");
            fprintf(fp,"\tstr\tr0,[sp,#-4]!\n");

////                这个的传递顺序好像有点问题的，感觉如果give_param 是按照参数列表的顺序的话，
////                应该是str r0,[sp,#-%d],(num-4-i+1)*4;因为最后一个参数（就是参数列表里面最大的参数应该是放在sp-4的位置）
////                所以说这个后面翻译的时候是需要改的。
//            if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
////                对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递
//                printf("    add r0,sp,#%d\n",i,x);
//
//            } else{
//                printf("    ldr r0,[sp,#%d]\n",x);
//                printf("    str r0,[sp,#-%d]\n",j*4);
////                这个的传递顺序好像有点问题的，感觉如果give_param 是按照参数列表的顺序的话，
////                应该是str r0,[sp,#-%d],(num-4-i+1)*4;因为最后一个参数（就是参数列表里面最大的参数应该是放在sp-4的位置）
////                所以说这个后面翻译的时候是需要改的。
//            }
//            printf("    ldr r0,[sp,#%d]\n",x);
//            printf("    str r0,[sp,#-%d]\n",i*4);
        }


//        填充r0
        tmp=one_param[0];
        int left_reg=tmp->inst->_reg_[1];
        Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
//        int x= get_value_offset_sp(hashMap,value1);
//        printf("    ldr r0,[sp,#%d]\n",x);
        if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
            if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
            } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                char arr[12]="0x";
                sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                printf("\tldr\tr0,=%s\n",arr);
                fprintf(fp,"\tldr\tr0,=%s\n",arr);
            } else if(isImmFloatType(value1->VTy)){
                char arr[12]="0x";
                sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
                printf("\tldr\tr0,=%s\n",arr);
                fprintf(fp,"\tldr\tr0,=%s\n",arr);
            }
        }
        else if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            }
        }else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
//                    这个需要另外处理
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tadd\tr0,r11,#%d\n",x);
            fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
        }
    }


//    tmp=ins;
//    while (get_next_inst(tmp)->inst->Opcode==GIVE_PARAM){
//        tmp= get_next_inst(tmp);
//    }
//    ins=tmp;

//    int x=get_value_offset_sp(hashMap,value1);
//    printf("hello world!\n")
    return NULL;
}

InstNode * arm_trans_ALLBEGIN(InstNode *ins){
//    int i=ins->inst->i;
    printf("**********ALLBEGIN**************\n");
    return ins;
}

InstNode * arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(InstNode *ins,HashMap*hashMap){

    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tcmp\tr1,#%d\n",x2);
            fprintf(fp,"\tcmp\tr1,#%d\n",x2);
        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
            char arr[12]="0x";
            sprintf(arr+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr);
            fprintf(fp,"\tldr\tr1,=%s\n",arr);
            printf("\tcmp\tr1,#%d\n",x2);
            fprintf(fp,"\tcmp\tr1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            char arr[12]="0x";
            sprintf(arr+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr);
            fprintf(fp,"\tldr\tr2,=%s\n",arr);
            printf("\tcmp\tr2,#%d\n",x1);
            fprintf(fp,"\tcmp\tr2,#%d\n",x1);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tcmp\tr1,r2\n");
            fprintf(fp,"\tcmp\tr1,r2\n");
        }
    }
    if(isImmIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");
            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1");

            int *xx2=(int*)&x2;
            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",*xx2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");

            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,r2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,r2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        }else{
            int *xx1=(int*)&x1;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            char arr2[12]="0x";
            sprintf(arr2+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr2);
            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tfcvt.f32.s32\ts2,r2\n");
            fprintf(fp,"\tfcvt.f32.s32\ts2,r2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }


    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tcmp\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tcmp\tr1,r%d\n",right_reg);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg);
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tcmp\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tcmp\tr1,r%d\n",right_reg);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg);
            }
        }

    }
    if(isImmIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32 s1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32 s1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x1);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32 s1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32 s1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tfcvt.f32.s32\ts2,rs\n");
        fprintf(fp,"\tfcvt.f32.s32\ts2,rs\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        char arr1[12]="0x";
        sprintf(arr1+2,"%0x",*xx1);
        printf("\tldr\tr1,=%s\n",arr1);
        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tcmp\tr%d,r2\n",left_reg-100);
                fprintf(fp,"\tcmp\tr%d,r2\n",left_reg-100);
            }else{
                printf("\tcmp\tr%d,r2\n",left_reg);
                fprintf(fp,"\tcmp\tr%d,r2\n",left_reg);
            }

        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr1);
            fprintf(fp,"\tldr\tr2,=%s\n",arr1);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tcmp\tr%d,r2\n",left_reg-100);
                fprintf(fp,"\tcmp\tr%d,r2\n",left_reg-100);
            }else{
                printf("\tcmp\tr%d,r2\n",left_reg);
                fprintf(fp,"\tcmp\tr%d,r2\n",left_reg);
            }
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        printf("\tvcvt.f32.s32\ts1,s1\n");
        fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",x2);
            printf("\tldr\tr2,=%s\n",arr1);
            fprintf(fp,"\tldr\tr2,=%s\n",arr1);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        char arr2[12]="0x";
        sprintf(arr2+2,"%0x",*xx2);
        printf("\tldr\tr2,=%s\n",arr2);
        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tcmp\tr%d,r%d\n",left_reg-100,right_reg-100);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tcmp\tr%d,r%d\n",left_reg-100,right_reg);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tcmp\tr%d,r%d\n",left_reg,right_reg-100);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg,right_reg-100);
        } else{
            printf("\tcmp\tr%d,r%d\n",left_reg,right_reg);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg,right_reg);
        }
    }
    if(isLocalVarIntType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
    }

    if(ins->inst->Opcode==LESS){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbge\tLABEL%d\n",x);
            fprintf(fp,"\tbge\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    } else if(ins->inst->Opcode==GREAT){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tble\tLABEL%d\n",x);
            fprintf(fp,"\tble\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    } else if(ins->inst->Opcode==LESSEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbgt\tLABEL%d\n",x);
            fprintf(fp,"\tbgt\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    } else if(ins->inst->Opcode==GREATEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tblt\tLABEL%d\n",x);
            fprintf(fp,"\tblt\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    } else if(ins->inst->Opcode==EQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbne\tLABEL%d\n",x);
            fprintf(fp,"\tbne\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    } else if(ins->inst->Opcode==NOTEQ){
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbeq\tLABEL%d\n",x);
            fprintf(fp,"\tbeq\tLABEL%d\n",x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            printf("\tb\tLABEL%d\n",x);
            fprintf(fp,"\tb\tLABEL%d\n",x);
        }
    }
    return ins;


}

InstNode * arm_trans_br_i1(InstNode *ins){
//    int i=ins->inst->i;
    int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
    printf("\tbne\tLABEL%d\n",x);
    fprintf(fp,"\tbne\tLABEL%d\n",x);
    x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("\tb\tLABEL%d\n",x);
    fprintf(fp,"\tb\tLABEL%d\n",x);
    return  ins;
}

InstNode * arm_trans_br(InstNode *ins){

    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("\tb\tLABEL%d\n",x);
    fprintf(fp,"\tb\tLABEL%d\n",x);
    return ins;
}

InstNode * arm_trans_br_i1_true(InstNode *ins){

    printf("arm_trans_br_i1_true\n");
    fprintf(fp,"arm_trans_br_i1_true\n");
    return ins;
}

InstNode * arm_trans_br_i1_false(InstNode *ins){

    printf("arm_trans_br_i1_false\n");
    fprintf(fp,"arm_trans_br_i1_false\n");
    return ins;
}

InstNode * arm_trans_Label(InstNode *ins){
//强制跳转的位置
    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("LABEL%d:\n",x);
    fprintf(fp,"LABEL%d:\n",x);
    return ins;
}

InstNode * arm_trans_tmp(InstNode *ins){

    printf("arm_trans_tmp\n");
    fprintf(fp,"arm_trans_tmp\n");
    return ins;
}

InstNode * arm_trans_XOR(InstNode *ins){
    printf("arm_trans_XOR\n");
    fprintf(fp,"arm_trans_XOR\n");
    return ins;
}

InstNode * arm_trans_zext(InstNode *ins){
//i1扩展为i32
    printf("arm_trans_zext\n");
    fprintf(fp,"arm_trans_zext\n");
    return ins;
}

InstNode * arm_trans_bitcast(InstNode *ins){
//类型转换，已经通过映射解决掉了bitcast产生的 多余的mov和load指令的问题
//    printf("arm_trans_bitcast\n");
    return ins;
}
void multiply_and_add_instructions_for_translated_arrays(InstNode*ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    int which_dimension=value0->pdata->var_pdata.iVal;
    int off=0;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];
    if(left_reg>100){
        int x=get_value_offset_sp(hashMap,value1);
        printf("\tldr\tr%d,");
        fprintf(fp,"\tldr\tr%d,");
    }

    return;
}
InstNode * arm_trans_GMP(InstNode *ins,HashMap*hashMap){
// 这个需要大改，ldr和str可以在这里处理掉，就不需要再去修改load和store对应的翻译了
// 使用乘加指令，首先需要记录当前所在的维数，这个lsy可能会存放在value0的ival里面(大于0就表示维数)
// 然后需要乘的数值在value2的ival里面，将其与后面维数的大小相乘(使用的是reg[2])
// 如果是常数的话，左值的ival放-1或者是-2，然后value2里面的ival存放的就是fixarray的最终结果
// 也就是直接给的相对于数组首地址的偏移量，不需要再进行相关的计算。
    Value *value0,*value1,*value2;
    int dest_reg,dest_reg_abs,left_reg=ins->inst->_reg_[1],right_reg;
    value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int off= get_value_offset_sp(hashMap,value1);
//    printf("off %d\n",off);
//   比如先把off装入%1对应的寄存器
    if(left_reg>100){
//        直接将%1的偏移量从内存取出
        int x= get_value_offset_sp(hashMap,value1);
        printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
        fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
    }else{
//        将其对应的偏移量直接移动到%1对应的寄存器
        if(imm_is_valid(off)){
            printf("\tmov\tr%d,#%d\n",left_reg,off);
            fprintf(fp,"\tmov\tr%d,#%d\n",left_reg,off);
        }else{
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",off);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tldr\tr%d,=%s\n",left_reg,arr1);
            fprintf(fp,"\tldr\tr%d,=%s\n",left_reg,arr1);
        }
    }

    while (1){
        value0=&ins->inst->user.value;
        value1= user_get_operand_use(&ins->inst->user,0)->Val;
        value2= user_get_operand_use(&ins->inst->user,1)->Val;
        dest_reg=ins->inst->_reg_[0];
        dest_reg_abs=abs(dest_reg);
        left_reg=ins->inst->_reg_[1];
        right_reg=ins->inst->_reg_[2];
        int flag=value0->pdata->var_pdata.iVal;
        if(flag<0){
//        偏移量可以直接计算出来,value2.iVal直接存放偏移量
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                x=value2->pdata->var_pdata.iVal;
                if(imm_is_valid(x)){
//                判断value.ival是否为合法立即数
                    printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x);
                    fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x);
                }else{
//                立即数为非法立即数
                    char arr1[12]="0x";
                    sprintf(arr1+2,"%0x",x);
                    printf("\tldr\tr2,=%s\n",arr1);
                    fprintf(fp,"\tldr\tr2,=%s\n",arr1);
                    printf("\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                    fprintf(fp,"\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                }
                if(dest_reg<0){
                    x= get_value_offset_sp(hashMap,value0);
                    printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                    fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                }
            }else{
                int x=value2->pdata->var_pdata.iVal;
                if(imm_is_valid(x)){
//                判断value.ival是否为合法立即数
                    printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x);
                    fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x);
                }else{
//                立即数为非法立即数
                    char arr1[12]="0x";
                    sprintf(arr1+2,"%0x",x);
                    printf("\tldr\tr2,=%s\n",arr1);
                    fprintf(fp,"\tldr\tr2,=%s\n",arr1);
                    printf("\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                    fprintf(fp,"\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                }
                if(dest_reg<0){
                    x= get_value_offset_sp(hashMap,value0);
                    printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                    fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                }
            }
        }else{
//        mla r0,r1,r2,r3                r1*r2+r3-->r0
//        这里的设计是reg1+reg2*r2=reg0
//        需要使用乘加指令进行翻译。需要用到reg0,reg1,reg2这三个寄存器,value1.alias为真正的数组，里面存了维数相关的信息
            int which_dimension=value0->pdata->var_pdata.iVal;//当前所在的维数
            int result= array_suffix(value1->alias,which_dimension);
//        还需要判断result是否为合法立即数
            if(imm_is_valid(result)){
                printf("\tmov\tr2,#%d\n",result);
                fprintf(fp,"\tmov\tr2,#%d\n",result);
            }else{
                char arr1[12]="0x";
                sprintf(arr1+2,"%0x",result);
                printf("\tldr\tr2,=%s\n",arr1);
                fprintf(fp,"\tldr\tr2,=%s\n",arr1);
            }
            if(left_reg>100&&right_reg>100){
                int x1= get_value_offset_sp(hashMap,value1);
                int x2= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg-100);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg-100);
            }else if(left_reg>100){
                int x1= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x1);
                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg-100);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg-100);
            }else if(right_reg>100){
                int x2= get_value_offset_sp(hashMap,value2);
                printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg);
            }else{
                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg);
            }
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        }
        if(get_next_inst(ins)->inst->Opcode!=GEP){
            break;
        }else{
            ins= get_next_inst(ins);
        }
    }

//    value1 存到value2对应的位置，需要判断value1和value2的类型
    if(get_next_inst((ins))->inst->Opcode==Store){
//        add r0,r2,#%d(off) 数组首地址的偏移量+数组内元素相对于数组首地址的偏移量就可以得到相对于帧指针fp的偏移量
//        str rd,[fp(r11),r0];
        InstNode *next= get_next_inst(ins);
        value0=&next->inst->user.value;//value0为空
        value1= user_get_operand_use(&next->inst->user,0)->Val;
        value2= user_get_operand_use(&next->inst->user,1)->Val;
        dest_reg=next->inst->_reg_[0];
//        这个记得改一下
        left_reg=next->inst->_reg_[1];//源
        right_reg=next->inst->_reg_[2];//保存的目的地

//        处理保存的地址,处理完之后，r0里面存放的是改数组元素相对于fp的偏移量[r11,r0]就可以了
        int right_reg_end;
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x);
            right_reg_end=right_reg-100;
        }else{
            right_reg_end=right_reg;
        }

//        printf("reg0:%d,reg1:%d,reg2:%d\n",dest_reg,left_reg,right_reg);

        int left_int_float=-1;
//        所以说使用的寄存器应该是value1和value2对应的寄存器,这个寄存器的分配结果好像是有点问题的，
//        现在是reg0对应value1
        if(isGlobalVarIntType(value1->VTy)){

        } else if(isGlobalVarFloatType(value1->VTy)){

        } else if(isGlobalArrayIntType(value1->VTy)){
            ;
        } else if(isGlobalArrayFloatType(value1->VTy)){
            ;
        } else if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
            printf("\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
            fprintf(fp,"\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
            printf("\tstr\tr1,[r11,r%d]\n",right_reg_end);
            fprintf(fp,"\tstr\tr1,[r11,r%d]\n",right_reg_end);
        }else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",value1->pdata->var_pdata.iVal);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tstr\tr1,[r11,r%d]\n",right_reg_end);
            fprintf(fp,"\tstr\tr1,[r11,r%d]\n",right_reg_end);
        }else if(isImmFloatType(value1->VTy)){
            float x2=value2->pdata->var_pdata.fVal;
            int *xx2=(int*)&x2;
            char arr1[12]="0x";
            sprintf(arr1+2,"%0x",*xx2);
            printf("\tldr\tr1,=%s\n",arr1);
            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tstr\tr1,[r11,r%d]\n",right_reg_end);
            fprintf(fp,"\tstr\tr1,[r11,r%d]\n",right_reg_end);
        }else if(isLocalVarIntType(value1->VTy)){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tstr\tr%d,[r11,r%d]\n",left_reg-100,right_reg_end);
                fprintf(fp,"\tstr\tr%d,[r11,r%d]\n",left_reg-100,right_reg_end);
            }else{
                printf("\tstr\tr%d,[r11,r%d]\n",left_reg,right_reg_end);
                fprintf(fp,"\tstr\tr%d,[r11,r%d]\n",left_reg,right_reg_end);
            }
        }else if(isLocalVarFloatType(value1->VTy)){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tstr\tr%d,[r11,r%d]\n",left_reg-100,right_reg_end);
                fprintf(fp,"\tstr\tr%d,[r11,r%d]\n",left_reg-100,right_reg_end);
            }else{
                printf("\tstr\tr%d,[r11,r%d]\n",left_reg,right_reg_end);
                fprintf(fp,"\tstr\tr%d,[r11,r%d]\n",left_reg,right_reg_end);
            }
        }else if(isLocalArrayIntType(value1->VTy)){

        }else if(isLocalVarFloatType(value1->VTy)){

        }
        return next;
    }
    if(get_next_inst((ins))->inst->Opcode==Load){
//        load的左值和右值都必然是变量,而且右值再内存中的偏移量已经再GEP指令中计算好了
//        这里只需要确认一下,left_reg>100?就可以了
        InstNode *next= get_next_inst(ins);
        value0=&next->inst->user.value;
        value1= user_get_operand_use(&next->inst->user,0)->Val;
        dest_reg=next->inst->_reg_[0];//加载到的位置
        dest_reg_abs=abs(dest_reg);
        left_reg=next->inst->_reg_[1];//被加载的值
//        printf("reg0:%d,reg1:%d,reg2:%d\n",dest_reg,left_reg,right_reg);
        if(left_reg>100){
            if(isLocalVarIntType(value0->VTy)&& isLocalVarIntType(value1->VTy)){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
            }else if(isLocalVarFloatType(value0->VTy)&& isLocalVarIntType(value1->VTy)){
                ;
            }else if(isLocalVarIntType(value0->VTy)&& isLocalVarFloatType(value1->VTy)){
                ;
            }else if(isLocalVarFloatType(value0->VTy)&& isLocalVarFloatType(value1->VTy)){
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
            }else{
//            这里还会出现unknown的情况
                int x= get_value_offset_sp(hashMap,value1);
                printf("\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",left_reg-100,x);
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg-100);
            }
        }else{
            if(isLocalVarIntType(value0->VTy)&& isLocalVarIntType(value1->VTy)){
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
            }else if(isLocalVarFloatType(value0->VTy)&& isLocalVarIntType(value1->VTy)){
                ;
            }else if(isLocalVarIntType(value0->VTy)&& isLocalVarFloatType(value1->VTy)){
                ;
            }else if(isLocalVarFloatType(value0->VTy)&& isLocalVarFloatType(value1->VTy)){
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
            }else{
//            这里还会出现unknown的情况
                printf("\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",dest_reg_abs,left_reg);
            }
        }
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }
        return next;
    }
//    value1 load 到 value0
//    if(get_next_inst((ins))->inst->Opcode==Load){
////        如果是load的话，我需要考虑将对应的值load到哪个寄存器
//        InstNode *next= get_next_inst(ins);
//        int x= get_value_offset_sp(hashMap,&next->inst->user.value);
//        printf("    ldr r1,[sp,#%d]\n",off_sp);
//        printf("    str r1,[sp,#%d]\n",x);
//        return next;
//    }
//    int which_dimension=value0->pdata->var_pdata.iVal;
//    if(which_dimension==-1){
////        表示为常数的情况，这里是只需要使用一个加法指令就可以了,直接在这里处理就可以了
//    }else{
////        表示非常数的情况，所以说需要使用乘加指令
//        while (ins->inst->Opcode==GEP){
//            multiply_and_add_instructions_for_translated_arrays(ins,hashMap);
//            ins= get_next_inst(ins);
//        }
//    }
//  把GEP指令翻译完之后，下面就是处理后继的load/store指令了。

// 数组初始化
// 获取栈帧首地址使用的是%i，不需要进到alias中
//    Value *array= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    for(;get_next_inst(ins)->inst->Opcode == GEP;){
//        ins= get_next_inst(ins);
////        Value *value0=&ins->inst->user.value;
////        Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
////        Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
////        printf("GEP test\n");
//    }
////    这里得到最后一条GMP指令
//    int off_sp= get_value_offset_sp(hashMap,array)+ins->inst->user.value.pdata->var_pdata.iVal*4;
////    Value *value0=&ins->inst->user.value;
////    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
////    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    if(get_next_inst((ins))->inst->Opcode==Store){
//        InstNode *next= get_next_inst(ins);
////        int off= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
//        int x= user_get_operand_use(&next->inst->user,0)->Val->pdata->var_pdata.iVal;
//        printf("    mov r0,#%d\n",x);
////        int off_sp= get_value_offset_sp(hashMap, user_get_operand_use(&next->inst->user,1)->Val->alias)+off;
//        printf("    str r0,[sp,#%d]\n",off_sp);
//        return next;
//    }
//    if(get_next_inst((ins))->inst->Opcode==Load){
////        如果是load的话，我需要考虑将对应的值load到哪个寄存器
//        InstNode *next= get_next_inst(ins);
//        int x= get_value_offset_sp(hashMap,&next->inst->user.value);
//        printf("    ldr r1,[sp,#%d]\n",off_sp);
//        printf("    str r1,[sp,#%d]\n",x);
//        return next;
//    }

//*************************************************************************
//这里是在数组处理的store指令没有被删除的时候的处理方式,理论上来说
//    if(get_next_inst((ins))->inst->Opcode==Store){
//        InstNode *next= get_next_inst(ins);
//        int off= user_get_operand_use(&ins->inst->user,0)->Val->pdata->var_pdata.iVal;
//        int x= user_get_operand_use(&next->inst->user,0)->Val->pdata->var_pdata.iVal;
//        printf("    mov r0,#%d\n",x);
//        int off_sp= get_value_offset_sp(hashMap, user_get_operand_use(&next->inst->user,1)->Val->alias)+off;
//        printf("    store r0,[sp,#%d]\n",off_sp*4);
//        return next;
//    }



//    printf("\n");
    return ins;
}

InstNode * arm_trans_MEMCPY(InstNode *ins){

//    涉及到数组的内容,后端不需要翻译这条ir和memcpy对应的ir
//    printf("arm_trans_MEMCPY\n");
    return ins;
}

InstNode * arm_trans_zeroinitializer(InstNode *ins){

    printf("arm_trans_zeroinitializer\n");
    fprintf(fp,"arm_trans_zeroinitializer\n");
    return ins;
}

InstNode * arm_trans_GLOBAL_VAR(InstNode *ins){

//全局变量声明
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    if(isGlobalVarIntType(value1->VTy)){
        char name[270];
        sprintf(name,"%s:",value1->name+1);
        strcat(globalvar_message,name);
        strcat(globalvar_message,"\n\t.long\t");
        char value_int[12];
        sprintf(value_int,"%d",value1->pdata->var_pdata.iVal);
        strcat(globalvar_message,value_int);
        strcat(globalvar_message,"\n");
        
    } else if(isGlobalVarFloatType(value1->VTy)){
        char name[270];
        sprintf(name,"%s:",value1->name+1);
        strcat(globalvar_message,name);
        strcat(globalvar_message,"\n\t.long\t");

        char value_int[12]="0x";
        float x=value1->pdata->var_pdata.fVal;
        int xx=*(int*)&x;
        sprintf(value_int,"%0x",xx);
        strcat(globalvar_message,value_int);
        strcat(globalvar_message,"\n");

    } else if(isGlobalArrayIntType(value1->VTy)){

    } else if(isGlobalArrayFloatType(value1->VTy)){

    }

//    printf("arm_trans_GLOBAL_VAR\n");
    return ins;
}

InstNode *arm_trans_Phi(InstNode *ins){

//    printf("arm_trans_Phi\n");
    return ins;
}

InstNode *arm_trans_MEMSET(InstNode *ins){

//    printf("arm_trans_MEMSET\n");
    return ins;
}
InstNode * arm_trans_Store(InstNode *ins,HashMap *hashMap){
//    value1是要保存的东西，一般通运算之后保存的，是保存在add等指令的左值，
//    所以是应该在add指令的左值判断该变量是否为全局变量，而不是在store指令里面执行吧
//    不对，在全局变量的add之后，会有一条store %i @a这样的指令
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    value2是要保存到的地方
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int left_reg=ins->inst->_reg_[1];
    int left_int_float=-1;
    if(isGlobalVarIntType(value1->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
        printf("\tldr\tr1,[r1]\n");
        fprintf(fp,"\tldr\tr1,[r1]\n");
        left_int_float=0;
    } else if(isGlobalVarFloatType(value1->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
        printf("\tldr\tr1,[r1]\n");
        fprintf(fp,"\tldr\tr1,[r1]\n");
        left_int_float=1;
    } else if(isGlobalArrayIntType(value1->VTy)){
        ;
    } else if(isGlobalArrayFloatType(value1->VTy)){
        ;
    } else if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
        printf("\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
        fprintf(fp,"\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
        left_int_float=0;
    }else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
        char arr[12]="0x";
        int xx=value1->pdata->var_pdata.iVal;
        sprintf(arr+2,"%0x",xx);
        printf("\tldr\tr1,=%s\n",arr);
        fprintf(fp,"\tldr\tr1,=%s\n",arr);
        left_int_float=0;
    }else if(isImmFloatType(value1->VTy)){
        char arr[12]="0x";
        float x=value1->pdata->var_pdata.fVal;
        int xx=*(int*)&x;
        sprintf(arr+2,"%0x",xx);
        printf("\tldr\tr1,=%s\n",arr);
        fprintf(fp,"\tldr\tr1,=%s\n",arr);
        left_int_float=1;
    }else if(isLocalVarIntType(value1->VTy)){
        left_int_float=0;
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,%d]\n",left_reg-100,x);
            printf("\tmov\tr1,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg-100);
        }else{
            printf("\tmov\tr1,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg);
        }

    }else if(isLocalVarFloatType(value1->VTy)){
        left_int_float=1;
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            printf("\tldr\tr%d,[r11,%d]\n",left_reg-100,x);
            fprintf(fp,"\tldr\tr%d,[r11,%d]\n",left_reg-100,x);
            printf("\tmov\tr1,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg-100);
        }else{
            printf("\tmov\tr1,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg);
        }
    }

//    value2
    if(isGlobalVarIntType(value2->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value2);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr2,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr2,%s\n",lcptLabel->LCPI);
        if(left_int_float==0){
            printf("\tstr\tr1,[r2]\n");
            fprintf(fp,"\tstr\tr1,[r2]\n");
        }else{
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.s32.f32\ts1,s1\n");
            fprintf(fp,"\tvcvt.s32.f32\ts1,s1\n");
            printf("\tvmov\tr1,s1\n");
            fprintf(fp,"\tvmov\tr1,s1\n");
            printf("\tstr\tr1,[r2]\n");
            fprintf(fp,"\tstr\tr1,[r2]\n");
        }

    } else if(isGlobalVarFloatType(value2->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value2);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr2,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr2,%s\n",lcptLabel->LCPI);
        if(left_int_float==1){
            printf("\tstr\tr1,[r2]\n");
            fprintf(fp,"\tstr\tr1,[r2]\n");
        }else{
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\tr1,s1\n");
            fprintf(fp,"\tvmov\tr1,s1\n");
            printf("\tstr\tr1,[r2]\n");
            fprintf(fp,"\tstr\tr1,[r2]\n");
        }

    } else if(isGlobalArrayIntType(value1->VTy)){
        ;
    } else if(isGlobalArrayFloatType(value1->VTy)){
        ;
    }
    return ins;
}
InstNode * arm_trans_Load(InstNode *ins,HashMap *hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs=abs(dest_reg);
    if(isGlobalVarIntType(value1->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
        if(dest_reg<0){
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
        }


    } else if(isGlobalVarFloatType(value1->VTy)){
        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
        if(lcptLabel==NULL){
            printf("HashMapGet(global_hashmap,value1); error\n");
//            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
        }
        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
//        printf("\tvldr\ts1,[r1]\n");
//        int x= get_value_offset_sp(hashMap,value0);
//        printf("\tvstr\ts1,[sp,%d]\n",x);
        if(dest_reg<0){
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
            int x= get_value_offset_sp(hashMap,value0);
            printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
        }else{
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
        }
    } else if(isGlobalArrayIntType(value1->VTy)){
        ;
    } else if(isGlobalArrayFloatType(value1->VTy)){
        ;
    }
    return ins;
}

InstNode *_arm_translate_ins(InstNode *ins,InstNode *head,HashMap*hashMap,int stack_size){

    int x=ins->inst->Opcode;
    switch(x){
        case Add:
            return arm_trans_Add(ins,hashMap);
        case Sub:
            return arm_trans_Sub(ins,hashMap);
        case Mul:
            return arm_trans_Mul(ins,hashMap);
        case Div:
            return arm_trans_Div(ins,hashMap);
        case Mod:
            return arm_trans_Module(ins,hashMap);
        case Call:
//            在进行call之前是需要至少保存lr寄存器的，call调用结束之后还需要将lr出栈恢复
            return arm_trans_Call(ins,hashMap);
//        case FunBegin:
//            return arm_trans_FunBegin(ins,hashMap);
        case Return:
            return arm_trans_Return(ins,head,hashMap,stack_size);
        case Store:
            return arm_trans_Store(ins,hashMap);

            printf("    store\n");
            fprintf(fp,"    store\n");
            return ins;
        case Load:
            return arm_trans_Load(ins,hashMap);
            printf("    load\n");
            fprintf(fp,"    load\n");
            return ins;
        case Alloca:
            return arm_trans_Alloca(ins,hashMap);
        case GIVE_PARAM:
            params[give_count++]=ins;
//            return arm_trans_GIVE_PARAM(ins,hashMap);
            return ins;
        case ALLBEGIN:
            return arm_trans_ALLBEGIN(ins);
        case LESS:
        case GREAT:
        case LESSEQ:
        case GREATEQ:
        case EQ:
        case NOTEQ:
            return arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(ins,hashMap);
        case br_i1:
            return arm_trans_br_i1(ins);
        case br:
            return arm_trans_br(ins);
        case br_i1_true:
            return arm_trans_br_i1_true(ins);
        case br_i1_false:
            return arm_trans_br_i1_false(ins);
        case Label:
            return arm_trans_Label(ins);
        case tmp:
            return arm_trans_tmp(ins);
        case XOR:
            return arm_trans_XOR(ins);
        case zext:
            return arm_trans_zext(ins);
        case bitcast:
            return arm_trans_bitcast(ins);
        case GEP:
            return arm_trans_GMP(ins,hashMap);
        case MEMCPY:
            return arm_trans_MEMCPY(ins);
        case zeroinitializer:
            return arm_trans_zeroinitializer(ins);
        case GLOBAL_VAR:
            return arm_trans_GLOBAL_VAR(ins);
        case Phi:
            return arm_trans_Phi(ins);
        case MEMSET:
            return arm_trans_MEMSET(ins);
        case CopyOperation:
            return arm_trans_CopyOperation(ins,hashMap);
        default:
            return ins;
    }
}
