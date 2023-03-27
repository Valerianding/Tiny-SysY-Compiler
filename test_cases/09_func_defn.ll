@a=dso_local global i32 0,align 4
define dso_local i32 @func(i32 %0) #0{
 %2= sub nsw i32 %0,1
 ret i32 %2
}

define dso_local i32 @main() #0{
 store i32 10,i32* @a,align 4
 %1 = load i32,i32* @a,align 4
 %2 = call i32 @func(i32 %1)
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
