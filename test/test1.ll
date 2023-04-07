define dso_local i32 @tes() #0{
 br label %1

1:
 %2 = phi i32[1 , %0], [%7 , %5] %3 = phi i32[10 , %0], [%6 , %5] %4 = icmp slt i32 %3,100
 br i1 %4,label %5,label %8

5:
 %6= add nsw i32 %3,1
 %7= sub nsw i32 %2,1
 br label %1

8:
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
