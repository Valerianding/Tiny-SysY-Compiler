//
// Created by ljf on 23-2-25.
//

#include "arm.h"

#define AND_LOW 65535
#define MOVE_RIGHT 16

#define lineScan 1 //使用线性扫描寄存器分配
HashMap * lineScan_param;


//ri
int regi=0;
//si
int regs=0;
//优化开关1打开
int optimization=1;  //优化总开关
int opt_div2=1;      //除以2幂次
int opt_mod2=1;      //取余2幂次
int opt_mul=1;       //乘法优化
int opt_copy=0 ;     //copy_operation优化，注意这个只能在使用块内寄存器分配时使用
int opt_label=1 ;    //label优化


//记录本函数调用的函数个数
int func_call_func;
//神奇的数组
extern InstNode *one_param[];
extern InstNode *params[];
extern HashMap *global_hashmap;
int give_count=0;

int globalvar_num;
//定义数据段的全局变量的信息
char globalvar_message[100000];

//存放打开文件的FILE指针
FILE *fp;
//记录需要保护的寄存器，1表示需要保护
int reg_save[16];
//记录r0-r3在FuncBegin的时候str时候的偏移量
int param_off[4];
char fileName[256];
char funcName[256];
int save_r11;
int global_flag=0;
int give_param_num;
Value *func_return_type=NULL; //用来进行函数return返回值类型转换
Value *func_param_type=NULL; //用来进行函数调用和接受类型转换
//用来记录开辟的文字池
int ltorg_num=0;
int give_param_flag[4];
char return_message[100000];
int stm_num; //8字节对齐
//现在reg1，reg2会出现负数的情况，就是说右边的寄存器在用完之后就回存到内存，腾出寄存器


void handle_lineScan_extra_reg(InstNode*ins,int param_num){
    Function *curFunction=ins->inst->Parent->Parent;
    Pair *ptr_pair;
    HashMapFirst(curFunction->lineScanReg);
    while ((ptr_pair= HashMapNext(curFunction->lineScanReg))!=NULL){
        Value *value=(Value*)ptr_pair->key;
        value_register *r=(value_register*)ptr_pair->value;
        int num=-1;
        assert(value->name!=NULL);
        if(value->name[0]=='%'){
            num=atoi(value->name+1);
            if(num>=4 && num<param_num){
//                多余的参数被分配了寄存器,需要在函数开始前就ldr到相应的寄存器
                offset *node= HashMapGet(lineScan_param,value);
                assert(node!=NULL);
                handle_illegal_imm(r->reg+100,node->offset_sp,1);
            }
        }
    }
}
//eabi不会保存r12
void printf_stmfd_rlist(){
//    printf();
//    fprintf(fp,);
    if(save_r11==1){
        reg_save[11]=1;
    }
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
//        strcat(return_message,"\tldr\tlr,[sp],#4\n");
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
//    strcat(return_message,"\tldmfd\tsp!,{");
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
    return result * 4;
}

//bool imm_is_valid(unsigned int imm){
//    int i;
//    for (i = 0; i <= 30; i += 2) {
//        unsigned int rotated = (imm >> i) | (imm << (32 - i));
//        if (rotated <= 255 && (rotated & ~0xff) == 0)
//            return true;
//    }
//    return false;
//}
bool imm_is_valid(int x)
{
    if(x>=0 && x<=255){
        return true;
    }
    for (int i = 0; i < 32; i += 2)
    {
        int rotated = (x << i) | (x >> (32 - i));
        if (rotated >= 0 && rotated <= 255)
        {
//            printf("%d is valid\n",x);
            return true;
        }
    }
//    printf("%d is not valid\n",x);
    return false;
}
//bool imm_is_valid(int x) { //int版本,有问题
//    for (int i = 0; i < 32; i += 2) {
//        int rotated = x;
//        rotated = (rotated << i) | (rotated >> (32 - i));  // Left rotation by i bits.
//        if (rotated >= -128 && rotated <= 127) {  // Can the value be represented with 8 bits?
//            return true;
//        }
//    }
//    return false;
//}

bool imm_is_valid2(int value){
    if(value>=-255 && value<=4095){
        return true;
    }else{
        return false;
    }
}
int power_of_two(int n){ //需要传入一个正数
    if (n <= 0 || (n & (n - 1)) != 0) {
        return -1;
    }
    int power = 0;
    while (n > 1) {
        n = n >> 1;
        power++;
    }
    return power;
}
int get_free_reg(){
    if(give_param_flag[0]==0){
        return 0;
    }
    for(int i=0;i<=3;i++){
        if(give_param_flag[i]==0){
            return i;
        }
    }
    assert(false);
//    如果都占用了，那没办法
}
void handle_illegal_imm1(int dest_reg,int x){
    if(x>=0&&x<=AND_LOW){//16位bit即可表示下，则只需要用到movw,但是一样需要用到movt,因为寄存器如果本来高16位如果为零的或就错了
        char arr1[12]="#0x";
        sprintf(arr1+3,"%0x",x&AND_LOW);
        printf("\tmovw\tr%d,%s\n",dest_reg,arr1);
        fprintf(fp,"\tmovw\tr%d,%s\n",dest_reg,arr1);
        printf("\tmovt\tr%d,#0x0\n",dest_reg);
        fprintf(fp,"\tmovt\tr%d,#0x0\n",dest_reg);
    }else{
        //负数,大于16位
        char arr1[12]="#0x";
        sprintf(arr1+3,"%0x",x&AND_LOW);
        printf("\tmovw\tr%d,%s\n",dest_reg,arr1);
        fprintf(fp,"\tmovw\tr%d,%s\n",dest_reg,arr1);

        char arr2[12]="#0x";
        sprintf(arr2+3,"%0x",(x >> MOVE_RIGHT)&0xffff); //修改过，将unsigned去掉
        printf("\tmovt\tr%d,%s\n",dest_reg,arr2);
        fprintf(fp,"\tmovt\tr%d,%s\n",dest_reg,arr2);
    }
}
void handle_illegal_imm(int handle_dest_reg ,int x,int flag){
    if(give_param_flag[3]==0){
        if(flag==0){ //str dest_reg_abs
            if(imm_is_valid2(x)){
                printf("\tstr\tr%d,[r11,#%d]\n",handle_dest_reg,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",handle_dest_reg,x);
            }else {
                handle_illegal_imm1(3,x);

                printf("\tstr\tr%d,[r11,r3]\n", handle_dest_reg);
                fprintf(fp, "\tstr\tr%d,[r11,r3]\n", handle_dest_reg);
            }
        }else if(flag==1){ //ldr left_reg-100
            if(imm_is_valid2(x)){
                printf("\tldr\tr%d,[r11,#%d]\n",handle_dest_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",handle_dest_reg-100,x);
            } else{
                handle_illegal_imm1(3,x);

                printf("\tldr\tr%d,[r11,r3]\n",handle_dest_reg-100);
                fprintf(fp,"\tldr\tr%d,[r11,r3]\n",handle_dest_reg-100);
            }
        }else { //ldr right_reg-100
            if (imm_is_valid2(x)) {
                printf("\tldr\tr%d,[r11,#%d]\n", handle_dest_reg - 100, x);
                fprintf(fp, "\tldr\tr%d,[r11,#%d]\n", handle_dest_reg - 100, x);
            } else {
                handle_illegal_imm1(3, x);

                printf("\tldr\tr%d,[r11,r3]\n", handle_dest_reg - 100);
                fprintf(fp, "\tldr\tr%d,[r11,r3]\n", handle_dest_reg - 100);
            }
        }
    }else{
//        r3寄存器用于传参，并且已经被占用,获取一个可用寄存器
        int tmp_reg=get_free_reg();
        if(flag==0){ //str dest_reg_abs
            if(imm_is_valid2(x)){
                printf("\tstr\tr%d,[r11,#%d]\n",handle_dest_reg,x);
                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",handle_dest_reg,x);
            }else {
                handle_illegal_imm1(tmp_reg,x);

                printf("\tstr\tr%d,[r11,r%d]\n", handle_dest_reg,tmp_reg);
                fprintf(fp, "\tstr\tr%d,[r11,r%d]\n", handle_dest_reg,tmp_reg);
            }
        }else if(flag==1){ //ldr left_reg-100
            if(imm_is_valid2(x)){
                printf("\tldr\tr%d,[r11,#%d]\n",handle_dest_reg-100,x);
                fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",handle_dest_reg-100,x);
            } else{
                handle_illegal_imm1(tmp_reg,x);

                printf("\tldr\tr%d,[r11,r%d]\n",handle_dest_reg-100,tmp_reg);
                fprintf(fp,"\tldr\tr%d,[r11,r%d]\n",handle_dest_reg-100,tmp_reg);
            }
        }else { //ldr right_reg-100
            if (imm_is_valid2(x)) {
                printf("\tldr\tr%d,[r11,#%d]\n", handle_dest_reg - 100, x);
                fprintf(fp, "\tldr\tr%d,[r11,#%d]\n", handle_dest_reg - 100, x);
            } else {
                handle_illegal_imm1(tmp_reg, x);

                printf("\tldr\tr%d,[r11,r%d]\n", handle_dest_reg - 100,tmp_reg);
                fprintf(fp, "\tldr\tr%d,[r11,r%d]\n", handle_dest_reg - 100,tmp_reg);
            }
        }
    }

}

void int_to_float(int si,int ri){
    printf("\tvmov\ts%d,r%d\n",si,ri);
    fprintf(fp,"\tvmov\ts%d,r%d\n",si,ri);
    printf("\tvcvt.f32.s32\ts%d,s%d\n",si,si);
    fprintf(fp,"\tvcvt.f32.s32\ts%d,s%d\n",si,si);
    printf("\tvmov\tr%d,s%d\n",ri,si);
    fprintf(fp,"\tvmov\tr%d,s%d\n",ri,si);
}
void float_to_int(int si,int ri){
    printf("\tvmov\ts%d,r%d\n",si,ri);
    fprintf(fp,"\tvmov\ts%d,r%d\n",si,ri);
    printf("\tvcvt.s32.f32\ts%d,s%d\n",si,si);
    fprintf(fp,"\tvcvt.s32.f32\ts%d,s%d\n",si,si);
    printf("\tvmov\tr%d,s%d\n",ri,si);
    fprintf(fp,"\tvmov\tr%d,s%d\n",ri,si);
}
void int_to_float2(int si,int srcri,int destri){
    printf("\tvmov\ts%d,r%d\n",si,srcri);
    fprintf(fp,"\tvmov\ts%d,r%d\n",si,srcri);
    printf("\tvcvt.f32.s32\ts%d,s%d\n",si,si);
    fprintf(fp,"\tvcvt.f32.s32\ts%d,s%d\n",si,si);
    printf("\tvmov\tr%d,s%d\n",destri,si);
    fprintf(fp,"\tvmov\tr%d,s%d\n",destri,si);
}
void float_to_int2(int si,int srcri,int destri){
    printf("\tvmov\ts%d,r%d\n",si,srcri);
    fprintf(fp,"\tvmov\ts%d,r%d\n",si,srcri);
    printf("\tvcvt.s32.f32\ts%d,s%d\n",si,si);
    fprintf(fp,"\tvcvt.s32.f32\ts%d,s%d\n",si,si);
    printf("\tvmov\tr%d,s%d\n",destri,si);
    fprintf(fp,"\tvmov\tr%d,s%d\n",destri,si);
}
bool is_int_array(Value *value){
    Value *value_alias = value->alias;
    if(value_alias->VTy->ID==AddressTyID && value_alias->pdata->symtab_array_pdata.address_type == 0){
        return true;
    }else if( (value_alias->VTy->ID==ArrayTy_INT) || (value_alias->VTy->ID==ArrayTyID_ConstINT) || (value_alias->VTy->ID==GlobalArrayConstINT) || (value_alias->VTy->ID==GlobalArrayInt) ){
        return true;
    }
    return  false;
}
bool is_float_array(Value *value){
    Value *value_alias=value->alias;
    if(value_alias->VTy->ID==AddressTyID && value_alias->pdata->symtab_array_pdata.address_type==1 ){
        return true;
    }
    else if( (value_alias->VTy->ID==ArrayTy_FLOAT) || (value_alias->VTy->ID==ArrayTyID_ConstFLOAT) || (value_alias->VTy->ID==GlobalArrayConstFLOAT) || (value_alias->VTy->ID==GlobalArrayFloat) ){
        return true;
    }

    return  false;
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
//    char new_ext[] = ".s";
//    char *dot_ptr = strrchr(filename, '.');
//    if(dot_ptr) {
//        int basename_len = dot_ptr - filename;  // 计算基本文件名的长度
//        char new_filename[basename_len + strlen(new_ext) + 1];  // 为新文件名分配足够的空间
//
//        strncpy(new_filename, filename, basename_len);  // 复制基本文件名
//        new_filename[basename_len] = '\0';  // 在基本文件名后添加空字符
//        strcat(new_filename, new_ext);  // 连接新的扩展名
////        打开文件
//        fp= fopen(new_filename,"w");
//        printf("new_filename %s\n",new_filename);
//    } else {
//        printf("输入文件名有误\n");
//    }
    char new_ext[260];
    strcpy(new_ext,filename);
    fp= fopen(new_ext,"w");
    return;
}
void arm_close_file(){
    fclose(fp);
    fp=NULL;
    return;
}
int get_value_offset_sp(HashMap *hashMap,Value*value){
//    printf("find %s\n",value->name);
    offset *node= HashMapGet(hashMap, value);
    if(node!=NULL) {
//        printf("ldr %s\n",value->name);
//        printf("get_value_offset_sp %d\n",node->offset_sp);
        return node->offset_sp;

    }
    if(node==NULL&&(isImmFloatType(value->VTy)|| isImmIntType(value->VTy))){
//        printf("this is imm,can't find in stack!!!");
    }
//    if(node==NULL){
//        printf("not find %s\n",value->name);
//        printf("%p\n",value);
//    }

//    if(value == NULL){printf("NULLL!\n");}
//    else{
//        printf("value can't find %s\n",value->name);
//    }
//    assert(false);
    return -1;
}

void FuncBegin_hashmap_add(HashMap*hashMap,Value *value,char *name,int *local_stack,int reg_flag){
//    reg_flag=-1;
// 这种存储方法好像是错的，因为后面用到的同一个value，这里给他分配了不同地址的mykey_node()，所以回得到不的key
// hashmap不会释放value*对应的内存
    char param_name[5]="%4";
//    if(value->VTy->ID==ArrayTyID){
////        是数组，offset_sp需要加更多
//    }
//    是参数,不用处理,这里是用来计算局部变量的栈开多大，再加上param_num*4就可以得到总的栈帧的大小了。
//    这里和之前的设计不同的是，r0-r3转递过来的参数也是当局部变量来处理的，但是超出r3的其他变量会在父函数give_param翻译的时候就直接str并且改变sp
//    所以说这里不需要为多余的参数再开辟空间，所以说这里是需要处理r0-r3这几个参数的就是对应%0-%3需要开辟局部变量空间

// 这里面的判断逻辑好像是有问题的,换一种方法，就是直接将%i生成数字i再进行比较
    int value_name_num=-1;
    int name_num= atoi(name+1);
    int param_name_num= atoi(param_name+1);
//    这里好像有问题，就是函数名只有一个字母的情况，get_one,名字只有一个g,g+1进行atoi就会得到0，所以需要判断第一个字母是不是%,比如全局变量
    if(value->name!=NULL && value->name[0]=='%'){
        value_name_num= atoi(value->name+1);
    }
//    if(value->name!=NULL && strcmp(value->name,name)<0 && strlen(value->name)<= strlen(name)){
//        if(strcmp(value->name,param_name)<0&& strlen(value->name)<= strlen(param_name)){
    if(value_name_num!=-1 && value_name_num<name_num){
//        是参数
        if(value_name_num<param_name_num){
//            为r0-r3对应的参数
//这个HashMapContian好像是有问题的，但是所有的问题都应该是没有进行hashPut
            if(!HashMapContain(hashMap, value)){
                int x= atoi((value->name)+1);
                offset *node=offset_node();
                node->memory=1;
                node->regr=-1;
                node->regs=-1;
                node->offset_sp=*local_stack;
                param_off[x]=node->offset_sp;
//                printf("param_off[%d]=%d\n",x,*local_stack);
                (*local_stack)+=4;
//                printf("funcbegin %s %d\n",value->name,node->offset_sp);
                HashMapPut(hashMap,value,node);
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
        if(reg_flag<0 && !HashMapContain(hashMap, value)){
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
//            printf("funcbegin %s %d\n",value->name,node->offset_sp);
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
            assert(false);
//            这个应该是处理其他的情况，比如说是unknown的情况,这里应该是存在的
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
    if(!HashMapContain(hashMap,value0)){ //这里应该是只进入一层就够了，因为每一层的node都是一样的，就没有必要追溯到第一层
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
void arm_translate_ins(InstNode *ins,char argv[]){
//    global_hashmap=HashMapInit();
    char new_ext[] = ".c";
    char *dot_ptr = strrchr(argv, '.');
    if(dot_ptr) {
        int basename_len = dot_ptr - argv;  // 计算基本文件名的长度
        char new_filename[basename_len + strlen(new_ext) + 1];  // 为新文件名分配足够的空间

        strncpy(new_filename, argv, basename_len);  // 复制基本文件名
        new_filename[basename_len] = '\0';  // 在基本文件名后添加空字符
        strcat(new_filename, new_ext);  // 连接新的扩展名
        strcpy(fileName,new_filename);
    }
//    memset(fileName,0,sizeof(fileName));
//    strcpy(fileName,argv);


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
            stm_num=0;
            global_hashmap=HashMapInit();
            lineScan_param=HashMapInit();
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
            if(func_call_func>0){
                k++;
            }
            hashMap=offset_init(ins,&local_var_num,k,lineScan_param);

//            如果使用了线性扫描，需要处理多余的寄存器,将多余的寄存器提前加载到给对应参数分配的寄存器中
            if(lineScan==1){
                handle_lineScan_extra_reg(ins,param_num);
            }

            int hashsize= HashMapSize(hashMap)*4;
//            printf("hashsize=%d\n",hashsize);

            func_return_type= user_get_operand_use(&ins->inst->user,0)->Val;

            ins= get_next_inst(ins);
        }
        ins=_arm_translate_ins(ins,head,hashMap,stack_size);
        if(ins->inst->Opcode==FunEnd){ //这里应该FunEnd再销毁hashMap,而不是RETURN,因为RETURN之后有可能还有语句
            offset_free(hashMap);
//            将全局变量的使用打印
//            usage_of_global_variables();
            HashMapDeinit(global_hashmap);
            HashMapDeinit(lineScan_param);
            global_hashmap=NULL;
            hashMap=NULL;
        }
    }
//    HashMapDeinit(hashMap);
    printf("%s\n",globalvar_message);
    fprintf(fp,"%s\n",globalvar_message);
    return;
}
InstNode *arm_trans_fptosi(HashMap *hashMap,InstNode *ins){
//    float强制转换为intvalue1 --> value0,现在的情况是value1有可能是立即数，也可能是变量
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int left_reg=ins->inst->_reg_[1];
    int left_reg_abs;
    int dest_reg_abs=abs(dest_reg);
//    如果说是立即数的话，我是分配哪个寄存器来接受呢？这里应该不影响的,现在就先用一个固定的r0
//    if(isImmFloatType(value1->VTy)){
//        assert(false);
//        float fx=value1->pdata->var_pdata.fVal;
//        int x=*(int*)(&fx);
//        handle_illegal_imm1(0,x);
//        left_reg_abs=0;
//    }
//    else
    if(isLocalVarFloatType(value1->VTy)){
        assert(left_reg!=0);
        if(left_reg>100){
            left_reg_abs=left_reg-100;
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
        }else{
            left_reg_abs=left_reg;
        }
        printf("\tvmov\ts%d,r%d\n",dest_reg_abs,left_reg_abs);
        fprintf(fp,"\tvmov\ts%d,r%d\n",dest_reg_abs,left_reg_abs);
        printf("\tvcvt.s32.f32\ts%d,s%d\n",dest_reg_abs,dest_reg_abs);
        fprintf(fp,"\tvcvt.s32.f32\ts%d,s%d\n",dest_reg_abs,dest_reg_abs);
        printf("\tvmov\tr%d,s%d\n",dest_reg_abs,dest_reg_abs);
        fprintf(fp,"\tvmov\tr%d,s%d\n",dest_reg_abs,dest_reg_abs);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);
        }
    }
//    else{
//        assert(false);
//    }

    return ins;
}
InstNode *arm_trans_sitofp(HashMap *hashMap,InstNode *ins){
//    float强制转换为intvalue1 --> value0,现在的情况是value1有可能是立即数，也可能是变量
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int left_reg=ins->inst->_reg_[1];
    int left_reg_abs;
    int dest_reg_abs=abs(dest_reg);
//    如果说是立即数的话，我是分配哪个寄存器来接受呢？这里应该不影响的,现在就先用一个固定的r0

//    if(isImmIntType(value1->VTy)){
//        assert(false);
//        int x=value1->pdata->var_pdata.iVal;
//        handle_illegal_imm1(0,x);
//        left_reg_abs=0;
//    }
//    else
    if(isLocalVarIntType(value1->VTy)){
        assert(left_reg!=0);
        if(left_reg>100){
            left_reg_abs=left_reg-100;
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
        }else{
            left_reg_abs=left_reg;
        }
        printf("\tvmov\ts%d,r%d\n",dest_reg_abs,left_reg_abs);
        fprintf(fp,"\tvmov\ts%d,r%d\n",dest_reg_abs,left_reg_abs);
        printf("\tvcvt.f32.s32\ts%d,s%d\n",dest_reg_abs,dest_reg_abs);
        fprintf(fp,"\tvcvt.f32.s32\ts%d,s%d\n",dest_reg_abs,dest_reg_abs);
        printf("\tvmov\tr%d,s%d\n",dest_reg_abs,dest_reg_abs);
        fprintf(fp,"\tvmov\tr%d,s%d\n",dest_reg_abs,dest_reg_abs);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);
        }
    }
//    else{
//        assert(false);
//    }

    return ins;
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
            handle_illegal_imm(dest_reg_abs,x,0);

        }
        return ins;
    }
    if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
        int x=value1->pdata->var_pdata.iVal;
        handle_illegal_imm1(dest_reg_abs,x);

        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

        }
        return ins;

    }
    if(isImmFloatType(value1->VTy)){
        float y=value1->pdata->var_pdata.fVal;
        int x=*(int*)&y;
        handle_illegal_imm1(dest_reg_abs,x);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",x);
//        printf("\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
//        fprintf(fp,"\tldr\tr%d,=%s\n",dest_reg_abs,arr1);
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

        }
        return ins;
    }
    if(isLocalVarIntType(value1->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
            if(optimization==1&&opt_copy==1){ //开启优化
                if(dest_reg<0){ //需要回存到内存
                    x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(left_reg-100,x,0);
                }
            }else if(dest_reg_abs!=(left_reg-100)){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            }
        } else{
            if(optimization==1&&opt_copy==1){ //开启优化
                if(dest_reg<0){ //需要回存到内存
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(left_reg,x,0);
                }
            }else if(dest_reg_abs!=left_reg){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            }
        }
        if(dest_reg<0 && !(optimization==1&&opt_copy==1)){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);
        }
        return ins;
    }
    if(isLocalVarFloatType(value1->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
            if(optimization==1&&opt_copy==1){ //开启优化
                if(dest_reg<0){ //需要回存到内存
                    x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(left_reg-100,x,0);
                }
            }else if(dest_reg_abs!=(left_reg-100)){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            }
        } else{
            if(optimization==1&&opt_copy==1){ //开启优化
                if(dest_reg<0){ //需要回存到内存
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(left_reg,x,0);
                }
            }else if(dest_reg_abs!=left_reg){
                printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            }
        }
        if(dest_reg<0 && !(optimization==1&&opt_copy==1)){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm(left_reg,x,1);
        if(optimization==1&&opt_copy==1){ //开启优化
            if(dest_reg<0){ //需要回存到内存
                x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(left_reg-100,x,0);
            }
        }else if(dest_reg_abs!=(left_reg-100)){
            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
        }
    } else{
        if(optimization==1&&opt_copy==1){ //开启优化
            if(dest_reg<0){ //需要回存到内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(left_reg,x,0);
            }
        }else if(dest_reg_abs!=left_reg){
            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
        }
    }
    if(dest_reg<0 &&!(optimization==1&&opt_copy==1)){
        int x= get_value_offset_sp(hashMap,value0);
        handle_illegal_imm(dest_reg_abs,x,0);

    }
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

//    int src_leftreg=ins->inst->_reg_[1];
//    int src_rightreg=ins->inst->_reg_[2];
//    int left_reg=abs(src_leftreg);
//    int right_reg=abs(src_rightreg);

//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        int ans=x1+x2;
        if(imm_is_valid(ans)){
            printf("\tmov\tr%d,#%d\n",dest_reg_abs,ans);
            fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,ans);
        }else{
            handle_illegal_imm1(dest_reg_abs,ans);
        }


        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                   handle_illegal_imm(dest_reg_abs,x,0);

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

        float fx1=(float)x1;
        float ans=fx1+x2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }



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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        float fx2=(float)x2;
        float ans=x1+fx2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }


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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        float ans=x1+x2;
        int ix=*(int*)&ans;
        if(imm_is_valid(ix)){
            printf("\tmov\tr0,#%d\n",ix);
            fprintf(fp,"\tmov\tr0,#%d\n",ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }



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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg-100,x1);
            }else{
                ;
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,right_reg,x1);
            }

        }else{
            handle_illegal_imm1(1,x1);

            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);
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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(left_reg,x,1);

                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
            }else{
                printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
                fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
            }
        }else{
            handle_illegal_imm1(2,x2);

            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);


            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tadd\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

