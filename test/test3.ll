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
* %4 to i8*
[4 x [2 x i32]]*
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
 %20 = call i32 @test( ret i32 %20
}

