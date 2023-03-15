@a=dso_local global i32 3,align 4
@b=dso_local global i32 5,align 4
define dso_local i32 @main() #0{
 %1 = alloca i32,align 4
 %2 = alloca i32,align 4
 store i32 0,i32* %1,align 4
 store i32 5,i32* %2,align 4
 %3 = load i32,i32* %2,align 4
 %4 = load i32,i32* @b,align 4
 %5= add nsw i32 %3,%4
 ret i32 %5
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
