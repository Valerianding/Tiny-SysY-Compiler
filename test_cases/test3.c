int b=999;
const int ccc=8;
int test(int c) {
    int a;
    a = ccc;
    if (+-!!!a) {
        a = - - -1;
    }
    else if(a<0 && a<-1){
        a = 0;
    }
    else
        return 2;
    return a;
}

int main()
{
    int k;
    int d[4][2] = {1, 2, {3}, {5}, 7, 8};
    k=d[1][1];
    return test(k);
}

///打印的IR
/*
@b=dso_local global i32 999,align 4
define dso_local i32 @test() #0{
 %2 = alloca i32,align 4
 %3 = alloca i32,align 4
 %4 = alloca i32,align 4
 store i32 %0,i32* %3,align 4
 store i32 8,i32* %4,align 4
 %5 = load i32,i32* %4,align 4
 %6 = icmp ne i32 %5,0
 %7 = xor i1 %6, true
 %8 = zext i1 %7 to i32
 %9= sub nsw i32 0,%8
 %10 = icmp ne i32 %9,0
 br i1 %10,label %11,label %12

11:
 store i32 -1,i32* %4,align 4
 br label %21

12:
 %13 = load i32,i32* %4,align 4
 %14 = icmp slt i32 %13,0
 br i1 %14,label %15,label %19

15:
 %16 = load i32,i32* %4,align 4
 %17 = icmp slt i32 %16,-1
 br i1 %17,label %18,label %19

18:
 store i32 0,i32* %4,align 4
 br label %20

19:
 store i32 2,i32* %2,align 4
 br label %23

20:
 br label %21

21:
 %22 = load i32,i32* %4,align 4
 store i32 %22,i32* %2,align 4
 br label %23

23:
 %24 = load i32,i32* %2,align 4
 ret i32 %24
}

define dso_local i32 @main() #0{
 %1 = alloca i32,align 4
 %2 = alloca [4 x [2 x i32]],align 16
 %3 = alloca i32,align 4
 store i32 0,i32* %1,align 4
 %4=bitcast i32* %2 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %4, i8 0, i64 8, i1 false)
 %5=bitcast i8* %4 to [4 x [2 x i32]]*
 %6=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %5, i32 0,i32 0
 %7=getelementptr inbounds [2 x i32],[2 x i32]* %6, i32 0,i32 0
 store i32 1,i32* %7,align 4
 %8=getelementptr inbounds [2 x i32],[2 x i32]* %6, i32 0,i32 1
 store i32 2,i32* %8,align 4
 %9=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %5, i32 0,i32 1
 %10=getelementptr inbounds [2 x i32],[2 x i32]* %9, i32 0,i32 0
 store i32 3,i32* %10,align 4
 %11=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %5, i32 0,i32 2
 %12=getelementptr inbounds [2 x i32],[2 x i32]* %11, i32 0,i32 0
 store i32 5,i32* %12,align 4
 %13=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %5, i32 0,i32 3
 %14=getelementptr inbounds [2 x i32],[2 x i32]* %13, i32 0,i32 0
 store i32 7,i32* %14,align 4
 %15=getelementptr inbounds [2 x i32],[2 x i32]* %13, i32 0,i32 1
 store i32 8,i32* %15,align 4
 %16=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %2, i32 0,i32 1
 %17=getelementptr inbounds [2 x i32],[2 x i32]* %16, i32 0,i32 1
 %18 = load i32,i32* %17,align 4
 store i32 %18,i32* %3,align 4
 %19 = load i32,i32* %3,align 4
 %20 = call i32 @test(i32 %19)
 ret i32 %20
}
 */

///实际IR
/*
 ALLBegin
 GLOBAL_VAR @b 999
 FunBegin test
 %2 = Alloca NULL                     //保存返回值（是有多个return语句的情况）
 %3 = Alloca NULL                     //参数
 %4 = Alloca a
 store %0, %3                         //将传入参数赋值, TODO 这里%0的处理还需再考虑
 store 8, %4                          //const则直接将变量用值取代
 %5 = load %4
 %6 = NOTEQ %5,0
 %7 = xor %6                          //第二个操作数默认为true，即省略
 %8 = zext %7
 %9 = sub 0, %8
 %10 = NOTEQ %9, 0
 br_i1 %10                            //两个跳转位置在instruction.user.value里，这里就不写了
 LABEL                                //LABEL值11在true_location里
 store -1， %4
 br                                   //跳转位置在instruction.user.value里，这里就不写了
 LABEL
 %13 = load %4
 %14 = LESS %13, 0
 br_i1 %14
 LABEL
 %16 = load %4
 %17 = LESS %16, -1
 br_i1 %17
 LABEL
 store 0, %4
 br
 LABEL
 store 2, %2
 br
 LABEL
 br
 LABEL
 %22 = load %4
 store %22, %2
 br
 LABEL
 %24 = load %2
 Return %24
 FunBegin main
 %1 = Alloca NULL                    //main函数多一个alloca，默认先存0
 %2 = Alloca d
 %3 = Alloca k
 store 0, %1                         //默认存0
 %4 = bitcast %2
 MEMSET %4, 8
 %5 = bitcast %4
 %6 = GMP %5, 0
 %7 = GMP %6, 0
 store 1, %7
 %8 = GMP %6, 1
 store 2, %8
 %9 = GMP %5, 1
 %10 = GMP %9, 0
 store 3, %10
 %11 = GMP %5, 2
 %12 = GMP %11, 0
 store 5, %12
 %13 = GMP %5, 3
 %14 = GMP %13, 0
 store 7, %13
 %15 = GMP %13, 1
 store 8, %15
 %16 = GMP %2, 1
 %17 = GMP %16, 1
 %18 = load %17
 store %18, %3
 %19 = load %3
 GIVE_PARAM %19
 %20 = Call test
 Return %20
 */
