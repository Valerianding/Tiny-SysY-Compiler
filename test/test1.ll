define dso_local i32 @main() #0{
 %1 = icmp sgt i32 10,10
 br i1 %1,label %2,label %5

2:
 %3= sub nsw i32 10,1
 %4= add nsw i32 1,1
 br label %8

5:
 %6= add nsw i32 10,1
 %7= sub nsw i32 1,1
 br label %8

8:
 ret i32 %10
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
l %16

16:
 %17 = load i32,i32* %2,align 4
 ret i32 %17
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