//    if(src_leftreg<0){
//        int x= get_value_offset_sp(hashMap,value1);
//        handle_illegal_imm(left_reg,x,0);
//    }
//    if(src_rightreg<0){
//        int x= get_value_offset_sp(hashMap,value2);
//        handle_illegal_imm(right_reg,x,0);
//    }
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

//    int src_leftreg=ins->inst->_reg_[1];
//    int src_rightreg=ins->inst->_reg_[2];
//    int left_reg=abs(src_leftreg);
//    int right_reg=abs(src_rightreg);
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        int ans=x1-x2;
        if(imm_is_valid(ans)){
            printf("\tmov\tr%d,#%d\n",dest_reg_abs,ans);
            fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,ans);
        }else{
            handle_illegal_imm1(dest_reg_abs,ans);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
    if(isImmIntType(value1->VTy) && isImmFloatType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        float x2=value2->pdata->var_pdata.fVal;

        float fx1=(float)x1;
        float ans=fx1-x2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

        float fx2=(float)x2;
        float ans=x1-fx2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }


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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        float ans=x1-x2;
        int ix=*(int*)&ans;
        if(imm_is_valid(ix)){
            printf("\tmov\tr0,#%d\n",ix);
            fprintf(fp,"\tmov\tr0,#%d\n",ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tmov\tr1,#%d\n",x1);
                fprintf(fp,"\tmov\tr1,#%d\n",x1);
                printf("\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                fprintf(fp,"\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
            }else{
                ;
                printf("\tmov\tr1,#%d\n",x1);
                fprintf(fp,"\tmov\tr1,#%d\n",x1);
                printf("\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                fprintf(fp,"\tsub\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
            }

        }else{
            handle_illegal_imm1(1,x1);

            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(left_reg,x,1);

                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x2);
            }else{
                printf("\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
                fprintf(fp,"\tsub\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x2);
            }
        }else{
            handle_illegal_imm1(2,x2);

            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tsub\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//    if(src_leftreg<0){
//        int x= get_value_offset_sp(hashMap,value1);
//        handle_illegal_imm(left_reg,x,0);
//    }
//    if(src_rightreg<0){
//        int x= get_value_offset_sp(hashMap,value2);
//        handle_illegal_imm(right_reg,x,0);
//    }
    return  ins;

}
int count_bit(int value){
    assert(value>0);
    int count=0;
    while (value){
        value&=(value-1);
        count++;
    }
    return count;
}
// 默认left_reg为已经加载的，并且left_reg时对应的寄存器编号
// 其实在这里面解决这个是否存在于内存中也是可以的
int optimization_mul(int dest_reg,int left_reg,int imm){
    if(optimization==0 || opt_mul==0){ //乘法优化开关被关闭
        return 0;
    }
    int imm_abs=abs(imm);
    int flag=0; // imm的正负
    int n1=0;
    int n2=0;
    if((imm > 2147483648) || (imm<-2147483648)){
        return 0;
    }
    if(imm<0) {
        flag = 1;
    }

    if(imm==0){
        printf("\tmov\tr%d,#0\n",dest_reg);
        fprintf(fp,"\tmov\tr%d,#0\n",dest_reg);
        return 1;
    }
    else if(imm==1){
        printf("\tmov\tr%d,r%d\n",dest_reg,left_reg);
        fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg,left_reg);
        return 1;
    }
    else if(imm==-1){
        printf("\trsb\tr%d,r%d,#0\n",dest_reg,left_reg);
        fprintf(fp,"\trsb\tr%d,r%d,#0\n",dest_reg,left_reg);
        return 1;
    }

    if(imm_abs%2==0){ //偶数
        if((imm_abs&(imm_abs-1))==0){ //2的幂次
            while (imm_abs!=1){
                imm_abs=imm_abs>>1;
                n1++;
            }
            if(flag==0){
                printf("\tlsl\tr%d,r%d,#%d\n",dest_reg, left_reg,n1);
                fprintf(fp,"\tlsl\tr%d,r%d,#%d\n",dest_reg, left_reg,n1);
            }else{
                printf("\tmov\tr0,#0\n");
                fprintf(fp,"\tmov\tr0,#0\n");
                printf("\tsub\tr%d,r0,r%d,lsl #%d\n",dest_reg,left_reg,n1);
                fprintf(fp,"\tsub\tr%d,r0,r%d,lsl #%d\n",dest_reg,left_reg,n1);
            }
            return 1;
        }else{  //非2的幂次
            while (imm_abs%2==0){
                imm_abs=imm_abs>>1;
                n1++;
            }
//           此时剩余的imm_abs是奇数，因为最后一位是1
            if(((imm_abs+1)&imm_abs)==0){ //剩余数加一是2的幂次
                int tmp=imm_abs+1;
                while (tmp!=1){
                    tmp=tmp>>1;
                    n2++;
                }
                if(flag==0){
                    printf("\trsb\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                    fprintf(fp,"\trsb\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                }else{
                    printf("\tsub\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                    fprintf(fp,"\tsub\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                }
                printf("\tlsl\tr%d,r%d,#%d\n",dest_reg,dest_reg,n1);
                fprintf(fp,"\tlsl\tr%d,r%d,#%d\n",dest_reg,dest_reg,n1);
                return 1;
            }else if(((imm_abs-1)&(imm_abs-2))==0){ //剩余数-1是2的幂次
                int tmp=imm_abs-1;
                while (tmp!=1){
                    tmp=tmp>>1;
                    n2++;
                }
                printf("\tadd\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                fprintf(fp,"\tadd\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                if(flag==1){
                    printf("\trsb\tr%d,r%d,#0\n",dest_reg,dest_reg);
                    fprintf(fp,"\trsb\tr%d,r%d,#0\n",dest_reg,dest_reg);
                }
                printf("\tlsl\tr%d,r%d,#%d\n",dest_reg,dest_reg,n1);
                fprintf(fp,"\tlsl\tr%d,r%d,#%d\n",dest_reg,dest_reg,n1);
                return 1;
            }
            return 0;
        }
    }else{
        if(((imm_abs+1)&imm_abs)==0){ //+1是2的幂次
            int tmp=imm_abs+1;
            while (tmp!=1){
                tmp=tmp>>1;
                n2++;
            }
            if(flag==0){
                printf("\trsb\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                fprintf(fp,"\trsb\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
            }else{
                printf("\tsub\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
                fprintf(fp,"\tsub\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
            }
            return 1;
        }
        else if(((imm_abs-2)&(imm_abs-1))==0){  //-1是2的幂次
            int tmp=imm_abs-1;
            while (tmp!=1){
                tmp=tmp>>1;
                n2++;
            }
            printf("\tadd\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
            fprintf(fp,"\tadd\tr%d,r%d,r%d,lsl #%d\n",dest_reg,left_reg,left_reg,n2);
            if(flag==1){
                printf("\trsb\tr%d,r%d,#0\n",dest_reg,dest_reg);
                fprintf(fp,"\trsb\tr%d,r%d,#0\n",dest_reg,dest_reg);
            }
            return 1;
        }
        return 0;
    }
    return 0;
//    int imm_abs= abs(imm);
//    int n= power_of_two(imm_abs);
//    if(n!=-1){ //    2^n 和 -2^n 优化
//        if(imm>0){
//            printf("\tlsl\tr%d,r%d,#%d\n",dest_reg,left_reg,n);
//            fprintf(fp,"\tlsl\tr%d,r%d,#%d\n",dest_reg,left_reg,n);
//        }else if(imm<0){
//            printf("\tmov\tr1,#0\n");
//            fprintf(fp,"\tmov\tr1,#0\n");
//            printf("\tsub\tr%d,r1,r%d,lsl #%d\n",dest_reg,left_reg,n);
//            fprintf(fp,"\tsub\tr%d,r1,r%d,lsl #%d\n",dest_reg,left_reg,n);
//        }
//        return 1;
//    }
//    int bit1_num= count_bit(imm_abs);
//    if(bit1_num==2){ // 优化乘数或者是被乘数（先取绝对值转换为正数）的二进制中有两个一的情况，改用移位和加法指令来实现。
////        这里还需要去确定到底是那两位是1，确定下来之后，将被乘数左移x1位存放到r0，r0再加上被乘数左移x2位
////        如果imm是负数的话，rsb 0将结果取反就可以了，如果是正数，r0里面直接就是存放的结果了
//        return 1;
//    }
////    还可以优化的就是照着谌强的区分奇偶和加减1成为2的幂次。

}
InstNode * arm_trans_Mul(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];

//    int src_leftreg=ins->inst->_reg_[1];
//    int src_rightreg=ins->inst->_reg_[2];
//    int left_reg=abs(src_leftreg);
//    int right_reg=abs(src_rightreg);
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){
//     两个都是立即数的情况会被优化掉
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        int ans=x1*x2;
        if(imm_is_valid(ans)){
            printf("\tmov\tr%d,#%d\n",dest_reg_abs,ans);
            fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,ans);
        }else{
            handle_illegal_imm1(dest_reg_abs,ans);
        }


        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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

        float fx1=(float)x1;
        float ans=fx1*x2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

        float fx2=(float)x2;
        float ans=x1*fx2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

        float ans=x1*x2;
        int ix=*(int*)&ans;
        if(imm_is_valid(ix)){
            printf("\tmov\tr0,#%d\n",ix);
            fprintf(fp,"\tmov\tr0,#%d\n",ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }


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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//优化
    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
//        这里需要进行优化
        int x1=value1->pdata->var_pdata.iVal;
//        if(power_of_two(x1)==-1){ //非2的幂次，优化不了
        int right_reg_end;
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);
            right_reg_end=right_reg-100;
        }else{
            right_reg_end=right_reg;
        }
        if(optimization_mul(dest_reg_abs,right_reg_end,x1)==0){ //返回值为1表示已经优化。返回值为零表示无法优化，使用mul指令
            if(imm_is_valid(x1)){
                printf("\tmov\tr1,#%d\n",x1);
                fprintf(fp,"\tmov\tr1,#%d\n",x1);
                if(right_reg>100){
                    int x= get_value_offset_sp(hashMap,value2);
                    handle_illegal_imm(right_reg,x,2);

                    printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                    fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                }else{
                    ;
                    printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                    fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                }

            }else{
                handle_illegal_imm1(1,x1);

                if(right_reg>100){
                    int x= get_value_offset_sp(hashMap,value2);
                    handle_illegal_imm(right_reg,x,2);

                    printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                    fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg-100);
                }else{
                    printf("\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                    fprintf(fp,"\tmul\tr%d,r1,r%d\n",dest_reg_abs,right_reg);
                }

            }
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//优化
    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        int left_reg_abs=0;
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
            left_reg_abs=left_reg-100;
        }else{
            left_reg_abs=left_reg;
        }
        if(optimization_mul(dest_reg_abs,left_reg_abs,x2)==0){ //返回1代表优化成功，0优化失败使用mul指令
            if((imm_is_valid(x2))){
                printf("\tmov\tr2,#%d\n",x2);
                fprintf(fp,"\tmov\tr2,#%d\n",x2);
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                    fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                }else{
                    printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                    fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                }
            }else{
                handle_illegal_imm1(2,x2);

                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                    fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg-100);
                }else{
                    printf("\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                    fprintf(fp,"\tmul\tr%d,r%d,r2\n",dest_reg_abs,left_reg);
                }
            }
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg-100);

        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
            fprintf(fp,"\tmul\tr%d,r%d,r%d\n",dest_reg_abs,left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
//            printf("\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
//            fprintf(fp,"\tldr\tr%d,[r11,#%d]\n",right_reg-100,x2);
            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

//    if(src_leftreg<0){
//        int x= get_value_offset_sp(hashMap,value1);
//        handle_illegal_imm(left_reg,x,0);
//    }
//    if(src_rightreg<0){
//        int x= get_value_offset_sp(hashMap,value2);
//        handle_illegal_imm(right_reg,x,0);
//    }
    return  ins;
}

InstNode * arm_trans_Div(InstNode *ins,HashMap*hashMap){
//    if(reg_save[12]==1){
//        printf("\tstr\tr12,[sp,#-4]!\n");
//        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
//    }
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];

//    int src_leftreg=ins->inst->_reg_[1];
//    int src_rightreg=ins->inst->_reg_[2];
//    int left_reg=abs(src_leftreg);
//    int right_reg=abs(src_rightreg);
//    立即数操作数是不用考虑寄存器，但是其结果需要考虑寄存器的。
//    这里的设计就是直接将运算结果存放在目的寄存器了。
    if(isImmIntType(value1->VTy)&&isImmIntType(value2->VTy)){

        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        int ans=x1/x2;
        if(imm_is_valid(ans)){
            printf("\tmov\tr%d,#%d\n",dest_reg_abs,ans);
            fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,ans);
        }else{
            handle_illegal_imm1(dest_reg_abs,ans);
        }

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg>0){
//               说明不用存回内存，所以这里不需要处理
                ;
            } else{
//                存回内存
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
        float fx1=(float)x1;
        float ans=fx1/x2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }
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
                handle_illegal_imm(dest_reg_abs,x,0);
//                printf("\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
//                fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",dest_reg_abs,x);
            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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

        float fx2=(float)x2;
        float ans=x1/fx2;
        int ix1=*(int*)&ans;
        if(imm_is_valid(ix1)){
            printf("\tmov\tr0,#%d\n",ix1);
            fprintf(fp,"\tmov\tr0,#%d\n",ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix1);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        float ans=x1/x2;
        int ix=*(int*)&ans;
        if(imm_is_valid(ix)){
            printf("\tmov\tr0,#%d\n",ix);
            fprintf(fp,"\tmov\tr0,#%d\n",ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }else{
            handle_illegal_imm1(0,ix);
            printf("\tvmov\ts0,r0\n");
            fprintf(fp,"\tvmov\ts0,r0\n");
        }

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//        这个是4/k这种情况，这个好像不好优化吧
        if(imm_is_valid(x1)){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
                if(reg_save[12]==1){
                    printf("\tstr\tr12,[sp,#-4]!\n");
                    fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                }
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
                if(reg_save[12]==1){
                    printf("\tldr\tr12,[sp],#4\n");
                    fprintf(fp,"\tldr\tr12,[sp],#4\n");
                }
            }else{
                ;
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
                if(reg_save[12]==1){
                    printf("\tstr\tr12,[sp,#-4]!\n");
                    fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                }
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
                if(reg_save[12]==1){
                    printf("\tldr\tr12,[sp],#4\n");
                    fprintf(fp,"\tldr\tr12,[sp],#4\n");
                }
            }

        }else{
            handle_illegal_imm1(0,x1);

            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
                if(reg_save[12]==1){
                    printf("\tstr\tr12,[sp,#-4]!\n");
                    fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                }
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
                if(reg_save[12]==1){
                    printf("\tldr\tr12,[sp],#4\n");
                    fprintf(fp,"\tldr\tr12,[sp],#4\n");
                }
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
                if(reg_save[12]==1){
                    printf("\tstr\tr12,[sp,#-4]!\n");
                    fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                }
                printf("\tbl\t__aeabi_idiv\n");
                fprintf(fp,"\tbl\t__aeabi_idiv\n");
                if(reg_save[12]==1){
                    printf("\tldr\tr12,[sp],#4\n");
                    fprintf(fp,"\tldr\tr12,[sp],#4\n");
                }
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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//        assert(x2!=0);

        int tmp= abs(x2);
        int n= power_of_two(tmp);
        if(n!=-1 && optimization==1 && opt_div2==1 ){ //优化
            if(x2<0){
                if(n==1){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tadd\tr0,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        fprintf(fp,"\tadd\tr0,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        printf("\tmov\tr1,#0\n");
                        fprintf(fp,"\tmov\tr1,#0\n");
                        printf("\tsub\tr%d,r1,r0,asr #1\n",dest_reg_abs);
                        fprintf(fp,"\tsub\tr%d,r1,r0,asr #1\n",dest_reg_abs);
                    }else{
                        printf("\tadd\tr0,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        fprintf(fp,"\tadd\tr0,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        printf("\tmov\tr1,#0\n");
                        fprintf(fp,"\tmov\tr1,#0\n");
                        printf("\tsub\tr%d,r1,r0,asr #1\n",dest_reg_abs);
                        fprintf(fp,"\tsub\tr%d,r1,r0,asr #1\n",dest_reg_abs);
                    }
                } else{
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tasr\tr1,r%d,#31\n",left_reg-100);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg-100);
                        printf("\tadd\tr0,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        fprintf(fp,"\tadd\tr0,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        printf("\tmov\tr1,#0\n");
                        fprintf(fp,"\tmov\tr1,#0\n");
                        printf("\tsub\tr%d,r1,r0,asr #%d\n",dest_reg_abs,n);
                        fprintf(fp,"\tsub\tr%d,r1,r0,asr #%d\n",dest_reg_abs,n);
                    }else{
                        printf("\tasr\tr1,r%d,#31\n",left_reg);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg);
                        printf("\tadd\tr0,r%d,r1,lsr #%d\n",left_reg,32-n);
                        fprintf(fp,"\tadd\tr0,r%d,r1,lsr #%d\n",left_reg,32-n);
                        printf("\tmov\tr1,#0\n");
                        fprintf(fp,"\tmov\tr1,#0\n");
                        printf("\tsub\tr%d,r1,r0,asr #%d\n",dest_reg_abs,n);
                        fprintf(fp,"\tsub\tr%d,r1,r0,asr #%d\n",dest_reg_abs,n);
                    }
                }

            }else if(x2>0){
                if(n==1){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tadd\tr0,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        fprintf(fp,"\tadd\tr0,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        printf("\tasr\tr%d,r0,#1\n",dest_reg_abs);
                        fprintf(fp,"\tasr\tr%d,r0,#1\n",dest_reg_abs);
                    }else{
                        printf("\tadd\tr0,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        fprintf(fp,"\tadd\tr0,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        printf("\tasr\tr%d,r0,#1\n",dest_reg_abs);
                        fprintf(fp,"\tasr\tr%d,r0,#1\n",dest_reg_abs);
                    }
                } else{
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tasr\tr1,r%d,#31\n",left_reg-100);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg-100);
                        printf("\tadd\tr0,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        fprintf(fp,"\tadd\tr0,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        printf("\tasr\tr%d,r0,#%d\n",dest_reg_abs,n);
                        fprintf(fp,"\tasr\tr%d,r0,#%d\n",dest_reg_abs,n);
                    }else{
                        printf("\tasr\tr1,r%d,#31\n",left_reg);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg);
                        printf("\tadd\tr0,r%d,r1,lsr #%d\n",left_reg,32-n);
                        fprintf(fp,"\tadd\tr0,r%d,r1,lsr #%d\n",left_reg,32-n);
                        printf("\tasr\tr%d,r0,#%d\n",dest_reg_abs,n);
                        fprintf(fp,"\tasr\tr%d,r0,#%d\n",dest_reg_abs,n);
                    }
                }
            }
        }else{
            if((imm_is_valid(x2))){
                printf("\tmov\tr1,#%d\n",x2);
                fprintf(fp,"\tmov\tr1,#%d\n",x2);
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                    if(reg_save[12]==1){
                        printf("\tstr\tr12,[sp,#-4]!\n");
                        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                    }
                    printf("\tbl\t__aeabi_idiv\n");
                    fprintf(fp,"\tbl\t__aeabi_idiv\n");
                    if(reg_save[12]==1){
                        printf("\tldr\tr12,[sp],#4\n");
                        fprintf(fp,"\tldr\tr12,[sp],#4\n");
                    }

                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                    if(reg_save[12]==1){
                        printf("\tstr\tr12,[sp,#-4]!\n");
                        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                    }
                    printf("\tbl\t__aeabi_idiv\n");
                    fprintf(fp,"\tbl\t__aeabi_idiv\n");
                    if(reg_save[12]==1){
                        printf("\tldr\tr12,[sp],#4\n");
                        fprintf(fp,"\tldr\tr12,[sp],#4\n");
                    }
                }
            }else{
                handle_illegal_imm1(0,x2);

                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                    if(reg_save[12]==1){
                        printf("\tstr\tr12,[sp,#-4]!\n");
                        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                    }
                    printf("\tbl\t__aeabi_idiv\n");
                    fprintf(fp,"\tbl\t__aeabi_idiv\n");
                    if(reg_save[12]==1){
                        printf("\tldr\tr12,[sp],#4\n");
                        fprintf(fp,"\tldr\tr12,[sp],#4\n");
                    }
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                    if(reg_save[12]==1){
                        printf("\tstr\tr12,[sp,#-4]!\n");
                        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
                    }
                    printf("\tbl\t__aeabi_idiv\n");
                    fprintf(fp,"\tbl\t__aeabi_idiv\n");
                    if(reg_save[12]==1){
                        printf("\tldr\tr12,[sp],#4\n");
                        fprintf(fp,"\tldr\tr12,[sp],#4\n");
                    }
                }
            }
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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);
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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
        if(reg_save[12]==1){
            printf("\tstr\tr12,[sp,#-4]!\n");
            fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
        }
        printf("\tbl\t__aeabi_idiv\n");
        fprintf(fp,"\tbl\t__aeabi_idiv\n");
        if(reg_save[12]==1){
            printf("\tldr\tr12,[sp],#4\n");
            fprintf(fp,"\tldr\tr12,[sp],#4\n");
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
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else if(isLocalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
            if(dest_reg>0){
                ;
            } else{
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
//    if(reg_save[12]==1){
//        printf("\tldr\tr12,[sp],#4\n");
//        fprintf(fp,"\tldr\tr12,[sp],#4\n");
//    }
//    if(src_leftreg<0){
//        int x= get_value_offset_sp(hashMap,value1);
//        handle_illegal_imm(left_reg,x,0);
//    }
//    if(src_rightreg<0){
//        int x= get_value_offset_sp(hashMap,value2);
//        handle_illegal_imm(right_reg,x,0);
//    }
    return  ins;
}


//现在的话，那个加减乘除法的基本已经接上和改完了

InstNode * arm_trans_Module(InstNode *ins,HashMap*hashMap){
//    这个比较简单，只会存在两种情况，就是int=int1 % int2和float=int1 % int2,右边出现非int都是错误的
// 1 ****************************************
// 将int1放在r0，将int2放在r1，然后执行bl	__aeabi_idivmod(PLT)
//    if(reg_save[12]==1){
//        printf("\tstr\tr12,[sp,#-4]!\n");
//        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
//    }
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    int right_reg=ins->inst->_reg_[2];

//    int src_leftreg=ins->inst->_reg_[1];
//    int src_rightreg=ins->inst->_reg_[2];
//    int left_reg=abs(src_leftreg);
//    int right_reg=abs(src_rightreg);

    if(isImmIntType(value1->VTy)&& isImmIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        int x2=value2->pdata->var_pdata.iVal;
        int ans=x1%x2;
        if(imm_is_valid(ans)){
            printf("\tmov\tr%d,#%d\n",dest_reg_abs,ans);
            fprintf(fp,"\tmov\tr%d,#%d\n",dest_reg_abs,ans);
        }else{
            handle_illegal_imm1(dest_reg_abs,ans);
        }
//        if(imm_is_valid(x1)&&(imm_is_valid(x2))){
//            printf("\tmov\tr0,#%d\n",x1);
//            fprintf(fp,"\tmov\tr0,#%d\n",x1);
//            printf("\tmov\tr1,#%d\n",x2);
//            fprintf(fp,"\tmov\tr1,#%d\n",x2);
//        }else if ((!imm_is_valid(x1))&&(imm_is_valid(x2))){
//            handle_illegal_imm1(0,x1);
//
//            printf("\tmov\tr1,#%d\n",x2);
//            fprintf(fp,"\tmov\tr1,#%d\n",x2);
//        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
//            printf("\tmov\tr0,#%d\n",x1);
//            fprintf(fp,"\tmov\tr0,#%d\n",x1);
//            handle_illegal_imm1(1,x2);
//
//        }else{
//            handle_illegal_imm1(0,x1);
//            handle_illegal_imm1(1,x2);
////            char arr1[12]="0x";
////            sprintf(arr1+2,"%0x",x1);
////            char arr2[12]="0x";
////            sprintf(arr2+2,"%0x",x2);
////            printf("\tldr\tr0,=%s\n",arr1);
////            fprintf(fp,"\tldr\tr0,=%s\n",arr1);
////            printf("\tldr\tr1,=%s\n",arr2);
////            fprintf(fp,"\tldr\tr1,=%s\n",arr2);
//        }
//        if(reg_save[12]==1){
//            printf("\tstr\tr12,[sp,#-4]!\n");
//            fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
//        }
//        printf("\tbl\t__aeabi_idivmod\n");
//        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
//        if(reg_save[12]==1){
//            printf("\tldr\tr12,[sp],#4\n");
//            fprintf(fp,"\tldr\tr12,[sp],#4\n");
//        }
//        printf("\tmov\tr%d,r1\n",dest_reg_abs);
//        fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);

        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }else if(isLocalArrayIntType(value0->VTy)){
            assert(false);
        }else if(isGlobalVarIntType(value0->VTy)){
//            ;这些都是需要补充完整的，不对，
//            全局变量会有相应的load和store指令，结果不应该在这里处理
//            这里只需要转换为相应的格式就可以了
            assert(false);
        }else if(isGlobalVarFloatType(value0->VTy)){
//                需要将相加的结果转化为IEEE754格式存放在r0中
            assert(false);
            printf("\tvmov\ts0,r%d\n",dest_reg_abs);
            fprintf(fp,"\tvmov\ts0,r%d\n",dest_reg_abs);
            printf("\tvcvt.f32.s32\ts0,s0\n");
            fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            printf("\tvmov\tr%d,s0\n",dest_reg_abs);
            fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
        }else if(isGlobalArrayIntType(value0->VTy)){
            assert(false);
        }
    }


    if(isImmIntType(value1->VTy) && isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr0,#%d\n",x1);
            fprintf(fp,"\tmov\tr0,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(0,x1);

            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tmov\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tmov\tr1,r%d\n",right_reg);
                fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
            }
        }
        if(reg_save[12]==1){
            printf("\tstr\tr12,[sp,#-4]!\n");
            fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        if(reg_save[12]==1){
            printf("\tldr\tr12,[sp],#4\n");
            fprintf(fp,"\tldr\tr12,[sp],#4\n");
        }
        printf("\tmov\tr%d,r1\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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

//    这里可以优化
    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
        int x2=value2->pdata->var_pdata.iVal;
        int tmp= abs(x2);
        int n= power_of_two(tmp);
        if(n!=-1 && optimization==1 && opt_mod2==1){ //优化
            if(x2<0){
                if(n==1){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tadd\tr1,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        fprintf(fp,"\tadd\tr1,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        printf("\tbic\tr1,r1,#1\n");
                        fprintf(fp,"\tbic\tr1,r1,#1\n");
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                    } else{
                        printf("\tadd\tr1,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        fprintf(fp,"\tadd\tr1,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        printf("\tbic\tr1,r1,#1\n");
                        fprintf(fp,"\tbic\tr1,r1,#1\n");
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                    }
                } else{
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);

                        printf("\tasr\tr1,r%d,#31\n",left_reg-100);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg-100);
                        printf("\tadd\tr1,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        fprintf(fp,"\tadd\tr1,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        if(imm_is_valid(tmp)){
                            printf("\tbic\tr1,r1,#%d\n",tmp-1);
                            fprintf(fp,"\tbic\tr1,r1,#%d\n",tmp-1);
                        }else{
                            handle_illegal_imm1(3,tmp-1);
                            printf("\tbic\tr1,r1,r3\n");
                            fprintf(fp,"\tbic\tr1,r1,r3\n");
                        }
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                    } else{
                        printf("\tasr\tr1,r%d,#31\n",left_reg);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg);
                        printf("\tadd\tr1,r%d,r1,lsr #%d\n",left_reg,32-n);
                        fprintf(fp,"\tadd\tr1,r%d,r1,lsr #%d\n",left_reg,32-n);
                        if(imm_is_valid(tmp)){
                            printf("\tbic\tr1,r1,#%d\n",tmp-1);
                            fprintf(fp,"\tbic\tr1,r1,#%d\n",tmp-1);
                        }else{
                            handle_illegal_imm1(3,tmp-1);
                            printf("\tbic\tr1,r1,r3\n");
                            fprintf(fp,"\tbic\tr1,r1,r3\n");
                        }
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                    }
                }
            }else if(x2>0){
                if(n==1){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tadd\tr1,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        fprintf(fp,"\tadd\tr1,r%d,r%d,lsr #31\n",left_reg-100,left_reg-100);
                        printf("\tbic\tr1,r1,#1\n");
                        fprintf(fp,"\tbic\tr1,r1,#1\n");
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                    } else{
                        printf("\tadd\tr1,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        fprintf(fp,"\tadd\tr1,r%d,r%d,lsr #31\n",left_reg,left_reg);
                        printf("\tbic\tr1,r1,#1\n");
                        fprintf(fp,"\tbic\tr1,r1,#1\n");
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                    }
                } else{
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);

                        printf("\tasr\tr1,r%d,#31\n",left_reg-100);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg-100);
                        printf("\tadd\tr1,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        fprintf(fp,"\tadd\tr1,r%d,r1,lsr #%d\n",left_reg-100,32-n);
                        if(imm_is_valid(tmp)){
                            printf("\tbic\tr1,r1,#%d\n",tmp-1);
                            fprintf(fp,"\tbic\tr1,r1,#%d\n",tmp-1);
                        }else{
                            handle_illegal_imm1(3,tmp-1);
                            printf("\tbic\tr1,r1,r3\n");
                            fprintf(fp,"\tbic\tr1,r1,r3\n");
                        }
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg-100);
                    } else{
                        printf("\tasr\tr1,r%d,#31\n",left_reg);
                        fprintf(fp,"\tasr\tr1,r%d,#31\n",left_reg);
                        printf("\tadd\tr1,r%d,r1,lsr #%d\n",left_reg,32-n);
                        fprintf(fp,"\tadd\tr1,r%d,r1,lsr #%d\n",left_reg,32-n);
                        if(imm_is_valid(tmp)){
                            printf("\tbic\tr1,r1,#%d\n",tmp-1);
                            fprintf(fp,"\tbic\tr1,r1,#%d\n",tmp-1);
                        }else{
                            handle_illegal_imm1(3,tmp-1);
                            printf("\tbic\tr1,r1,r3\n");
                            fprintf(fp,"\tbic\tr1,r1,r3\n");
                        }
                        printf("\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                        fprintf(fp,"\tsub\tr%d,r%d,r1\n",dest_reg_abs,left_reg);
                    }
                }
            }
        }else{
            if((imm_is_valid(x2))){
                printf("\tmov\tr1,#%d\n",x2);
                fprintf(fp,"\tmov\tr1,#%d\n",x2);
                if(left_reg > 100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                } else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
            }else{
                handle_illegal_imm1(1,x2);

                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                } else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
            }
            if(reg_save[12]==1){
                printf("\tstr\tr12,[sp,#-4]!\n");
                fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
            }
            printf("\tbl\t__aeabi_idivmod\n");
            fprintf(fp,"\tbl\t__aeabi_idivmod\n");
            if(reg_save[12]==1){
                printf("\tldr\tr12,[sp],#4\n");
                fprintf(fp,"\tldr\tr12,[sp],#4\n");
            }
            printf("\tmov\tr%d,r1\n",dest_reg_abs);
            fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);

        }






        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tmov\tr0,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            printf("\tmov\tr1,r%d\n",right_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
        if(reg_save[12]==1){
            printf("\tstr\tr12,[sp,#-4]!\n");
            fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
        }
        printf("\tbl\t__aeabi_idivmod\n");
        fprintf(fp,"\tbl\t__aeabi_idivmod\n");
        if(reg_save[12]==1){
            printf("\tldr\tr12,[sp],#4\n");
            fprintf(fp,"\tldr\tr12,[sp],#4\n");
        }
        printf("\tmov\tr%d,r1\n",dest_reg_abs);
        fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);
        if(isLocalVarIntType(value0->VTy)){
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

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
                handle_illegal_imm(dest_reg_abs,x,0);

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
//    if(reg_save[12]==1){
//        printf("\tldr\tr12,[sp],#4\n");
//        fprintf(fp,"\tldr\tr12,[sp],#4\n");
//    }
//    printf("arm_trans_Module\n");
//    if(src_leftreg<0){
//        int x= get_value_offset_sp(hashMap,value1);
//        handle_illegal_imm(left_reg,x,0);
//    }
//    if(src_rightreg<0){
//        int x= get_value_offset_sp(hashMap,value2);
//        handle_illegal_imm(right_reg,x,0);
//    }
    return ins;
}

InstNode * arm_trans_Call(InstNode *ins,HashMap*hashMap){
    int flag=0,reflag=0;
//    还需要进行返回值类型的转化

//    现在的话，参数传递是在call指令bl之前来处理的，
//    所以说现在是先调用一下get_param_list(),传入call的value1，和give_count这个变量的地址，然后就可以使用0ne_param里面的ir了
    memset(give_param_flag,0, sizeof(give_param_flag));

    int operandNum=ins->inst->user.value.NumUserOperands;

//    现在这个call简单修复了一下，就是返回值为Unkonwn的话，
//    将相当于使void没有返回值，这个时候是不需要进行将r0移到左值里
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    if(reg_save[12]==1 && isSySYFunction(value1)){
//        printf("\tstr\tr12,[sp,#-4]!\n");
//        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
//    }


    func_param_type=value1;
    int param_num_=value1->pdata->symtab_func_pdata.param_num;
//    if((param_num_%2)!=0){
//        printf("\tstr\tr0,[sp,#-4]!\n");
//        fprintf(fp,"\tstr\tr0,[sp,#-4]!\n");
//    }
    get_param_list(value1,&give_count);
//    for(int i=0;i<param_num_;i++){
//        InstNode *temp=one_param[i];
//        printf("\n");
//    }
    if(reg_save[12]==1 && isSySYFunction(value1)){
        flag=1;
    }
    if(param_num_<=4){
        if(flag==1){
            printf("\tsub\tsp,sp,#4\n");
            fprintf(fp,"\tsub\tsp,sp,#4\n");
            reflag=1;
        }
    }else{
        if(((flag+param_num_-4)%2)!=0){
            printf("\tsub\tsp,sp,#4\n");
            fprintf(fp,"\tsub\tsp,sp,#4\n");
            reflag=1;
        }
    }
    arm_trans_GIVE_PARAM(hashMap,param_num_);
//    if(strcmp(user_get_operand_use(&ins->inst->user,0)->Val->name,"putfloat")==0){
//        printf("\tvmov\ts0,r0\n");
//        fprintf(fp,"\tvmov\ts0,r0\n");
//    }
//    printf("CALL\n");
    if(reg_save[12]==1 && isSySYFunction(value1)){
        printf("\tstr\tr12,[sp,#-4]!\n");
        fprintf(fp,"\tstr\tr12,[sp,#-4]!\n");
    }
    printf("\tbl\t%s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    fprintf(fp,"\tbl\t%s\n", user_get_operand_use(&ins->inst->user,0)->Val->name);

    if(reg_save[12]==1 && isSySYFunction(value1)){
        printf("\tldr\tr12,[sp],#4\n");
        fprintf(fp,"\tldr\tr12,[sp],#4\n");
    }
    memset(give_param_flag,0, sizeof(give_param_flag));

//    if(strcmp(user_get_operand_use(&ins->inst->user,0)->Val->name,"getfloat")==0){
//        printf("\tvmov\tr0,s0\n");
//        fprintf(fp,"\tvmov\tr0,s0\n");
//    }
//    这里还需要调整sp,去掉压入栈的参数，这里可以使用add直接调整sp，也可以使用mov sp,fp直接调整
    if(param_num_>4 || reflag==1 ){
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
////        call函数的返回值为void，不需要处理,这个operandNum是没有标识value0的，所以说没用
//    }

    //        call函数的返回值不为void,还要将r0转移到左值
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs= abs(dest_reg);

//    if(dest_reg_abs!=0){
//        printf("\tmov\tr%d,r0\n",dest_reg_abs);
//        fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
//    }
    // 这个也是默认放回值类型和左值类型是一致的
    // 如果用一个float型去接受int型的结果，后面应该会有Copy指令的

//    返回值为int
    if(!returnValueNotUsed(ins)){ //返回值被使用，需要保存
        if(func_param_type->pdata->symtab_func_pdata.return_type.ID==Var_INT){
            if(isLocalVarIntType(value0->VTy)){ //接受为int
                printf("\tmov\tr%d,r0\n",dest_reg_abs);
                fprintf(fp,"\tmov\tr%d,r0\n",dest_reg_abs);
                if(dest_reg<0){
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(dest_reg_abs,x,0);

                }
            } else if(isLocalVarFloatType(value0->VTy)){ //接受为float
                printf("\tvmov\ts0,r0\n");
                fprintf(fp,"\tvmov\ts0,r0\n");
                printf("\tvcvt.f32.s32\ts0,s0\n");
                fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
                printf("\tvmov\tr%d,s0\n",dest_reg_abs);
                fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
                if(dest_reg<0){
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(dest_reg_abs,x,0);
                }
            }
        }
//    返回值为float
        if(func_param_type->pdata->symtab_func_pdata.return_type.ID==Var_FLOAT){
            if(isLocalVarIntType(value0->VTy)){ //接受为int
                printf("\tvcvt.s32.f32\ts0,s0\n");
                fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
                printf("\tvmov\tr%d,s0\n",dest_reg_abs);
                fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
                if(dest_reg<0){
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(dest_reg_abs,x,0);

                }


            } else if(isLocalVarFloatType(value0->VTy)){ //接受为float
                printf("\tvmov\tr%d,s0\n",dest_reg_abs);
                fprintf(fp,"\tvmov\tr%d,s0\n",dest_reg_abs);
                if(dest_reg<0){
                    int x= get_value_offset_sp(hashMap,value0);
                    handle_illegal_imm(dest_reg_abs,x,0);

                }
            }
        }
    }


//    else if(isLocalArrayIntType(value0->VTy)){
//        ;
//    }else if(isGlobalVarIntType(value0->VTy)){
////            ;这些都是需要补充完整的，不对，
////            全局变量会有相应的load和store指令，结果不应该在这里处理
////            这里只需要转换为相应的格式就可以了
//    }else if(isGlobalVarFloatType(value0->VTy)){
////                需要将相加的结果转化为IEEE754格式存放在r0中
////        printf("\tvmov\ts0,r%d\n",dest_reg_abs);
////        printf("\tvcvt.f32.s32\ts0,s0\n");
////        printf("\tvmov\tr%d,s0\n",dest_reg_abs);
//    }else if(isGlobalArrayIntType(value0->VTy)){
//        ;
//    }
//    if(reg_save[12]==1 && isSySYFunction(value1)){
//        printf("\tldr\tr12,[sp],#4\n");
//        fprintf(fp,"\tldr\tr12,[sp],#4\n");
//    }

    return ins;
}
InstNode *arm_tarns_SysYMemset(HashMap *hashMap,InstNode *ins){ //翻译sysymemset
    memset(give_param_flag,0, sizeof(give_param_flag));

    get_param_list(NULL,&give_count);
    func_param_type=NULL; //memset没有函数调用名对应的value，而且不需要类型匹配

    arm_trans_GIVE_PARAM(hashMap,3);
    printf("\tbl\tmemset\n");
    fprintf(fp,"\tbl\tmemset\n");

    memset(give_param_flag,0, sizeof(give_param_flag));
    return ins;
}
InstNode * arm_trans_FunBegin(InstNode *ins,int *stakc_size){
    memset(return_message,0, sizeof(return_message));
    int k;
    printf("\t.align\t2\n"
           "\t.global\t%s\n"
           "\t.arch armv7-a\n"
           "\t.syntax unified\n"
           "\t.arm\n"
           "\t.fpu vfp\n"
           "\t.type\t%s, %%function\n"
           ,user_get_operand_use(&ins->inst->user,0)->Val->name
           ,user_get_operand_use(&ins->inst->user,0)->Val->name);
    fprintf(fp,"\t.align\t2\n"
            "\t.global\t%s\n"
            "\t.arch armv7-a\n"
            "\t.syntax unified\n"
            "\t.arm\n"
            "\t.fpu vfp\n"
            "\t.type\t%s, %%function\n"
        ,user_get_operand_use(&ins->inst->user,0)->Val->name
        ,user_get_operand_use(&ins->inst->user,0)->Val->name);

    memset(reg_save,0, sizeof(reg_save));

//    reg_save[11]=1;fp帧指针并不是每个函数都需要保存，是在该函数调用了其他的函数的时候才需要保存，其实和lr是一样的
//    上面这种设置方法是错误的，r11是否需要保存取决于该函数有没有破坏r11寄存器，就是函数一开始的时候，有没有mov r11,sp这样的指令
//    这个mov r11,sp只要是开辟了局部栈也就是执行了sub 这个指令，那么都会执行mov r11,sp也就是破坏掉r11，这个时候就需要保存r11
    save_r11=1;
    reg_save[11]=1;
    memset(param_off,-1, sizeof(param_off));
    memset(funcName,0, sizeof(funcName));
    printf("%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    fprintf(fp,"%s:\n", user_get_operand_use(&ins->inst->user,0)->Val->name);
    strcpy(funcName,user_get_operand_use(&ins->inst->user,0)->Val->name);
    InstNode *tmp=ins;
//        这里好像是如果main中调用了其他的函数，就需要将r11,sp压栈,所以说需要查找一下有没有call
//        这个好像所有函数都是一样的，如果它调用了其他的函数的话，就需要保存r11(也称为fp)和sp，那这样的话
//        main函数好像和其他的函数就没有什么区别了
//    if(!strcmp("main", user_get_operand_use(&ins->inst->user,0)->Val->name)){
//        printf("    push {r11,lr}\n");
////        printf("    mov r1,sp\n");
//    }


//只要存在bl指令，就需要保存lr
    for(;tmp!=NULL && tmp->inst->Opcode!= FunEnd;tmp= get_next_inst(tmp)){
//        printf("\tid ;%d\t%d\t%d\t%d\n",tmp->inst->i,tmp->inst->_reg_[0],tmp->inst->_reg_[1],tmp->inst->_reg_[2]);
        handle_reg_save(tmp->inst->_reg_[0]);
        handle_reg_save(tmp->inst->_reg_[1]);
        handle_reg_save(tmp->inst->_reg_[2]);
//        数组初始化的时候会bl
        if(tmp->inst->Opcode==Alloca){
            Value *value0=&tmp->inst->user.value;
            if(isLocalArrayIntType(value0->VTy)|| isLocalArrayFloatType(value0->VTy)|| isGlobalArrayIntType(value0->VTy)||isGlobalArrayFloatType(value0->VTy)){
                func_call_func+=1;
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
                    func_call_func+=1;
                }
            }
        }
//        bl__aeabi_divimod
        if(tmp->inst->Opcode==Mod){
            func_call_func+=1;
        }
        if(tmp->inst->Opcode==Call){
            func_call_func+=1;
//            break;
        }

    }
    if(tmp->inst->Opcode==Return){
        handle_reg_save(tmp->inst->_reg_[0]);
        handle_reg_save(tmp->inst->_reg_[1]);
        handle_reg_save(tmp->inst->_reg_[2]);
    }
    for(int i=4;i<13;i++){
        if(reg_save[i]==1){
            stm_num++;
        }
    }
//    printf("stmnum1 %d\n",stm_num);
    k=stm_num;
    if(func_call_func>0){
        stm_num++;
    }
//    printf("stmnum2 %d\n",stm_num);
    if((stm_num%2)!=0){  //让stm是偶数个，就8字节对齐了，奇数个就随便多加一个好了
        if(k==9){ //r4-r12
            if(stm_num==9){
                func_call_func=1;
            }
        }else{
            for(int i=4;i<13;i++){
                if(reg_save[i]==0){
                    reg_save[i]=1;
                    break;
                }
            }
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

    int local_stack=0;//
//    int x= get_siezof_sp(hashMap);
//    printf("    sub sp,sp,#%d\n",4*x);
    //    在函数开始的时候要进行参数传递的str的处理
    int param_num=user_get_operand_use(&ins->inst->user,0)->Val->pdata->symtab_func_pdata.param_num;
    give_param_num=param_num;
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
//                reg0=ins->inst->_reg_[0];
//                handle_reg_save(reg0);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
                break;
            case Add:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case Sub:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case Mul:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case Div:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case Mod:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case Call:
//                operandNum=ins->inst->user.value.NumUserOperands; //这个表示use的value的个数（就是value1和value2的个数，跟value0无关）
//                if(operandNum==2){
                  if(returnValueNotUsed(ins)){ //返回值未被使用，不需要保存
                      value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                      FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                  }else{ //返回值被使用需要保存
                      value0 = &ins->inst->user.value;
                      value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                      if(value0->VTy!=Unknown){
//                          FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
//                      }
                      FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                      FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                  }

//                }else if(operandNum==1){
//                    value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
//                    FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                }

//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case Store:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case Load:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case GIVE_PARAM:
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg1);
                break;
            case LESS:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case GREAT:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case LESSEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case GREATEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case EQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case NOTEQ:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
                break;
            case XOR:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case zext:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case bitcast:
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack);
//                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
                break;
            case GEP:
//                value0代表存放位置，value1代表相对起始位置，value2代表偏移量
                value0 = &ins->inst->user.value;
                value1 = user_get_operand_use(&ins->inst->user, 0)->Val;
                value2 = user_get_operand_use(&ins->inst->user, 1)->Val;
//                FuncBegin_hashmap_bitcast_add(hashMap,value0,value1,&local_stack);
//                GEP指令已经进行了改变，所以说GEP操作的是数组，
//                alloc指令应该是再最前面的，也就如何GEP操作数组的时候对应了array的类型，不会被重新开辟
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
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
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
                FuncBegin_hashmap_add(hashMap,value2,name,&local_stack,ins->inst->_reg_[2]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                reg2=ins->inst->_reg_[2];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
//                handle_reg_save(reg2);
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
                FuncBegin_hashmap_add(hashMap,value0,name,&local_stack,ins->inst->_reg_[0]);
                FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
//                reg0=ins->inst->_reg_[0];
//                reg1=ins->inst->_reg_[1];
//                handle_reg_save(reg0);
//                handle_reg_save(reg1);
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
            FuncBegin_hashmap_add(hashMap,value1,name,&local_stack,ins->inst->_reg_[1]);
            int reg1=ins->inst->_reg_[1];
//            handle_reg_save(reg1);
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
    if((local_stack%8)!=0){
        local_stack+=4;
    }
    *stakc_size=local_stack;
//    if(local_stack!=0){
//        save_r11=1;
//    }
//    在sub之前，是需要先保护被破坏的寄存器,lr的保存也放在这里面吧
    printf_stmfd_rlist();

    if((*stakc_size)!=0){

//        这里还需要赋值fp
        if(imm_is_valid(*stakc_size)){
            printf("\tsub\tsp,sp,#%d\n",*stakc_size);
            fprintf(fp,"\tsub\tsp,sp,#%d\n",*stakc_size);
        }else{
            handle_illegal_imm1(4,*stakc_size);

            printf("\tsub\tsp,sp,r4\n");
            fprintf(fp,"\tsub\tsp,sp,r4\n");
        }



    }
//        调整fp帧指针,在一个函数中fp只需要在FunBegin的时候调整一次就可以了
//        具体实现的时候，需要将fp改为其对应的具体的寄存器
    printf("\tmov\tr11,sp\n");
    fprintf(fp,"\tmov\tr11,sp\n");

    if(param_num>0 && lineScan==0){
//        存在参数的传递
//        int local_stack=mystack;
        for(int j=0;j<param_num && j<4;j++){
//            printf("\tstr\tr%d,[sp,#%d]\n",j,local_stack+j*4);
//            fprintf(fp,"\tstr\tr%d,[sp,#%d]\n",j,local_stack+j*4);
            if(param_off[j]!=-1){
//                如果传递过来的参数没有被用到的话，就不需要存了
                if(imm_is_valid2(param_off[j])){
                    printf("\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
                    fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
                }else{
                    handle_illegal_imm1(2,param_off[j]);
                    printf("\tstr\tr%d,[r11,r2]\n",j);
                    fprintf(fp,"\tstr\tr%d,[r11,r2]\n",j);
                }

            }
        }
    }else if(lineScan==1){
        Function *curFunction=ins->inst->Parent->Parent;
        char arr[3]="%";
        for(int j=0;j<param_num && j<4 ;j++){
            sprintf(arr+1,"%d",j);
            int flag=0;
            Pair *ptr_pair;
            HashMapFirst(curFunction->lineScanReg);
            while ((ptr_pair= HashMapNext(curFunction->lineScanReg))!=NULL){
                Value *value=(Value*)ptr_pair->key;
                value_register *r=(value_register*)ptr_pair->value;
                if(strcmp(arr,value->name)==0){
                    printf("\tmov\tr%d,r%d\n",r->reg,j);
                    fprintf(fp,"\tmov\tr%d,r%d\n",r->reg,j);
                    flag=1;
                    break;
                }
            }
            if(flag==0){
//                说明%j参数没有被分配到寄存器，所以说需要回存到内存中
                if(param_off[j]!=-1){
//                如果传递过来的参数没有被用到的话，就不需要存了
                    if(imm_is_valid2(param_off[j])){
                        printf("\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
                        fprintf(fp,"\tstr\tr%d,[r11,#%d]\n",j,param_off[j]);
                    }else{
                        handle_illegal_imm1(4,param_off[j]);
                        printf("\tstr\tr%d,[r11,r4]\n",j);
                        fprintf(fp,"\tstr\tr%d,[r11,r4]\n",j);
                    }
                }
            }
        }
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

//还需要对返回值进行处理
    int operandNum=ins->inst->user.value.NumUserOperands;
    if(operandNum!=0){ //返回值非void,value1为进行返回得值
        Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
        int left_reg= ins->inst->_reg_[1];

//        返回值为int型，通过r0返回
        if(func_return_type->pdata->symtab_func_pdata.return_type.ID==Var_INT){
            if(isImmIntType(value1->VTy)){
                if(imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                } else{
                    int x=value1->pdata->var_pdata.iVal;
                    handle_illegal_imm1(0,x);

                }
            } else if(isImmFloatType(value1->VTy)){ //将浮点数转化为int存入r0
                float  x=value1->pdata->var_pdata.fVal;
                int xx=*(int*)&x;
                handle_illegal_imm1(0,xx);
//                char arr[12]="0x";
//                sprintf(arr+2,"%0x",xx);
//                printf("\tldr\tr0,=%s\n",arr);
//                fprintf(fp,"\tldr\tr0,=%s\n",arr);
                float_to_int(0,0);

            } else if(isLocalVarIntType(value1->VTy)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
            } else if(isLocalVarFloatType(value1->VTy)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1); //已经将浮点数加载到了left_reg-100
                    printf("\tvmov\ts0,r%d\n",left_reg-100);
                    fprintf(fp,"\tvmov\ts0,r%d\n",left_reg-100);
                    printf("\tvcvt.s32.f32\ts0,s0\n");
                    fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
                    printf("\tvmov\tr0,s0\n");
                    fprintf(fp,"\tvmov\tr0,s0\n");
                }else{
                    printf("\tvmov\ts0,r%d\n",left_reg);
                    fprintf(fp,"\tvmov\ts0,r%d\n",left_reg);
                    printf("\tvcvt.s32.f32\ts0,s0\n");
                    fprintf(fp,"\tvcvt.s32.f32\ts0,s0\n");
                    printf("\tvmov\tr0,s0\n");
                    fprintf(fp,"\tvmov\tr0,s0\n");
                }

            }
        }
//        返回类型为float，需要将返回值放入s0
        else if(func_return_type->pdata->symtab_func_pdata.return_type.ID==Var_FLOAT){
            if(isImmIntType(value1->VTy)){
                if(imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                } else{
                    int x=value1->pdata->var_pdata.iVal;
                    handle_illegal_imm1(0,x);
                }
                printf("\tvmov\ts0,r0\n");
                fprintf(fp,"\tvmov\ts0,r0\n");
                printf("\tvcvt.f32.s32\ts0,s0\n");
                fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            } else if(isImmFloatType(value1->VTy)){
                float  x=value1->pdata->var_pdata.fVal;
                int xx=*(int*)&x;
                handle_illegal_imm1(0,xx);
//                char arr[12]="0x";
//                sprintf(arr+2,"%0x",xx);
//                printf("\tldr\tr0,=%s\n",arr);
//                fprintf(fp,"\tldr\tr0,=%s\n",arr);
                printf("\tvmov\ts0,r0\n");
                fprintf(fp,"\tvmov\ts0,r0\n");
            } else if(isLocalVarIntType(value1->VTy)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
                printf("\tvmov\ts0,r0\n");
                fprintf(fp,"\tvmov\ts0,r0\n");
                printf("\tvcvt.f32.s32\ts0,s0\n");
                fprintf(fp,"\tvcvt.f32.s32\ts0,s0\n");
            } else if(isLocalVarFloatType(value1->VTy)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
                printf("\tvmov\ts0,r0\n");
                fprintf(fp,"\tvmov\ts0,r0\n");
            }
        }



//        if(isImmIntType(value1->VTy)){
//            if(imm_is_valid(value1->pdata->var_pdata.iVal)){
//                printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
//                fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
//            } else{
//                int x=value1->pdata->var_pdata.iVal;
//                handle_illegal_imm1(0,x);
//
//            }
//        } else if(isImmFloatType(value1->VTy)){
//            float  x=value1->pdata->var_pdata.fVal;
//            int xx=*(int*)&x;
//            char arr[12]="0x";
//            sprintf(arr+2,"%0x",xx);
//            printf("\tldr\tr0,=%s\n",arr);
//            fprintf(fp,"\tldr\tr0,=%s\n",arr);
//        } else if(isLocalVarIntType(value1->VTy)){
//            if(left_reg>100){
//                int x= get_value_offset_sp(hashMap,value1);
//                handle_illegal_imm(left_reg,x,1);
//
//                printf("\tmov\tr0,r%d\n",left_reg-100);
//                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
//            }else{
//                printf("\tmov\tr0,r%d\n",left_reg);
//                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
//            }
//        } else if(isLocalVarFloatType(value1->VTy)){
//            ;
//            if(left_reg>100){
//                int x= get_value_offset_sp(hashMap,value1);
//                handle_illegal_imm(left_reg,x,1);
//
//                printf("\tmov\tr0,r%d\n",left_reg-100);
//                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
//            }else{
//                printf("\tmov\tr0,r%d\n",left_reg);
//                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
//            }
//        }
    }

    int x1= stack_size;
    if(x1!=0){
        if(imm_is_valid(x1)){
            printf("\tadd\tsp,sp,#%d\n",x1);
            fprintf(fp,"\tadd\tsp,sp,#%d\n",x1);
//            char mesg[30];
//            sprintf(mesg,"\tadd\tsp,sp,#%d\n",x1);
//            strcat(return_message,mesg);
        }else{
            handle_illegal_imm1(2,x1);

//            char arr1[12]="#0x";
//            sprintf(arr1+3,"%0x",x1&AND_LOW);
//            printf("\tmovw\tr%d,%s\n",2,arr1);
//            fprintf(fp,"\tmovw\tr%d,%s\n",2,arr1);
//            strcat(return_message,"\tmovw\tr2,");
//            strcat(return_message,arr1);
//            strcat(return_message,"\n");

//            char arr2[12]="#0x";
//            sprintf(arr2+3,"%0x",(x1 >> MOVE_RIGHT)&0xffff);
//            printf("\tmovt\tr%d,%s\n",2,arr2);
//            fprintf(fp,"\tmovt\tr%d,%s\n",2,arr2);

//            strcat(return_message,"\tmovt\tr2,");
//            strcat(return_message,arr2);
//            strcat(return_message,"\n");

            printf("\tadd\tsp,sp,r2\n");
            fprintf(fp,"\tadd\tsp,sp,r2\n");

//            strcat(return_message,"\tadd\tsp,sp,r2\n");

        }

    }
//    恢复被保护的寄存器
    printf_ldmfd_rlist();

//    if(func_call_func>0){
//        printf("\tldmfd\tsp!,{r11,lr}\n");
//        fprintf(fp,"\tldmfd\tsp!,{r11,lr}\n");
//    }
    printf("\tbx\tlr\n");
    fprintf(fp,"\tbx\tlr\n");
//    strcat(return_message,"\tbx\tlr\n");

//    这里先加入一个固定的文字池
//    printf("\tb\t.ROG_%d\n",ltorg_num);
//    fprintf(fp,"\tb\t.ROG_%d\n",ltorg_num);
    char mesg[30];
    sprintf(mesg,"\tb\t.ROG_%d\n",ltorg_num);
    strcat(return_message,mesg);

//    printf("\t.ltorg\n\t.space 200\n");
//    fprintf(fp,"\t.ltorg\n\t.space 200\n");
    strcat(return_message,"\t.ltorg\n\t.space 200\n");

//    printf(".ROG_%d:\n",ltorg_num);
//    fprintf(fp,".ROG_%d:\n",ltorg_num);
    memset(mesg,0, sizeof(mesg));
    sprintf(mesg,".ROG_%d:\n",ltorg_num);
    strcat(return_message,mesg);
    ltorg_num++;
//    if(strcmp(funcName,"main")==0){
//        printf("\t.size\t%s, .-%s\n\n",funcName,funcName);
//        fprintf(fp,"\t.size\t%s, .-%s\n\n",funcName,funcName);
//    }
    char mesg2[1000];
    sprintf(mesg2,"\t.size\t%s, .-%s\n\n",funcName,funcName);
    strcat(return_message,mesg2);

    return ins;
}
InstNode *arm_trans_FuncEnd(InstNode*ins){
//    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    printf("here is FuncEnd\n");
    printf("%s",return_message);
    fprintf(fp,"%s",return_message);
    memset(return_message,0, sizeof(return_message));
//    printf("funcEnd\n");
//    printf("hello\n");
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
    return ins;
}
InstNode * arm_trans_Alloca(InstNode *ins,HashMap*hashMap){
//    在汇编中，alloca不需要翻译,但是栈帧分配的时候需要用到。
// 这个现在暂定是用来进行数组的初始话操作。bl memset来进行处理，就不需要调用多次store
//    Value *value0=&ins->inst->user.value;
////    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    if(isLocalArrayIntType(value0->VTy)|| isLocalArrayFloatType(value0->VTy)|| isGlobalArrayIntType(value0->VTy)||isGlobalArrayFloatType(value0->VTy)){
////        printf("%s\n",value0->alias->name);
//        int x=get_value_offset_sp(hashMap,value0);
//        printf("\tadd\tr0,r11,#%d\n",x);
//        fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
//        printf("\tmov\tr1,#0\n");
//        fprintf(fp,"\tmov\tr1,#0\n");
//        x= get_array_total_occupy(value0->alias,0);
//        printf("\tmov\tr2,#%d\n",x);
//        fprintf(fp,"\tmov\tr2,#%d\n",x);
//        printf("\tbl\tmemset\n");
//
//        fprintf(fp,"\tbl\tmemset\n");
//    }
    return ins;
}

InstNode * arm_trans_GIVE_PARAM(HashMap*hashMap,int param_num){

//只有int和float相互转换的时候需要强制转换，其他的是不需要的。函数定义参数的类型只有三种var_int,var_float和address

//  这边需要修改的就是，在call的时候将function对应的Value*存放在了func_param_type这个全局变量里面
//  在翻译give_param语句的时候，需要知道当前正在翻译的是第几个参数，这个和tmp=one_param[i]的i是一致的
//  在传递第i个参数的时候，需要将give_param的value的类型和func_param_type的参数列表的类型进行对应，如果不是的话则需要进行类型转换


//  现在需要做的就是数组传参的处理，有可能是全局数组传参，也有可能是局部数组传参
//  如果是数组传参的话，前面肯定是有一条GEP指令计算出起数组首地址，一边是常数偏移量为0
//  然后give_param的数组首地址类型被标为address
//  还有就是因为数组传参之前的GEP已经计算出了数组首地址的绝对地址（将r11加上了）这里就不用加了，直接mov


//  现在需要处理的操作就是有可能参数传的是全局变量的情况
//  但是全局变量要进行传参的话，跟局部变量是一致的，全局变量会先被load，giveparam里面是不会出现globel类型的参数的
//  所以说这个应该不需要改


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
//            printf(" id : %d\t",tmp->inst->i);
            int left_reg= tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
//            判断传递参数时的give_param的类型和被调用函数期望的类型是否一致？
//            if(value1->VTy->ID==func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID){
//
//            }
            // 对于全局变量来说是可以直接调用的，并不需要通过give_param来进行传递，但是也是会出现那全局变量来传参的情况，但是不影响
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                if(func_param_type!=NULL) assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID!=AddressTyID);

                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    if(func_param_type!=NULL &&func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
//                        传入为int,接受为float
                        int_to_float(i,i);
                    }

                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    handle_illegal_imm1(i,value1->pdata->var_pdata.iVal);
                    if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                        int_to_float(i,i);
                    }
                } else if(isImmFloatType(value1->VTy)){
                    handle_illegal_imm1(i,value1->pdata->var_pdata.iVal);
//                    char arr[12]="0x";
//                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
//                    printf("\tldr\tr%d,=%s\n",i,arr);
//                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                    if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                        printf("\tvmov\ts%d,r%d\n",i,i);
                        fprintf(fp,"\tvmov\ts%d,r%d\n",i,i);
                    }else if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_INT){
                        float_to_int(i,i);
                    }
                }
            }else{
//              变量的情况，全局变量应该不用传参，需要传参的只是局部变量和立即数
                if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                    if(func_param_type!=NULL) assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID!=AddressTyID);
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                    if(isLocalVarIntType(value1->VTy)){
                        if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                            int_to_float(i,i);
                        }
                    }else if(isLocalVarFloatType(value1->VTy)){
                        if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                            printf("\tvmov\ts%d,r%d\n",i,i);
                            fprintf(fp,"\tvmov\ts%d,r%d\n",i,i);
                        }else if(func_param_type!=NULL && func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_INT){
                            float_to_int(i,i);
                        }
                    }
                }
