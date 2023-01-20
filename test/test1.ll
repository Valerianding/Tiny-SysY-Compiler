define dso_local i32 @if_if_Else() #0{
 %1 = alloca i32,align 4
 %2 = alloca i32,align 4
 store i32 5,i32* %1,align 4
 store i32 10,i32* %2,align 4
 %3 = load i32,i32* %1,align 4
 %4 = icmp eq i32 %3,5
 br i1 %4,label %5,label %10

5:
 %6 = load i32,i32* %2,align 4
 %7 = icmp eq i32 %6,10
 br i1 %7,label %8,label %9

8:
 store i32 25,i32* %1,align 4
 br label %9

9:
 br label %13

10:
 %11 = load i32,i32* %1,align 4
 %12= add nsw i32 %11,15
 store i32 %12,i32* %1,align 4
 br label %13

13:
 %14 = load i32,i32* %1,align 4
 ret i32 %14
}

define dso_local i32 @main() #0{
 %1 = alloca i32,align 4
 %2 = alloca i32,align 4
 %3 = alloca i32,align 4
 store i32 0,i32* %1,align 4
 store i32 2,i32* %2,align 4
 store i32 3,i32* %3,align 4
 br label %4

4:
 %5 = load i32,i32* %2,align 4
 %6= add nsw i32 %5,1
 store i32 %6,i32* %2,align 4
 %7 = load i32,i32* %2,align 4
 %8 = icmp sgt i32 %7,10
 br i1 %8,label %9,label %10

9:
 br label %11

10:
 br label %4

11:
 %12 = load i32,i32* %2,align 4
 store i32 %12,i32* %3,align 4
 %13 = call i32 @if_if_Else( ret i32 %13
}

