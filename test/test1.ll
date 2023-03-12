define dso_local i32 @if_if_Else() #0{
 %1 = alloca i32,align 4
 %2 = alloca i32,align 4
 %4 = icmp sgt i32 10,10
 br i1 %4,label %5,label %8

5:
 %7= sub nsw i32 10,(null)
 br label %11

8:
 %10= add nsw i32 10,(null)
 br label %11

11:
 ret i32 %phi
}

define dso_local i32 @main() #0{
 %2 = call i32 @if_if_Else()
 ret i32 %2
}

declare dso_local i32 @getint(...) #1
declare dso_local i32 @putint(...) #1
32* %1,align 4
 br label %11

11:
 %12 = load i32,i32* %1,align 4
 ret i32 %12
}

define dso_local i32 @main() #0{
 %1 = alloca i32,align 4
 store i32 0,i32* %1,align 4
 %2 = call i32 @if_if_Else()
 ret i32 %2
}

declare dso_local i32 @getint(...) #1
declare dso_local i32 @putint(...) #1