//                else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
////                    这个需要另外处理
//                    int x= get_value_offset_sp(hashMap,value1);
//                    printf("\tadd\tr%d,r11,#%d\n",i,x);
//                    fprintf(fp,"\tadd\tr%d,r11,#%d\n",i,x);
//                }
//  表示这个是数组传参，传的是数组首地址，之前是有GEP指令计算过数组首地址的了，所以说这里会被表示为address
                else if(value1->VTy->ID==AddressTyID){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);

                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                }

//                这里还需要加上特地为SysYMemset判断的数组类型
                else if(isLocalArrayIntType(value1->VTy) || isLocalArrayFloatType(value1->VTy)){
                    int x= get_value_offset_sp(hashMap,value1);
                    if(imm_is_valid(x)){
                        if(x==0){
                            printf("\tmov\tr%d,r11\n",i);
                            fprintf(fp,"\tmov\tr%d,r11\n",i);
                        }else{
                            printf("\tadd\tr%d,r11,#%d\n",i,x);
                            fprintf(fp,"\tadd\tr%d,r11,#%d\n",i,x);
                        }
                    }else{
                        handle_illegal_imm1(1,x);
                    }
                }
                else if(isGlobalArrayIntType(value1->VTy) || isGlobalArrayFloatType(value1->VTy)){
                    printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                    fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                    printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                    fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                }
                else{
                    assert(false);
                }



            }
