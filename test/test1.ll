@arr=dso_local global [10 x [10 x i32]] zeroinitializer, align 4
@a=dso_local global i32 7,align 4
define dso_local i32 @main() #0{
 %1 = call i32 (...) @getint ()
 %2= add nsw i32 %1,%1
 %3= add nsw i32 %2,1
 %4=getelementptr inbounds [10 x [10 x i32]],[10 x [10 x i32]]* @arr, i32 0,i32 %3
 %5=getelementptr inbounds [10 x i32],[10 x i32]* %4, i32 0,i32 %1
 %6 = load i32,i32* %5,align 4
 ret i32 %6
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
