define dso_local i32 @ifElseIf() #0{
 %1 = icmp eq i32 5,6
 br i1 %1,label %4,label %2

2:
 %3 = icmp eq i32 10,11
 br i1 %3,label %4,label %5

4:
 br label %23

5:
 %6 = icmp eq i32 10,10
 br i1 %6,label %7,label %10

7:
 %8 = icmp eq i32 5,1
 br i1 %8,label %9,label %10

9:
 br label %20

10:
 %11 = icmp eq i32 10,10
 br i1 %11,label %12,label %16

12:
 %13 = icmp eq i32 5,-5
 br i1 %13,label %14,label %16

14:
 %15= add nsw i32 5,15
 br label %18

16:
 %17= sub nsw i32 0,5
 br label %18

18:
 br label %20

20:
 br label %22

22:
 br label %23

23:
 ret i32 %25
}

define dso_local i32 @main() #0{
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
 label %30

30:
 br label %31

31:
 br label %32

32:
 %33 = load i32,i32* %2,align 4
 store i32 %33,i32* %1,align 4
 br label %34

34:
 %35 = load i32,i32* %1,align 4
 ret i32 %35
}

define dso_local i32 @main() #0{
 %1 = alloca i32,align 4
 store i32 0,i32* %1,align 4
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