//            直接在这个地方判断类型，然后加上add ri,sp,#%d好像就可以了
            give_param_flag[i]=1;
        }
    }else{
        int i;
        int k=num-1;
        int temp=k;

        for (int j = num-4; j > 0; j--) {
//            原因好像是在这里，应该逆序压栈，所以之前的tmp=one_param[i++]是有问题的，
//            和之前不同的是one_param里面的参数是从0-(num-1)排布的，并不是说帮我修改为应该压栈的顺序
            temp=k;
            tmp=one_param[k--];
            int left_reg=tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID!=AddressTyID);
                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr0,#%d\n",value1->pdata->var_pdata.iVal);
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_FLOAT){
//                        传入为int,接受为float
                        int_to_float(0,0);
                    }

                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    handle_illegal_imm1(0,value1->pdata->var_pdata.iVal);
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_FLOAT){
//                        传入为int,接受为float
                        int_to_float(0,0);
                    }
                } else if(isImmFloatType(value1->VTy)){
                    handle_illegal_imm1(0,value1->pdata->var_pdata.iVal);
//                    char arr[12]="0x";
//                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
//                    printf("\tldr\tr0,=%s\n",arr);
//                    fprintf(fp,"\tldr\tr0,=%s\n",arr);
//                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_FLOAT){
//                        这里不需要处理
//                        printf("\tvmov\ts0,r0\n");
//                        fprintf(fp,"\tvmov\ts0,r0\n");
//                    }
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_INT){
                        float_to_int(0,0);
                    }
                }
            }
            else if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID!=AddressTyID);
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
                if(isLocalVarIntType(value1->VTy)){
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_FLOAT){
                        int_to_float(0,0);
                    }
                }else if(isLocalVarFloatType(value1->VTy)){
//                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_FLOAT){
//                        printf("\tvmov\ts0,r0\n");
//                        fprintf(fp,"\tvmov\ts0,r0\n");
//                    }
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[temp].ID==Var_INT){
                        float_to_int(0,0);
                    }
                }
            }
