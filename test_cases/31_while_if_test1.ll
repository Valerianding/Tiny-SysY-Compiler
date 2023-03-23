define dso_local i32 @whileIf() #0{
 br label %1

1:
 %4 = icmp slt i32 %3,100
 br i1 %4,label %5,label %18

5:
 %6 = icmp eq i32 %3,5
 br i1 %6,label %7,label %8

7:
 br label %15

8:
 %9 = icmp eq i32 %3,10
 br i1 %9,label %10,label %11

10:
 br label %13

11:
 %12= mul nsw i32 %3,2
 br label %13

13:
 br label %15

15:
 %17= add nsw i32 %3,1
 br label %3

18:
 ret i32 %2
}

define dso_local i32 @main() #0{
 %1 = call i32 @whileIf()
 ret i32 %1
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
* %1,align 4
 %2 = call i32 @whileIf()
 ret i32 %2
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
