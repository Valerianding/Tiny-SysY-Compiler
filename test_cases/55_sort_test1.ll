@n=dso_local global i32 0,align 4
define dso_local i32 @bubblesort(i32* %0) #0{
 br label %2

2:
 %3 = phi i32[%11 , %36] %4 = phi i32[%12 , %36] %5 = phi i32[%37 , %36], [0 , %1] %6 = load i32,i32* @n,align 4
 %7= sub nsw i32 %6,1
 %8 = icmp slt i32 %5,%7
 br i1 %8,label %9,label %38

9:
 br label %10

10:
 %11 = phi i32[%35 , %33], [0 , %9] %12 = phi i32[%34 , %33], [%4 , %9] %13 = load i32,i32* @n,align 4
 %14= sub nsw i32 %13,%5
 %15= sub nsw i32 %14,1
 %16 = icmp slt i32 %11,%15
 br i1 %16,label %17,label %36

17:
 %18=getelementptr inbounds i32,i32* %0,i32 %11
 %19 = load i32,i32* %18,align 4
 %20= add nsw i32 %11,1
 %21=getelementptr inbounds i32,i32* %0,i32 %20
 %22 = load i32,i32* %21,align 4
 %23 = icmp sgt i32 %19,%22
 br i1 %23,label %24,label %33

24:
 %25= add nsw i32 %11,1
 %26=getelementptr inbounds i32,i32* %0,i32 %25
 %27 = load i32,i32* %26,align 4
 %28= add nsw i32 %11,1
 %29=getelementptr inbounds i32,i32* %0,i32 %28
 %30=getelementptr inbounds i32,i32* %0,i32 %11
 %31 = load i32,i32* %30,align 4
 store i32 %31,i32* %29,align 4
 %32=getelementptr inbounds i32,i32* %0,i32 %11
 store i32 %27,i32* %32,align 4
 br label %33

33:
 %34 = phi i32[%12 , %17], [%27 , %24] %35= add nsw i32 %11,1
 br label %10

36:
 %37= add nsw i32 %5,1
 br label %2

38:
 ret i32 0
}

define dso_local i32 @main() #0{
 %1 = alloca [10 x i32],align 16
 store i32 10,i32* @n,align 4
 %2=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 0
 store i32 4,i32* %2,align 4
 %3=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 1
 store i32 3,i32* %3,align 4
 %4=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 2
 store i32 9,i32* %4,align 4
 %5=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 3
 store i32 2,i32* %5,align 4
 %6=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 4
 store i32 0,i32* %6,align 4
 %7=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 5
 store i32 1,i32* %7,align 4
 %8=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 6
 store i32 6,i32* %8,align 4
 %9=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 7
 store i32 5,i32* %9,align 4
 %10=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 8
 store i32 7,i32* %10,align 4
 %11=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 9
 store i32 8,i32* %11,align 4
 %12=getelementptr inbounds i32,i32* %1, i32 0,i32 0
 %13 = call i32 @bubblesort(i32 %12)
 br label %14

14:
 %15 = phi i32[%13 , %0], [%24 , %19] %16 = phi i32[10 , %19] %17 = load i32,i32* @n,align 4
 %18 = icmp slt i32 %15,%17
 br i1 %18,label %19,label %25

19:
 %20=getelementptr inbounds [10 x i32],[10 x i32]* %1, i32 0,i32 %15
 %21 = load i32,i32* %20,align 4
 %22 = call i32 (i32,...)bitcast(i32 (...)* @putint to i32(i32,...)*)(i32 %21)
 %23 = call i32 (i32,...)bitcast(i32 (...)* @putch to i32(i32,...)*)(i32 10)
 %24= add nsw i32 %15,1
 br label %14

25:
 ret i32 0
}

declare dso_local i32 @getint(...) #1
declare dso_local i32 @putint(...) #1
declare dso_local i32 @getch(...) #1
declare dso_local i32 @getarray(...) #1
declare dso_local i32 @getfloat(...) #1
declare dso_local i32 @getfarray(...) #1
declare dso_local i32 @putch(...) #1
declare dso_local i32 @putarray(...) #1
declare dso_local i32 @putfloat(...) #1
declare dso_local i32 @putfarray(...) #1
declare dso_local i32 @putf(...) #1