//            else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
////                    这个需要另外处理
//                int x= get_value_offset_sp(hashMap,value1);
//                printf("\tadd\tr0,r11,#%d\n",x);
//                fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
//            }
            else if(value1->VTy->ID==AddressTyID){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tmov\tr0,r%d\n",left_reg-100);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
                }else{
                    printf("\tmov\tr0,r%d\n",left_reg);
                    fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
                }
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
        // 可以考虑一下，把这一部分代码放在后面，就是先传超出的参数，再去传前四个参数
        for(i=1;i<4;++i){

            tmp=one_param[i];
            int left_reg= tmp->inst->_reg_[1];
            Value *value1= user_get_operand_use(&tmp->inst->user,0)->Val;
            if(isImmIntType(value1->VTy)|| isImmFloatType(value1->VTy)){
                assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID!=AddressTyID);
                if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
                    printf("\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    fprintf(fp,"\tmov\tr%d,#%d\n",i,value1->pdata->var_pdata.iVal);
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
//                        传入为int,接受为float
                        int_to_float(i,i);
                    }
                } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                    handle_illegal_imm1(i,value1->pdata->var_pdata.iVal);
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
//                        传入为int,接受为float
                        int_to_float(i,i);
                    }
                } else if(isImmFloatType(value1->VTy)){
                    handle_illegal_imm1(i,value1->pdata->var_pdata.iVal);
//                    char arr[12]="0x";
//                    sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
//                    printf("\tldr\tr%d,=%s\n",i,arr);
//                    fprintf(fp,"\tldr\tr%d,=%s\n",i,arr);
                    if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                        printf("\tvmov\ts%d,r%d\n",i,i);
                        fprintf(fp,"\tvmov\ts%d,r%d\n",i,i);
                    }else if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_INT){
                        float_to_int(i,i);
                    }
                }
            }else{
//                变量的情况，全局变量应该不用传参，需要传参的只是局部变量和立即数
                if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
                    assert(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID!=AddressTyID);
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);
                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                    if(isLocalVarIntType(value1->VTy)){
                        if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                            int_to_float(i,i);
                        }
                    }else if(isLocalVarFloatType(value1->VTy)){
                        if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_FLOAT){
                            printf("\tvmov\ts%d,r%d\n",i,i);
                            fprintf(fp,"\tvmov\ts%d,r%d\n",i,i);
                        }else if(func_param_type->pdata->symtab_func_pdata.param_type_lists[i].ID==Var_INT){
                            float_to_int(i,i);
                        }
                    }
                }
//                else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
////                    这个需要另外处理
//                    int x= get_value_offset_sp(hashMap,value1);
//                    printf("\tadd\tr%d,r11,#%d\n",i,x);
//                    fprintf(fp,"\tadd\tr%d,r11,#%d\n",i,x);
//                }
                else if(value1->VTy->ID==AddressTyID){
                    if(left_reg>100){
                        int x= get_value_offset_sp(hashMap,value1);
                        handle_illegal_imm(left_reg,x,1);

                        printf("\tmov\tr%d,r%d\n",i,left_reg-100);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg-100);
                    }else{
                        printf("\tmov\tr%d,r%d\n",i,left_reg);
                        fprintf(fp,"\tmov\tr%d,r%d\n",i,left_reg);
                    }
                }
            }
            give_param_flag[i]=1;
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
                if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_FLOAT){
                    int_to_float(0,0);
                }
            } else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
                handle_illegal_imm1(0,value1->pdata->var_pdata.iVal);
                if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_FLOAT){
                    int_to_float(0,0);
                }
            } else if(isImmFloatType(value1->VTy)){
                handle_illegal_imm1(0,value1->pdata->var_pdata.iVal);
//                char arr[12]="0x";
//                sprintf(arr+2,"%0x",value1->pdata->var_pdata.iVal);
//                printf("\tldr\tr0,=%s\n",arr);
//                fprintf(fp,"\tldr\tr0,=%s\n",arr);
                if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_FLOAT){
                    printf("\tvmov\ts0,r0\n");
                    fprintf(fp,"\tvmov\ts0,r0\n");
                }else if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_INT){
                    float_to_int(0,0);
                }
            }
        }
        else if(isLocalVarIntType(value1->VTy)|| isLocalVarFloatType(value1->VTy)){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            }
            if(isLocalVarIntType(value1->VTy)){
                if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_FLOAT){
                    int_to_float(0,0);
                }
            }else if(isLocalVarFloatType(value1->VTy)){
                if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_FLOAT){
                    printf("\tvmov\ts0,r0\n");
                    fprintf(fp,"\tvmov\ts0,r0\n");
                }else if(func_param_type->pdata->symtab_func_pdata.param_type_lists[0].ID==Var_INT){
                    float_to_int(0,0);
                }
            }
        }
//        else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
////                    这个需要另外处理
//            int x= get_value_offset_sp(hashMap,value1);
//            printf("\tadd\tr0,r11,#%d\n",x);
//            fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
//        }
        else if(value1->VTy->ID==AddressTyID){
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tmov\tr0,r%d\n",left_reg-100);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg-100);
            }else{
                printf("\tmov\tr0,r%d\n",left_reg);
                fprintf(fp,"\tmov\tr0,r%d\n",left_reg);
            }
        }
    }



    return NULL;
}

