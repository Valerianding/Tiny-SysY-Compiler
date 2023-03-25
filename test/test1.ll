define dso_local i32 @whileif() #0{
 br label %1

1:
 %4 = icmp slt i32 %3,%2
 br i1 %4,label %5,label %14

5:
 br label %6

6:
 %9 = icmp sgt i32 %8,2
 br i1 %9,label %10,label %13

10:
 %11= add nsw i32 %8,1
 %12= add nsw i32 %7,1
 br label %6

13:
 br label %1

14:
 ret i32 1
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
 @getch(...) #1
declare dso_local i32 @getarray(...) #1
declare dso_local i32 @getfloat(...) #1
declare dso_local i32 @getfarray(...) #1
declare dso_local i32 @putch(...) #1
declare dso_local i32 @putarray(...) #1
declare dso_local i32 @putfloat(...) #1
declare dso_local i32 @putfarray(...) #1
declare dso_local i32 @putf(...) #1