InstNode * arm_trans_ALLBEGIN(InstNode *ins){
//    int i=ins->inst->i;
//    printf("**********ALLBEGIN**************\n");
    printf("\t.arch armv7-a\n"
//           "\t.eabi_attribute 28, 1\n"
//           "\t.eabi_attribute 20, 1\n"
//           "\t.eabi_attribute 21, 1\n"
//           "\t.eabi_attribute 23, 3\n"
//           "\t.eabi_attribute 24, 1\n"
//           "\t.eabi_attribute 25, 1\n"
//           "\t.eabi_attribute 26, 2\n"
//           "\t.eabi_attribute 30, 6\n"
//           "\t.eabi_attribute 34, 1\n"
//           "\t.eabi_attribute 18, 4\n"
//           "\t.file\t\"%s\"\n"
           "\t.text\n");
    fprintf(fp,"\t.arch armv7-a\n"
//            "\t.eabi_attribute 28, 1\n"
//            "\t.eabi_attribute 20, 1\n"
//            "\t.eabi_attribute 21, 1\n"
//            "\t.eabi_attribute 23, 3\n"
//            "\t.eabi_attribute 24, 1\n"
//            "\t.eabi_attribute 25, 1\n"
//            "\t.eabi_attribute 26, 2\n"
//            "\t.eabi_attribute 30, 6\n"
//            "\t.eabi_attribute 34, 1\n"
//            "\t.eabi_attribute 18, 4\n"
//           "\t.file\t\"%s\"\n"
           "\t.text\n");
//    printf("\t.text\n"
//           "\t.align\t2\n"
//           "\t.global\tmain\n"
//           "\t.fpu\tvfp\n"
//           "\t.type\tmain, %%function\n"
//           "\t.code\t32\n");
//    fprintf(fp,"\t.text\n"
//           "\t.align\t2\n"
//           "\t.global\tmain\n"
//           "\t.fpu\tvfp\n"
//           "\t.type\tmain, %%function\n"
//           "\t.code\t32\n");
    return ins;
}

InstNode * arm_trans_LESS_GREAT_LEQ_GEQ_EQ_NEQ(InstNode *ins,HashMap*hashMap){
    Value *value0=&ins->inst->user.value;
    Value *value1=user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs=abs(dest_reg);
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
            handle_illegal_imm1(1,x1);

            printf("\tcmp\tr1,#%d\n",x2);
            fprintf(fp,"\tcmp\tr1,#%d\n",x2);
        } else if((imm_is_valid(x1))&&(!imm_is_valid(x2))){
            handle_illegal_imm1(2,x2);

            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            printf("\tcmp\tr1,r2\n");
            fprintf(fp,"\tcmp\tr1,r2\n");
        }else{
            handle_illegal_imm1(1,x1);
            handle_illegal_imm1(2,x2);
//            char arr1[12]="0x";
//            sprintf(arr1+2,"%0x",x1);
//            char arr2[12]="0x";
//            sprintf(arr2+2,"%0x",x2);
//            printf("\tldr\tr1,=%s\n",arr1);
//            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
//            printf("\tldr\tr2,=%s\n",arr2);
//            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
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
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            int *xx2=(int*)&x2;
            handle_illegal_imm1(2,*xx2);
//            char arr2[12]="0x";
//            sprintf(arr2+2,"%0x",*xx2);
//            printf("\tldr\tr2,=%s\n",arr2);
//            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
            printf("\tvmrs\tAPSR_nzcv,fpscr\n");
            fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

            int *xx2=(int*)&x2;
            handle_illegal_imm1(2,*xx2);
//            char arr2[12]="0x";
//            sprintf(arr2+2,"%0x",*xx2);
//            printf("\tldr\tr2,=%s\n",arr2);
//            fprintf(fp,"\tldr\tr2,=%s\n",arr2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");

            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
            printf("\tvmrs\tAPSR_nzcv,fpscr\n");
            fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
        }

    }
    if(isImmFloatType(value1->VTy)&&isImmIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
            int *xx1=(int*)&x1;
            handle_illegal_imm1(1,*xx1);
//            char arr1[12]="0x";
//            sprintf(arr1+2,"%0x",*xx1);
//            printf("\tldr\tr1,=%s\n",arr1);
//            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");

            printf("\tmov\tr2,#%d\n",x2);
            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
            printf("\tvmrs\tAPSR_nzcv,fpscr\n");
            fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
        }else{
            int *xx1=(int*)&x1;
            handle_illegal_imm1(1,*xx1);
//            char arr1[12]="0x";
//            sprintf(arr1+2,"%0x",*xx1);
//            printf("\tldr\tr1,=%s\n",arr1);
//            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            printf("\tvcmp.f32\ts1,s2\n");
            fprintf(fp,"\tvcmp.f32\ts1,s2\n");
            printf("\tvmrs\tAPSR_nzcv,fpscr\n");
            fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
        }
    }
    if(isImmFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        float x2=value2->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");

        int *xx2=(int*)&x2;
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }


    if(isImmIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        int x1=value1->pdata->var_pdata.iVal;
        if(imm_is_valid(x1)){
            printf("\tmov\tr1,#%d\n",x1);
            fprintf(fp,"\tmov\tr1,#%d\n",x1);
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tcmp\tr1,r%d\n",right_reg-100);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg-100);
            }else{
                printf("\tcmp\tr1,r%d\n",right_reg);
                fprintf(fp,"\tcmp\tr1,r%d\n",right_reg);
            }

        }else{
            handle_illegal_imm1(1,x1);

            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

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
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }else{
            handle_illegal_imm1(1,x1);

            printf("\tvmov\ts1,r1\n");
            fprintf(fp,"\tvmov\ts1,r1\n");
            printf("\tvcvt.f32.s32 s1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32 s1,s1\n");
            if(right_reg>100){
                int x= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x,2);

                printf("\tvmov\ts2,r%d\n",right_reg-100);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            }else{
                printf("\tvmov\ts2,r%d\n",right_reg);
                fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            }
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvcvt.f32.s32\ts2,s2\n");
        fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }
    if(isImmFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        float x1=value1->pdata->var_pdata.fVal;
        int *xx1=(int*)&x1;
        handle_illegal_imm1(1,*xx1);
//        char arr1[12]="0x";
//        sprintf(arr1+2,"%0x",*xx1);
//        printf("\tldr\tr1,=%s\n",arr1);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr1);
        printf("\tvmov\ts1,r1\n");
        fprintf(fp,"\tvmov\ts1,r1\n");
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        }else{
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isImmIntType(value2->VTy)){
//        第二操作数是立即数，可以不需要移到寄存器，存储方式和mov的合法立即数是一样的
        int x2=value2->pdata->var_pdata.iVal;
        if(imm_is_valid(x2)){
//            printf("\tmov\tr2,#%d\n",x2);
//            fprintf(fp,"\tmov\tr2,#%d\n",x2);
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tcmp\tr%d,#%d\n",left_reg-100,x2);
                fprintf(fp,"\tcmp\tr%d,#%d\n",left_reg-100,x2);
            }else{
                printf("\tcmp\tr%d,#%d\n",left_reg,x2);
                fprintf(fp,"\tcmp\tr%d,#%d\n",left_reg,x2);
            }

        }else{
            handle_illegal_imm1(2,x2);

            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

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
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
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
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }else{
            handle_illegal_imm1(2,x2);

            printf("\tvmov\ts2,r2\n");
            fprintf(fp,"\tvmov\ts2,r2\n");
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
            if(left_reg>100){
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tvmov\ts1,r%d\n",left_reg-100);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            }else{
                printf("\tvmov\ts1,r%d\n",left_reg);
                fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            }
        }
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isImmFloatType(value2->VTy)){
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
        }else{
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
        }
        float x2=value2->pdata->var_pdata.fVal;
        int *xx2=(int*)&x2;
        handle_illegal_imm1(2,*xx2);
//        char arr2[12]="0x";
//        sprintf(arr2+2,"%0x",*xx2);
//        printf("\tldr\tr2,=%s\n",arr2);
//        fprintf(fp,"\tldr\tr2,=%s\n",arr2);
        printf("\tvmov\ts2,r2\n");
        fprintf(fp,"\tvmov\ts2,r2\n");
        printf("\tvcmp.f32\ts1,s2\n");
        fprintf(fp,"\tvcmp.f32\ts1,s2\n");
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }

    if(isLocalVarIntType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tcmp\tr%d,r%d\n",left_reg-100,right_reg-100);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg-100,right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tcmp\tr%d,r%d\n",left_reg-100,right_reg);
            fprintf(fp,"\tcmp\tr%d,r%d\n",left_reg-100,right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

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
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);
            printf("\tvcvt.f32.s32\ts1,s1\n");
            fprintf(fp,"\tvcvt.f32.s32\ts1,s1\n");

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
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarIntType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
            printf("\tvcvt.f32.s32\ts2,s2\n");
            fprintf(fp,"\tvcvt.f32.s32\ts2,s2\n");
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }
    if(isLocalVarFloatType(value1->VTy)&&isLocalVarFloatType(value2->VTy)){
        if(left_reg>100&&right_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);

            printf("\tvmov\ts2,r%d\n",right_reg-100);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg-100);
        } else if(left_reg>100){
            int x1= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x1,1);

            printf("\tvmov\ts1,r%d\n",left_reg-100);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg-100);
            printf("\tvmov\ts2,r%d\n",right_reg);
            fprintf(fp,"\tvmov\ts2,r%d\n",right_reg);
        }else if(right_reg>100){
            int x2= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x2,2);
            printf("\tvmov\ts1,r%d\n",left_reg);
            fprintf(fp,"\tvmov\ts1,r%d\n",left_reg);

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
        printf("\tvmrs\tAPSR_nzcv,fpscr\n");
        fprintf(fp,"\tvmrs\tAPSR_nzcv,fpscr\n");
    }

    if(ins->inst->Opcode==LESS){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmovlt\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmovlt\tr%d,#1\n",dest_reg_abs);
            printf("\tmovge\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovge\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbge\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tbge\t%sLABEL%d\n",funcName,x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);

            temp= get_next_inst(ins);
            if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                if(y!=x){
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            }else{
                printf("\tb\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
            }
        }
    } else if(ins->inst->Opcode==GREAT){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmovgt\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmovgt\tr%d,#1\n",dest_reg_abs);
            printf("\tmovle\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovle\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tble\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tble\t%sLABEL%d\n",funcName,x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);

            temp= get_next_inst(ins);
            if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                if(y!=x){
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            }else{
                printf("\tb\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
            }

        }
    } else if(ins->inst->Opcode==LESSEQ){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmovle\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmovle\tr%d,#1\n",dest_reg_abs);
            printf("\tmovgt\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovgt\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbgt\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tbgt\t%sLABEL%d\n",funcName,x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            temp= get_next_inst(ins);
            if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                if(y!=x){
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            }else{
                printf("\tb\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
            }
        }
    } else if(ins->inst->Opcode==GREATEQ){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmovge\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmovge\tr%d,#1\n",dest_reg_abs);
            printf("\tmovlt\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovlt\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tblt\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tblt\t%sLABEL%d\n",funcName,x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            temp= get_next_inst(ins);
            if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                if(y!=x){
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            }else{
                printf("\tb\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
            }
        }
    } else if(ins->inst->Opcode==EQ){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmoveq\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmoveq\tr%d,#1\n",dest_reg_abs);
            printf("\tmovne\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovne\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode == br_i1){
            ins= temp;
            int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
            printf("\tbne\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tbne\t%sLABEL%d\n",funcName,x);
            x= get_value_pdata_inspdata_true(&ins->inst->user.value);
            temp= get_next_inst(ins);
            if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                if(y!=x){
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            } else{
                printf("\tb\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
            }
        }
    } else if(ins->inst->Opcode==NOTEQ){
        if(JudgeIcmp(ins)){ //true表示需要保存到dest_reg里面
            printf("\tmovne\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmovne\tr%d,#1\n",dest_reg_abs);
            printf("\tmoveq\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmoveq\tr%d,#0\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }
        InstNode *temp= get_next_inst(ins);
        if(temp->inst->Opcode==XOR){
            ins=temp;
            dest_reg=temp->inst->_reg_[0];
            left_reg=temp->inst->_reg_[1];
            dest_reg_abs=abs(dest_reg);
            printf("\tmovne\tr%d,#0\n",dest_reg_abs);
            fprintf(fp,"\tmovne\tr%d,#0\n",dest_reg_abs);
            printf("\tmoveq\tr%d,#1\n",dest_reg_abs);
            fprintf(fp,"\tmoveq\tr%d,#1\n",dest_reg_abs);
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,&temp->inst->user.value);
                handle_illegal_imm(dest_reg_abs,x,0);
            }
        }else{
            if(temp->inst->Opcode == br_i1){
                ins= temp;
                int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
                printf("\tbeq\t%sLABEL%d\n",funcName,x);
                fprintf(fp,"\tbeq\t%sLABEL%d\n",funcName,x);
                x= get_value_pdata_inspdata_true(&ins->inst->user.value);
                temp= get_next_inst(ins);
                if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
                    int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
                    if(y!=x){
                        printf("\tb\t%sLABEL%d\n",funcName,x);
                        fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                    }
                } else{
                    printf("\tb\t%sLABEL%d\n",funcName,x);
                    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
                }
            }
        }

    }
    return ins;


}

InstNode * arm_trans_br_i1(InstNode *ins){
//    int i=ins->inst->i;
    int x= get_value_pdata_inspdata_false(&ins->inst->user.value);
    printf("\tbne\t%sLABEL%d\n",funcName,x);
    fprintf(fp,"\tbne\t%sLABEL%d\n",funcName,x);
    x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
        int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
        if(y!=x){
            printf("\tb\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
        }
    } else{
        printf("\tb\t%sLABEL%d\n",funcName,x);
        fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
    }
//    printf("\tb\t%sLABEL%d\n",funcName,x);
//    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
    return  ins;
}

InstNode * arm_trans_br(InstNode *ins){

    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    InstNode *temp= get_next_inst(ins);
    if(temp->inst->Opcode==Label && opt_label==1 && optimization==1){
        int y=temp->inst->user.value.pdata->instruction_pdata.true_goto_location;
        if(y!=x){
            printf("\tb\t%sLABEL%d\n",funcName,x);
            fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
        }
    } else{
        printf("\tb\t%sLABEL%d\n",funcName,x);
        fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
    }
//    printf("\tb\t%sLABEL%d\n",funcName,x);
//    fprintf(fp,"\tb\t%sLABEL%d\n",funcName,x);
    return ins;
}

InstNode * arm_trans_br_i1_true(InstNode *ins){
    assert(false);
    printf("arm_trans_br_i1_true\n");
    fprintf(fp,"arm_trans_br_i1_true\n");
    return ins;
}

InstNode * arm_trans_br_i1_false(InstNode *ins){
    assert(false);
    printf("arm_trans_br_i1_false\n");
    fprintf(fp,"arm_trans_br_i1_false\n");
    return ins;
}

InstNode * arm_trans_Label(InstNode *ins){
//强制跳转的位置
    int x= get_value_pdata_inspdata_true(&ins->inst->user.value);
    printf("%sLABEL%d:\n",funcName,x);
    fprintf(fp,"%sLABEL%d:\n",funcName,x);
    return ins;
}

InstNode * arm_trans_tmp(InstNode *ins){
    assert(false);
    printf("arm_trans_tmp\n");
    fprintf(fp,"arm_trans_tmp\n");
    return ins;
}

InstNode * arm_trans_XOR(InstNode *ins){
//    int dest_reg=ins->inst->_reg_[0];
//    int dest_reg_abs= abs(dest_reg);
//    int left_reg=ins->inst->_reg_[1];
//    int right_reg=ins->inst->_reg_[2];
    assert(false);
//    InstNode *prevIns = get_prev_inst(ins);
//    if(prevIns->inst->Opcode == EQ){
//        printf("\tmoveq\tr%d, #0\n",dest_reg_abs);
//        fprintf(fp,"\tmoveq\tr%d, #0\n",dest_reg_abs);
//        printf("\tmovne\tr%d, #1\n",dest_reg_abs);
//        fprintf(fp,"\tmovne\tr%d, #1\n",dest_reg_abs);
//    }else if(prevIns->inst->Opcode == NOTEQ){
//        printf("\tmoveq\tr%d, #1\n",dest_reg_abs);
//        fprintf(fp,"\tmoveq\tr%d, #1\n",dest_reg_abs);
//        printf("\tmovne\tr%d, #0\n",dest_reg_abs);
//        fprintf(fp,"\tmovne\tr%d, #0\n",dest_reg_abs);
//    }else{
//        assert(false);
//    }
//
//    if(dest_reg < 0){
//        //means we need to store back
//        assert(false);
//    }

    return ins;
}

InstNode * arm_trans_zext(InstNode *ins){
//i1扩展为i32
    assert(false);
    printf("arm_trans_zext\n");
    fprintf(fp,"arm_trans_zext\n");
    return ins;
}

InstNode * arm_trans_bitcast(InstNode *ins){
//类型转换，已经通过映射解决掉了bitcast产生的 多余的mov和load指令的问题
//    printf("arm_trans_bitcast\n");
//    assert(false);
    return ins;
}
//void multiply_and_add_instructions_for_translated_arrays(InstNode*ins,HashMap*hashMap){
//    Value *value0=&ins->inst->user.value;
//    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    Value *value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    int which_dimension=value0->pdata->var_pdata.iVal;
//    int off=0;
//    int dest_reg=ins->inst->_reg_[0];
//    int dest_reg_abs= abs(dest_reg);
//    int left_reg=ins->inst->_reg_[1];
//    int right_reg=ins->inst->_reg_[2];
//    if(left_reg>100){
//        int x=get_value_offset_sp(hashMap,value1);
//        printf("\tldr\tr%d,");
//        fprintf(fp,"\tldr\tr%d,");
//    }
//
//    return;
//}
InstNode * arm_trans_GMP(InstNode *ins,HashMap*hashMap){
// 因为之前设计上的问题，所以说需要把数组的load和store进行分离，这个的实现逻辑是需要改的
// 这个需要大改，ldr和str可以在这里处理掉，就不需要再去修改load和store对应的翻译了
// 使用乘加指令，首先需要记录当前所在的维数，这个lsy可能会存放在value0的ival里面(大于0就表示维数)
// 然后需要乘的数值在value2的ival里面，将其与后面维数的大小相乘(使用的是reg[2])
// 如果是常数的话，左值的ival放-1或者是-2，然后value2里面的ival存放的就是fixarray的最终结果
// 也就是直接给的相对于数组首地址的偏移量，不需要再进行相关的计算。
// 这个GEP指令的翻译逻辑应该是得修改一下

//现在是需要处理全局变量了，是不是全局变量是通过value1的类型来判断的
//全局变量数组的处理和普通数组的处理好像是差别不大的，就是第一条GEP的时候，value1对应为全局变量数组，
//也就是说其数组首地址的偏移量是不用从栈的hashmap中取出偏移量，从栈hashmap中取出数组首地址的偏移量直接就是int类型的数，可以直接用于计算
//但是对于全局数组来说，数组首地址对应的翻译为ldr rd,.LCPI_1_0这些，所以说第一个数组首地址并不能像局部数组首地址那样直接用于计算


//现在就是说在GEP的时候，如果是局部数组，直接将结果加上r11，这样直接就给出绝对地址
//如果说是全局数组的话，就不用加，计算出来的自然就是绝对地址了
    Value *value0,*value1,*value2;
    int dest_reg,dest_reg_abs,left_reg,right_reg,left_reg_abs;
    value0=&ins->inst->user.value;
    value1= user_get_operand_use(&ins->inst->user,0)->Val;
    value2= user_get_operand_use(&ins->inst->user,1)->Val;
    dest_reg=ins->inst->_reg_[0];
    dest_reg_abs=abs(dest_reg);
    left_reg=ins->inst->_reg_[1];
    right_reg=ins->inst->_reg_[2];
//    数组好像只有第一条GEP指令的value1类型是对的，之后的GEP指令对应的都是address,全局比那辆也是一样的，所以说可以利用value1的类型来判断是不是第一条GEP


//    现在计算GEP需要多添加一种情况。就是该数组是通过数组传参过来的数组，和之前的也是一样的，只有计算第一条GEP的时候方式不同，剩下的GEP是一样的计算步骤
//    之后是使用isParam（）函数判断该GEP是不是计算的数组传参的GEP，第一个参数是传GEP的数组首地址value1，第二个参数是传该函数的参数个数，这个只在FuncBegin的value里面存有
//    参数数组的第一条GEP
    if(value1->name[0]=='%' && isParam(value1,give_param_num)){ //这个isParam的实现很简单，就是判断%i是不是参数就可以了 i<param_num就代表其为参数
//        printf("isParam\n");
        int x;
        if(left_reg>100){
//            printf("%s\n",value1->name);
            x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);
            // left_reg_abs里面存放的是数组首地址的绝对地址
            left_reg_abs=left_reg-100;
        }else{
            left_reg_abs=left_reg;
        }
        assert(left_reg_abs!=0);

        int flag=value0->pdata->var_pdata.iVal;
        if(flag<0){
            int y=value2->pdata->var_pdata.iVal*4;
            if(imm_is_valid(y)){
                if(y!=0){
                    printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg_abs,y);
                    fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg_abs,y);
                }else{
                    if(dest_reg_abs!=left_reg_abs){
                        printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg_abs);
                        fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg_abs);
                    }

                }

            }else{
                handle_illegal_imm1(1,y);

                printf("\tadd\tr%d,r%d,r1\n",dest_reg_abs,left_reg_abs);
                fprintf(fp,"\tadd\tr%d,r%d,r1\n",dest_reg_abs,left_reg_abs);
            }
            if(dest_reg<0){
                x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
//            如果开的栈比较大，x也是非法立即数怎么办呢，这个也是需要处理的呀

            }
        }else{
            int which_dimension=value0->pdata->var_pdata.iVal;
            int result= array_suffix(value1->alias,which_dimension);
//            被乘的常数放在r2,r2与 right_reg相乘，再加上数组首地址的绝对地址left_reg_abs

            if(right_reg==0){ //right==0表示为立即数，没有进行寄存器分配
                int y=value2->pdata->var_pdata.iVal;
                y*=result;
                if(imm_is_valid(y)){
                    if(y!=0){
                        printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg_abs,y);
                        fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg_abs,y);
                    }else{
                        if(dest_reg_abs!=left_reg_abs){
                            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg_abs);
                            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg_abs);
                        }

                    }

                }else{
                    handle_illegal_imm1(2,y);

                    printf("\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg_abs);
                    fprintf(fp,"\tadd\tr%d,r%d,r2\n",dest_reg_abs,left_reg_abs);
                }
            }else{
                if(imm_is_valid(result)){
                    printf("\tmov\tr2,#%d\n",result);
                    fprintf(fp,"\tmov\tr2,#%d\n",result);
                }else{
                    handle_illegal_imm1(2,result);

                }
//          计算数组传参的第一条GEP，数组首地址是没有进行寄存器分配的
//          计算数组传参的第一条GEP，数组首地址是参数，所以说是进行了寄存器分配的left_reg不可能是0
//

                if(right_reg>100){
                    int x2= get_value_offset_sp(hashMap,value2);
                    handle_illegal_imm(right_reg,x2,2);

                    printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg_abs);
                    fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg_abs);
                }else{  //right > 0 && right < 100
                    printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg_abs);
                    fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg_abs);
                }
            }

            if(dest_reg<0){
                int z= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,z,0);
            }
        }
    }
//    局部变量的第一条GEP
    else if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)){
        int flag=value0->pdata->var_pdata.iVal;
        int off= get_value_offset_sp(hashMap,value1);
        assert(off!=-1);
//        if(value1 && value1->name && strcmp(value1->name,"%5")==0){
//            printf("hhe\n");
//        }
//        printf("%s %d\n",value1->name,off);
        if(flag<0){ //常数
            int x=value2->pdata->var_pdata.iVal*4;
//            printf("x  %d\n",x);
            x+=off;
            if(imm_is_valid(x)){
                if(x!=0){
                    printf("\tadd\tr%d,r11,#%d\n",dest_reg_abs,x);
                    fprintf(fp,"\tadd\tr%d,r11,#%d\n",dest_reg_abs,x);
                }else{
                    printf("\tmov\tr%d,r11\n",dest_reg_abs);
                    fprintf(fp,"\tmov\tr%d,r11\n",dest_reg_abs);
                }

            }else{
                handle_illegal_imm1(0,x);

                printf("\tadd\tr%d,r11,r0\n",dest_reg_abs);
                fprintf(fp,"\tadd\tr%d,r11,r0\n",dest_reg_abs);
            }
            if(dest_reg<0){
                x= get_value_offset_sp(hashMap,value0);
//            如果开的栈比较大，x也是非法立即数怎么办呢，这个也是需要处理的呀
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }else{ //非常数
//        flag大于零，需要使用乘加指令,
//        但是这里好像没有处理%1偏移一个a的情况,这个目前有效的处理方法是判断value1是否为address就可以了
//        如果是address，说明这不是基于数组首地址的，不然就说明这是基于数组首地址的
//        像这样的情况仅仅是会在一维数组中出现，但是lsy的一维数组处理好像是有问题的

//        还存在一个问题就是，数组的首地址是没有分配寄存器的，
//        如果left_reg=0就代表这个计算数组的第一条GEP，是基于数组首地址的，所以代码的逻辑得改一下，
//        还需要处理的就是，GEP直接计算出角绝对地址，load和store的时候不需要再加上r11，
//        所以说，如arr[a][b][c]生成多条GEP，但是只需要再其中一条GEP加上一个r11就可以了，就是第一条GEP的时候
            int which_dimension=value0->pdata->var_pdata.iVal;//当前所在的维数
            int result= array_suffix(value1->alias,which_dimension);

//            下面这个其实可以注释掉

            if(right_reg==0){ //非常数，但是其实给的是常数，只是lsy那里标错了
//                assert(false);
                int y=value2->pdata->var_pdata.iVal;
                y*=result;
                y+=off;
                if(imm_is_valid(y)){
                    if(y!=0){
                        printf("\tadd\tr%d,r11,#%d\n",dest_reg_abs,y);
                        fprintf(fp,"\tadd\tr%d,r11,#%d\n",dest_reg_abs,y);
                    }else{
                        printf("\tmov\tr%d,r11\n",dest_reg_abs);
                        fprintf(fp,"\tmov\tr%d,r11\n",dest_reg_abs);
                    }

                }else{
                    handle_illegal_imm1(2,y);

                    printf("\tadd\tr%d,r11,r2\n",dest_reg_abs);
                    fprintf(fp,"\tadd\tr%d,r11,r2\n",dest_reg_abs);
                }
            }else{
                //            计算数组的第一条GEP，基于数组首地址进行偏移，还需要加上r11
                if(left_reg==0){
                    int x1= get_value_offset_sp(hashMap,value1);//数组首地址的偏移量,这里可以直接r11加上数组首地址的偏移量就可以了
//                这里面也是需要判断x1是否为合法立即数的
                    if(imm_is_valid(x1)){
                        if(x1!=0){
                            printf("\tadd\tr1,r11,#%d\n",x1);
                            fprintf(fp,"\tadd\tr1,r11,#%d\n",x1);
                        }else{
                            printf("\tmov\tr1,r11\n");
                            fprintf(fp,"\tmov\tr1,r11\n");
                        }

                    }else{
                        handle_illegal_imm1(2,x1);

                        printf("\tadd\tr1,r11,r2\n");
                        fprintf(fp,"\tadd\tr1,r11,r2\n");
                    }
                    if(imm_is_valid(result)){
                        printf("\tmov\tr2,#%d\n",result);
                        fprintf(fp,"\tmov\tr2,#%d\n",result);
                    }else{
                        handle_illegal_imm1(2,result);

                    }
                    if(right_reg>100){
                        int x2= get_value_offset_sp(hashMap,value2);
                        handle_illegal_imm(right_reg,x2,2);
                        printf("\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg-100);
                        fprintf(fp,"\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg-100);
                    }else{
                        printf("\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg);
                        fprintf(fp,"\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg);
                    }
                }
            }

            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }
    }
//    全局变量的第一条GEP
    else if(isGlobalArrayIntType(value1->VTy)|| isGlobalArrayFloatType(value1->VTy)){
        int flag=value0->pdata->var_pdata.iVal;
        if(flag<0){
//            常数的计算
            int x=value2->pdata->var_pdata.iVal*4;
//            LCPTLabel *lcptLabel=(LCPTLabel*) HashMapGet(global_hashmap,value1);
//            if(lcptLabel==NULL){
//                printf("GEP Global error\n");
//            }
//            printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
//            fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
            printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
            fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
            printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
            fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
            if(imm_is_valid(x)){
                if(x!=0){
                    printf("\tadd\tr%d,r1,#%d\n",dest_reg_abs,x);
                    fprintf(fp,"\tadd\tr%d,r1,#%d\n",dest_reg_abs,x);
                }else{
                    printf("\tmov\tr%d,r1\n",dest_reg_abs);
                    fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);
                }

            }else{
                handle_illegal_imm1(0,x);
                printf("\tadd\tr%d,r1,r0\n",dest_reg_abs);
                fprintf(fp,"\tadd\tr%d,r1,r0\n",dest_reg_abs);
            }

            if(dest_reg<0){
                x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        } else{
//        flag大于零，需要使用乘加指令,
//        但是这里好像没有处理%1偏移一个a的情况,这个目前有效的处理方法是判断value1是否为address就可以了
//        如果是address，说明这不是基于数组首地址的，不然就说明这是基于数组首地址的
//        像这样的情况仅仅是会在一维数组中出现，但是lsy的一维数组处理好像是有问题的
            int which_dimension=value0->pdata->var_pdata.iVal;//当前所在的维数
            int result= array_suffix(value1->alias,which_dimension);
//            if(imm_is_valid(result)){
//                printf("\tmov\tr2,#%d\n",result);
//                fprintf(fp,"\tmov\tr2,#%d\n",result);
//            }else{
//                handle_illegal_imm1(2,result);
//
//            }
            if(right_reg==0){//非常数，但是其实给的是常数，只是lsy那里标错了
//                assert(false);
                int x;
//                LCPTLabel *lcptLabel=(LCPTLabel*) HashMapGet(global_hashmap,value1);
//                if(lcptLabel==NULL){
//                    printf("GEP Global error\n");
//                }
//                printf("\tldr\tr1,%s\n",lcptLabel->LCPI); //数组首地址的偏移量的绝对地址，而再局部数组中是数组首地址的偏移量+r11
//                fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
                printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                int y=value2->pdata->var_pdata.iVal;
                y*=result;
                if(imm_is_valid(y)){
                    if(y!=0){
                        printf("\tadd\tr%d,r1,#%d\n",dest_reg_abs,y);
                        fprintf(fp,"\tadd\tr%d,r1,#%d\n",dest_reg_abs,y);
                    }else{
                        printf("\tmov\tr%d,r1\n",dest_reg_abs);
                        fprintf(fp,"\tmov\tr%d,r1\n",dest_reg_abs);
                    }

                }else{
                    handle_illegal_imm1(2,y);

                    printf("\tadd\tr%d,r1,r2\n",dest_reg_abs);
                    fprintf(fp,"\tadd\tr%d,r1,r2\n",dest_reg_abs);
                }
            } else{
                if(imm_is_valid(result)){
                    printf("\tmov\tr2,#%d\n",result);
                    fprintf(fp,"\tmov\tr2,#%d\n",result);
                }else{
                    handle_illegal_imm1(2,result);

                }
                if(left_reg==0){
                    int x;
//                    LCPTLabel *lcptLabel=(LCPTLabel*) HashMapGet(global_hashmap,value1);
//                    if(lcptLabel==NULL){
//                        printf("GEP Global error\n");
//                    }
//                    printf("\tldr\tr1,%s\n",lcptLabel->LCPI); //数组首地址的偏移量的绝对地址，而再局部数组中是数组首地址的偏移量+r11
//                    fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
                    printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                    fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
                    printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                    fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
                    if(right_reg>100){
                        int x2= get_value_offset_sp(hashMap,value2);
                        handle_illegal_imm(right_reg,x2,2);

                        printf("\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg-100);
                        fprintf(fp,"\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg-100);
                    }else{
                        printf("\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg);
                        fprintf(fp,"\tmla\tr%d,r%d,r2,r1\n",dest_reg_abs,right_reg);
                    }
                }
            }
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }
    }

    // 非第一条GEP，局部数组和全局数组都是一样的处理
    else{
        int left_reg_flag=ins->inst->_reg_[1];
        assert(left_reg_flag!=0);
        int flag=value0->pdata->var_pdata.iVal;
        if(strcmp(value1->name,"%340")==0){
//            printf("\thello\n");
        }
        if(flag<0){
//            printf("GEP next1\n");
//            非第一条GEP而且是常数的偏移（其后不含变量）,常数的偏移的话直接add就可以了
//            想这种情况right_reg=0,直接取value2里面的值就可以了
            int x=value2->pdata->var_pdata.iVal*4;

            if(imm_is_valid(x)){
                if(left_reg>100){
                    int x1= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x1,1);

                    if(x!=0){
                        printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x);
                        fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,x);
                    }else{
                        if(dest_reg_abs!=left_reg-100){
                            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                        }

                    }

                }else{
                    if(x!=0){
                        printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x);
                        fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,x);
                    }else{
                        if(dest_reg_abs!=left_reg){
                            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                        }

                    }

                }
            }else{
                handle_illegal_imm1(0,x);
                if(left_reg>100){
                    int x1= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x1,1);

                    printf("\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg-100);
                    fprintf(fp,"\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg-100);
                }else{
                    printf("\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg);
                    fprintf(fp,"\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg);
                }
            }
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);
            }

        }else if(isImmIntType(value2->VTy)){
//            非第一条GEP，且常数，其后是非常数,这里可以直接value2.ival*维数计算之后的值，再和基准相加
            int y=value2->pdata->var_pdata.iVal;
            int which_dimension=value0->pdata->var_pdata.iVal;//当前所在的维数
            int result= array_suffix(value1->alias,which_dimension);
            result*=y;

            if(left_reg>100){
                int x1= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x1,1);
                if(imm_is_valid(result)){
                    if(result!=0){
                        printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,result);
                        fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg-100,result);
                    }else{
                        if(dest_reg_abs!=left_reg-100){
                            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg-100);
                        }

                    }
                }else{
                    handle_illegal_imm1(0,result);
                    printf("\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg-100);
                    fprintf(fp,"\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg-100);
                }



            }else{
                if(imm_is_valid(result)){
                    if(result!=0){
                        printf("\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,result);
                        fprintf(fp,"\tadd\tr%d,r%d,#%d\n",dest_reg_abs,left_reg,result);
                    }else{
                        if(dest_reg_abs!=left_reg){
                            printf("\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                            fprintf(fp,"\tmov\tr%d,r%d\n",dest_reg_abs,left_reg);
                        }

                    }

                }else{
                    handle_illegal_imm1(0,result);
                    printf("\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg);
                    fprintf(fp,"\tadd\tr%d,r%d,r0\n",dest_reg_abs,left_reg);
                }

            }
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }
        else{
//            非第一条GEP且非常数的偏移
//            printf("GEP next2\n");

            int which_dimension=value0->pdata->var_pdata.iVal;//当前所在的维数
            int result= array_suffix(value1->alias,which_dimension);
            if(imm_is_valid(result)){
                printf("\tmov\tr2,#%d\n",result);
                fprintf(fp,"\tmov\tr2,#%d\n",result);
            }else{
                handle_illegal_imm1(2,result);

            }
            if(left_reg>100&&right_reg>100){
                int x1= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x1,1);
                int x2= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x2,2);

                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg-100);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg-100);
            }else if(left_reg>100){
                int x1= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x1,1);

                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg-100);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg-100);
            }else if(right_reg>100){
                int x2= get_value_offset_sp(hashMap,value2);
                handle_illegal_imm(right_reg,x2,2);

                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg-100,left_reg);
            }else{
                printf("\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg);
                fprintf(fp,"\tmla\tr%d,r%d,r2,r%d\n",dest_reg_abs,right_reg,left_reg);
            }
            if(dest_reg<0){
                int x= get_value_offset_sp(hashMap,value0);
                handle_illegal_imm(dest_reg_abs,x,0);

            }
        }
    }




//    printf("\n");
    return ins;
}

InstNode * arm_trans_MEMCPY(InstNode *ins){
    assert(false);
//    涉及到数组的内容,后端不需要翻译这条ir和memcpy对应的ir
//    printf("arm_trans_MEMCPY\n");
    return ins;
}

InstNode * arm_trans_zeroinitializer(InstNode *ins){
    assert(false);
    printf("arm_trans_zeroinitializer\n");
    fprintf(fp,"arm_trans_zeroinitializer\n");
    return ins;
}

InstNode * arm_trans_GLOBAL_VAR(InstNode *ins){
// 使用.bss替代.data表示该全局变量没有被初始化，
// 现在的设计就是就当全部都初始化了的，如果没有初始化就赋0，
// 但是这样是不行的，应该先检测是否被初始化，没有放在.bss里面，有放在.data里面
//全局变量声明
    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    Value *value2=NULL;
    if(isGlobalArrayIntType(value1->VTy)|| isGlobalArrayFloatType(value1->VTy)){
//        说明这个全局变量是全局变量数组类型
//        它的初始化信息可以通过value1->pdata->symtab_array_pdata.is_init查看，如果为1表示已被初始化，0表示未被初始化
//        value1->pdata->symtab_array_pdata.array可以知道它被初始化的值，为零表示未被初始化，不为零表示已经进行了初始化
//        value1->pdata->symtab_array_pdata.f_array表示浮点型数组
        if(isGlobalArrayIntType(value1->VTy)){
            int arr_size= get_array_total_occupy(value1,0);
            int arr_num=arr_size/4;//获取数组总的元素个数
            if(value1->pdata->symtab_array_pdata.is_init==1){
//                这里处理的是已经初始化了的全局数组
                char name[270];
                sprintf(name,"\t.data\n%s:",value1->name+1);
                strcat(globalvar_message,name);
                int zero_count=0;
                for(int i=0;i<arr_num;i++){
                    if(value1->pdata->symtab_array_pdata.array[i]!=0){
                        if(zero_count>0){
                            strcat(globalvar_message,"\n\t.zero\t");
                            char value_int[12];
                            sprintf(value_int,"%d",zero_count*4);
                            strcat(globalvar_message,value_int);
//                            strcat(globalvar_message,"\n");
//                            printf(".zero %d\n", zero_count * 4);
                            zero_count = 0;
                        }
//                        printf(".long %d\n", value1->pdata->symtab_array_pdata.array[i]);
                        strcat(globalvar_message,"\n\t.long\t");
                        char value_int[12];
                        sprintf(value_int,"%d",value1->pdata->symtab_array_pdata.array[i]);
                        strcat(globalvar_message,value_int);
//                        strcat(globalvar_message,"\n");
                    }
                    else
                    {
                        zero_count++;
                    }
                }
                if (zero_count > 0)
                {
//                    printf(".zero %d\n", zero_count * 4);
                    strcat(globalvar_message,"\n\t.zero\t");
                    char value_int[12];
                    sprintf(value_int,"%d",zero_count*4);
                    strcat(globalvar_message,value_int);
                    strcat(globalvar_message,"\n");
                }else{
                    strcat(globalvar_message,"\n");
                }
            }else{
//                这里处理的是未初始化的全局数组
                char name[270];
                sprintf(name,"\t.bss\n%s:",value1->name+1);
                strcat(globalvar_message,name);
                strcat(globalvar_message,"\n\t.zero\t");
                char value_int[12];
                sprintf(value_int,"%d",arr_size);
                strcat(globalvar_message,value_int);
                strcat(globalvar_message,"\n");
            }
        }
        else if(isGlobalArrayFloatType(value1->VTy)){
            int arr_size= get_array_total_occupy(value1,0);
            int arr_num=arr_size/4;//获取数组总的元素个数
            if(value1->pdata->symtab_array_pdata.is_init==1){
//                这里处理的是已经初始化了的全局数组
                char name[270];
                sprintf(name,"\t.data\n%s:",value1->name+1);
                strcat(globalvar_message,name);
                int zero_count=0;
                for(int i=0;i<arr_num;i++){
                    if(value1->pdata->symtab_array_pdata.f_array[i]!=0){
                        if(zero_count>0){
                            strcat(globalvar_message,"\n\t.zero\t");
                            char value_int[12];
                            sprintf(value_int,"%d",zero_count*4);
                            strcat(globalvar_message,value_int);
//                            strcat(globalvar_message,"\n");
//                            printf(".zero %d\n", zero_count * 4);
                            zero_count = 0;
                        }
//                        printf(".long %d\n", value1->pdata->symtab_array_pdata.array[i]);
                        strcat(globalvar_message,"\n\t.long\t");
//                        对于float需要使用IEEE754格式
                        float x=value1->pdata->symtab_array_pdata.f_array[i];
                        int xx=*(int *)(&x);
                        char value_int[12];
                        sprintf(value_int,"%d",xx);
                        strcat(globalvar_message,value_int);
//                        strcat(globalvar_message,"\n");
                    }
                    else
                    {
                        zero_count++;
                    }
                }
                if (zero_count > 0)
                {
//                    printf(".zero %d\n", zero_count * 4);
                    strcat(globalvar_message,"\n\t.zero\t");
                    char value_int[12];
                    sprintf(value_int,"%d",zero_count*4);
                    strcat(globalvar_message,value_int);
                    strcat(globalvar_message,"\n");
                }else{
                    strcat(globalvar_message,"\n");
                }
            }else{
//                这里处理的是未初始化的全局数组
                char name[270];
                sprintf(name,"\t.bss\n%s:",value1->name+1);
                strcat(globalvar_message,name);
                strcat(globalvar_message,"\n\t.zero\t");
                char value_int[12];
                sprintf(value_int,"%d",arr_size);
                strcat(globalvar_message,value_int);
                strcat(globalvar_message,"\n");
            }
        }
    }else{
        value2= user_get_operand_use(&ins->inst->user,1)->Val;
    }

//    if(global_flag==0){
//        printf("\t.bss\n");
//        fprintf(fp,"\t.bss\n");
//    }
    if(isGlobalVarIntType(value1->VTy)){
        char name[270];
        sprintf(name,"\t.data\n%s:",value1->name+1);
        strcat(globalvar_message,name);
        strcat(globalvar_message,"\n\t.long\t");
        char value_int[12];
        sprintf(value_int,"%d",value1->pdata->var_pdata.iVal);
        strcat(globalvar_message,value_int);
        strcat(globalvar_message,"\n");
        
    } else if(isGlobalVarFloatType(value1->VTy)){
        char name[270];
        sprintf(name,"\t.data\n%s:",value1->name+1);
        strcat(globalvar_message,name);
        strcat(globalvar_message,"\n\t.long\t");

//        char value_int[12]="0x";
//        float x=value1->pdata->var_pdata.fVal;
//        int xx=*(int*)&x;
//        sprintf(value_int+2,"%0x",xx);
//        这里直接使用IEEE754格式的值就可以，不需要使用16进制
//        之前的使用16进制的逻辑是没有问题的，但是使用十六进制需要明确加上0x
        char value_int[12];
        float x=value1->pdata->var_pdata.fVal;
        int xx=*(int*)&x;
        sprintf(value_int,"%d",xx);
        strcat(globalvar_message,value_int);
        strcat(globalvar_message,"\n");

    }

//    printf("arm_trans_GLOBAL_VAR\n");
    return ins;
}

InstNode *arm_trans_Phi(InstNode *ins){
    assert(false);
//    printf("arm_trans_Phi\n");
    return ins;
}

InstNode *arm_trans_MEMSET(HashMap *hashMap,InstNode *ins){
//    Value*value0=&ins->inst->user.value;
    Value*value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int memset_value=0;
//    Value*value2= user_get_operand_use(&ins->inst->user,1)->Val;
//    printf("arm_trans_MEMSET\n");
    if(isLocalArrayIntType(value1->VTy)|| isLocalArrayFloatType(value1->VTy)|| isGlobalArrayIntType(value1->VTy)||isGlobalArrayFloatType(value1->VTy)){
//        printf("%s\n",value0->alias->name);
        int x=get_value_offset_sp(hashMap,value1);
        if(imm_is_valid(x)){
            printf("\tadd\tr0,r11,#%d\n",x);
            fprintf(fp,"\tadd\tr0,r11,#%d\n",x);
        }else{
            handle_illegal_imm1(2,x);

            printf("\tadd\tr0,r11,r2\n");
            fprintf(fp,"\tadd\tr0,r11,r2\n");
        }

        if(imm_is_valid(memset_value)){
            printf("\tmov\tr1,#%d\n",memset_value);
            fprintf(fp,"\tmov\tr1,#%d\n",memset_value);
        }else{
            handle_illegal_imm1(1,memset_value);

        }
        x= get_array_total_occupy(value1->alias,0);
        if(imm_is_valid(x)){
            printf("\tmov\tr2,#%d\n",x);
            fprintf(fp,"\tmov\tr2,#%d\n",x);
        }else{
            handle_illegal_imm1(2,x);

        }

        printf("\tbl\tmemset\n");

        fprintf(fp,"\tbl\tmemset\n");
    }
    return ins;
}
InstNode * arm_trans_Store(InstNode *ins,HashMap *hashMap){
//    直接寻找value2的alias的pdata的array_pdata 0表示int，1表示float


//    value1是要保存的东西，一般通运算之后保存的，是保存在add等指令的左值，
//    所以是应该在add指令的左值判断该变量是否为全局变量，而不是在store指令里面执行吧
//    不对，在全局变量的add之后，会有一条store %i @a这样的指令

// store这里还是需要处理全局变量和数组的store，因为在数组里面的store和GEP捆绑在一起的话可能是会有点问题的
// 这里买呢先讨论数组的相关处理。store就是将value1 str到value2里面，然后还有就是value2的Vty==AdderssTyID
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
//    value2是要保存到的地方
    Value *value2=user_get_operand_use(&ins->inst->user,1)->Val;
    int left_reg=ins->inst->_reg_[1];//需要存的值，能是变量，也可能是立即数
    int right_reg=ins->inst->_reg_[2];
    assert(value2->alias!=NULL);
//    表示将某个值存放到数组中，这个数组可能是全局数组也可能是局部数组，这个数组给定直接就是绝对地址
    if(value2->VTy->ID==AddressTyID){
//        这个value2->VTy==AddressTyID是不是标识局部数组store的唯一标识这个还需要确认和处理
//        处理局部数组的store，处理完成之后就直接返回就可以了，不需要影响到其他全局变量的处理
        int right_reg_end;
        if(right_reg>100){
            int x= get_value_offset_sp(hashMap,value2);
            handle_illegal_imm(right_reg,x,2);

            right_reg_end=right_reg-100;
        }else{
            right_reg_end=right_reg;
        }
//      这里计算完成之后就是就是right_reg_end里面存放的就是需要保存到的绝对的地址



        int left_int_float=-1;
//        所以说使用的寄存器应该是value1和value2对应的寄存器,这个寄存器的分配结果好像是有点问题的，
//        现在是reg0对应value1

//        还有一种就是将数组的值存回给数组，就是说left是address,right也是address
        if(value1->VTy->ID==AddressTyID){
//            局部数组address存到address,理论上应该不存在这种情况的
//            assert(false);
            assert(value1->alias!=NULL);
            if((is_int_array(value1)&& is_int_array(value2))||(is_float_array(value1)&& is_float_array(value2))){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    printf("\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                }else{
                    printf("\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                }
            }else if(is_int_array(value1)&& is_float_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    int_to_float2(0,left_reg-100,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }else{
                    int_to_float2(0,left_reg,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }
            }else if(is_float_array(value1)&& is_int_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
                    float_to_int2(0,left_reg-100,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }else{
                    float_to_int2(0,left_reg,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }
            }


        }
        else if(isGlobalVarIntType(value1->VTy)){

        } else if(isGlobalVarFloatType(value1->VTy)){

        } else if(isGlobalArrayIntType(value1->VTy)){
            ;
        } else if(isGlobalArrayFloatType(value1->VTy)){
            ;
        } else if(isImmIntType(value1->VTy)&& imm_is_valid(value1->pdata->var_pdata.iVal)){
            printf("\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
            fprintf(fp,"\tmov\tr1,#%d\n",value1->pdata->var_pdata.iVal);
            if(is_float_array(value2)){
//                int_to_float(1,1);
                int_to_float2(0,1,0);
                printf("\tstr\tr0,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
            }else{

                printf("\tstr\tr1,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr1,[r%d]\n",right_reg_end);
            }

        }else if(isImmIntType(value1->VTy)&& !imm_is_valid(value1->pdata->var_pdata.iVal)){
            handle_illegal_imm1(1,value1->pdata->var_pdata.iVal);
            if(is_float_array(value2)){
                int_to_float2(0,1,0);
                printf("\tstr\tr0,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
            }else{
                printf("\tstr\tr1,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr1,[r%d]\n",right_reg_end);
            }

        }else if(isImmFloatType(value1->VTy)){
            float x2=value1->pdata->var_pdata.fVal;
//            printf("%d\n",value1->pdata->var_pdata.iVal);
            int *xx2=(int*)&x2;
            handle_illegal_imm1(1,*xx2);
//            char arr1[12]="0x";
//            sprintf(arr1+2,"%0x",*xx2);
//            printf("\tldr\tr1,=%s\n",arr1);
//            fprintf(fp,"\tldr\tr1,=%s\n",arr1);
            if(is_int_array(value2)){
//                float_to_int(1,1);
                float_to_int2(0,1,0);
                printf("\tstr\tr0,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
            }else{
                printf("\tstr\tr1,[r%d]\n",right_reg_end);
                fprintf(fp,"\tstr\tr1,[r%d]\n",right_reg_end);
            }

        }else if(isLocalVarIntType(value1->VTy)){
            if(is_float_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
//                    int_to_float(left_reg-100,left_reg-100);
                    int_to_float2(0,left_reg-100,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }else{
//                    int_to_float(left_reg,left_reg);
                    int_to_float2(0,left_reg,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }
            }else if(is_int_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                }else{
                    printf("\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                }
            }


        }else if(isLocalVarFloatType(value1->VTy)){
            if(is_float_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);

                    printf("\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg-100,right_reg_end);
                }else{
                    printf("\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                    fprintf(fp,"\tstr\tr%d,[r%d]\n",left_reg,right_reg_end);
                }
            } else if(is_int_array(value2)){
                if(left_reg>100){
                    int x= get_value_offset_sp(hashMap,value1);
                    handle_illegal_imm(left_reg,x,1);
//                    float_to_int(left_reg-100,left_reg-100);
                    float_to_int2(0,left_reg-100,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }else{
//                    float_to_int(left_reg,left_reg);
                    float_to_int2(0,left_reg,0);
                    printf("\tstr\tr0,[r%d]\n",right_reg_end);
                    fprintf(fp,"\tstr\tr0,[r%d]\n",right_reg_end);
                }
            }

        }else if(isLocalArrayIntType(value1->VTy)){

        }else if(isLocalVarFloatType(value1->VTy)){

        }
        return ins;

    }
//  表示将一个值存放到全局变量中
    int left_int_float=-1;
    if(isGlobalVarIntType(value1->VTy)){
//        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
//        if(lcptLabel==NULL){
//            printf("HashMapGet(global_hashmap,value1); error\n");
////            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
//        }
//        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
//        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
        printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        printf("\tldr\tr1,[r1]\n");
        fprintf(fp,"\tldr\tr1,[r1]\n");
        left_int_float=0;
    } else if(isGlobalVarFloatType(value1->VTy)){
//        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value1);
//        if(lcptLabel==NULL){
//            printf("HashMapGet(global_hashmap,value1); error\n");
////            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
//        }
//        printf("\tldr\tr1,%s\n",lcptLabel->LCPI);
//        fprintf(fp,"\tldr\tr1,%s\n",lcptLabel->LCPI);
        printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
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
        int xx=value1->pdata->var_pdata.iVal;
        handle_illegal_imm1(1,xx);

        left_int_float=0;

    }else if(isImmFloatType(value1->VTy)){
//        char arr[12]="0x";
        float x=value1->pdata->var_pdata.fVal;
        int xx=*(int*)&x;
        handle_illegal_imm1(1,xx);
//        sprintf(arr+2,"%0x",xx);
//        printf("\tldr\tr1,=%s\n",arr);
//        fprintf(fp,"\tldr\tr1,=%s\n",arr);
        left_int_float=1;
    }else if(isLocalVarIntType(value1->VTy)){
        left_int_float=0;
        if(left_reg>100){
            int x= get_value_offset_sp(hashMap,value1);
            handle_illegal_imm(left_reg,x,1);

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
            handle_illegal_imm(left_reg,x,1);

            printf("\tmov\tr1,r%d\n",left_reg-100);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg-100);
        }else{
            printf("\tmov\tr1,r%d\n",left_reg);
            fprintf(fp,"\tmov\tr1,r%d\n",left_reg);
        }
    }

//    value2
    if(isGlobalVarIntType(value2->VTy)){
//        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value2);
//        if(lcptLabel==NULL){
//            printf("HashMapGet(global_hashmap,value1); error\n");
////            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
//        }
//        printf("\tldr\tr2,%s\n",lcptLabel->LCPI);
//        fprintf(fp,"\tldr\tr2,%s\n",lcptLabel->LCPI);
        printf("\tmovw\tr2,#:lower16:%s\n",value2->name+1);
        fprintf(fp,"\tmovw\tr2,#:lower16:%s\n",value2->name+1);
        printf("\tmovt\tr2,#:upper16:%s\n",value2->name+1);
        fprintf(fp,"\tmovt\tr2,#:upper16:%s\n",value2->name+1);
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
//        LCPTLabel *lcptLabel=(LCPTLabel*)HashMapGet(global_hashmap,value2);
//        if(lcptLabel==NULL){
//            printf("HashMapGet(global_hashmap,value1); error\n");
////            fprintf(fp,"HashMapGet(global_hashmap,value1); error\n");
//        }
//        printf("\tldr\tr2,%s\n",lcptLabel->LCPI);
//        fprintf(fp,"\tldr\tr2,%s\n",lcptLabel->LCPI);
        printf("\tmovw\tr2,#:lower16:%s\n",value2->name+1);
        fprintf(fp,"\tmovw\tr2,#:lower16:%s\n",value2->name+1);
        printf("\tmovt\tr2,#:upper16:%s\n",value2->name+1);
        fprintf(fp,"\tmovt\tr2,#:upper16:%s\n",value2->name+1);
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
//    load和store是需要修改的，现在的情况就是float arr[3]={1,2,3}这个时候，往该地址里面存的值为int，这样取出来的还是int直接vmov到s1进性
//    计算会导致错误，这里理想的情况是往数组里面存对应类型的数据，类型不符合就需要进行类型转换之后才能回存，load和store都是会有同样的问题。

//在GEP中已经算出了变量的绝对地址，局部变量已经加上r11,全局变量不需要加上r11，所以说这里的load和store就是直接处理就可以了
//还有就是涉及到数组的（包括局部数组和全局数组），value1的类型都是AddressTyID，这里是并不能判断他是局部数组还是全局数组的

    Value *value0=&ins->inst->user.value;
    Value *value1= user_get_operand_use(&ins->inst->user,0)->Val;
    int dest_reg=ins->inst->_reg_[0];
    int dest_reg_abs=abs(dest_reg);
    int left_reg=ins->inst->_reg_[1];
    assert(value1->alias!=NULL);
    if(value1->VTy->ID==AddressTyID){
//        这个是跟store差不多的，处理局部数组的和全局数组load问题
//        所以说这里面再去判断value1的类型是没有什么意义的
//         这里把值存到数组里面，数组有int和float，值本身也有int和float两种类型
//         load的左值为Var_int,Var_float，address三种情况，address的话就先不进行类型转换
        if(is_int_array(value1)){
            if(left_reg>100){
//            这里说明GEP计算好的偏移量被放置如栈内存中
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);


            }else{
                printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);

            }
            if(value0->VTy->ID==Var_INT){
                ;
            }else if(value0->VTy->ID==Var_FLOAT){
                int_to_float(dest_reg_abs,dest_reg_abs);
            }else{ //address
                ;
            }
        }else if(is_float_array(value1)){
            if(left_reg>100){
//            这里说明GEP计算好的偏移量被放置如栈内存中
                int x= get_value_offset_sp(hashMap,value1);
                handle_illegal_imm(left_reg,x,1);

                printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);
                fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);
            }else{
                printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);
                fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);

            }
            if(value0->VTy->ID==Var_INT){
                float_to_int(dest_reg_abs,dest_reg_abs);
            }else if(value0->VTy->ID==Var_FLOAT){
                ;
            }else{ //address
                ;
            }

        }

//        if(left_reg>100){
////            这里说明GEP计算好的偏移量被放置如栈内存中
//            int x= get_value_offset_sp(hashMap,value1);
//            handle_illegal_imm(left_reg,x,1);
//
//            printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);
//            fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg-100);
//
//
//        }else{
//            printf("\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);
//            fprintf(fp,"\tldr\tr%d,[r%d]\n",dest_reg_abs,left_reg);
//
//        }
        if(dest_reg<0){
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

        }
        return ins;
    }

//    处理普通全局变量,处理全局变量的类型问题先不改
    if(isGlobalVarIntType(value1->VTy)){

        printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        if(dest_reg<0){
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

        }else{
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
        }


    } else if(isGlobalVarFloatType(value1->VTy)){

        printf("\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        fprintf(fp,"\tmovw\tr1,#:lower16:%s\n",value1->name+1);
        printf("\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        fprintf(fp,"\tmovt\tr1,#:upper16:%s\n",value1->name+1);
        if(dest_reg<0){
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
            int x= get_value_offset_sp(hashMap,value0);
            handle_illegal_imm(dest_reg_abs,x,0);

        }else{
            printf("\tldr\tr%d,[r1]\n",dest_reg_abs);
            fprintf(fp,"\tldr\tr%d,[r1]\n",dest_reg_abs);
        }
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

//            printf("    store\n");
//            fprintf(fp,"    store\n");
            return ins;
        case Load:
            return arm_trans_Load(ins,hashMap);
//            printf("    load\n");
//            fprintf(fp,"    load\n");
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
            return arm_trans_MEMSET(hashMap,ins);
        case CopyOperation:
            return arm_trans_CopyOperation(ins,hashMap);
        case FunEnd:
            return arm_trans_FuncEnd(ins);
        case sitofp:
            return arm_trans_sitofp(hashMap,ins);
        case fptosi:
            return arm_trans_fptosi(hashMap,ins);
        case SysYMemset:
            return arm_tarns_SysYMemset(hashMap,ins);
        default:
            return ins;
    }
}
