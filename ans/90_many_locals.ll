; ModuleID = '90_many_locals.bc'
source_filename = "90_many_locals.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__const.foo.arr = private unnamed_addr constant [16 x i32] [i32 0, i32 1, i32 2, i32 3, i32 0, i32 1, i32 2, i32 3, i32 0, i32 1, i32 2, i32 3, i32 0, i32 1, i32 2, i32 3], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @foo() #0 {
  %1 = alloca [16 x i32], align 16
  %2 = bitcast [16 x i32]* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %2, i8* align 16 bitcast ([16 x i32]* @__const.foo.arr to i8*), i64 64, i1 false)
  %3 = add nsw i32 3, 7
  %4 = add nsw i32 %3, 5
  %5 = add nsw i32 %4, 6
  %6 = add nsw i32 %5, 1
  %7 = add nsw i32 %6, 0
  %8 = add nsw i32 %7, 3
  %9 = add nsw i32 %8, 5
  %10 = add nsw i32 4, 2
  %11 = add nsw i32 %10, 7
  %12 = add nsw i32 %11, 9
  %13 = add nsw i32 %12, 8
  %14 = add nsw i32 %13, 1
  %15 = add nsw i32 %14, 4
  %16 = add nsw i32 %15, 6
  %17 = add nsw i32 %9, %16
  %18 = sext i32 3 to i64
  %19 = getelementptr inbounds [16 x i32], [16 x i32]* %1, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = add nsw i32 %17, %20
  ret i32 %21
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = add nsw i32 3, 7
  %2 = add nsw i32 %1, 5
  %3 = add nsw i32 %2, 6
  %4 = add nsw i32 %3, 1
  %5 = add nsw i32 %4, 0
  %6 = add nsw i32 %5, 3
  %7 = add nsw i32 %6, 5
  %8 = add nsw i32 4, 2
  %9 = add nsw i32 %8, 7
  %10 = add nsw i32 %9, 9
  %11 = add nsw i32 %10, 8
  %12 = add nsw i32 %11, 1
  %13 = add nsw i32 %12, 4
  %14 = add nsw i32 %13, 6
  %15 = call i32 @foo()
  %16 = add nsw i32 %7, %15
  %17 = call i32 @foo()
  %18 = add nsw i32 %14, %17
  %19 = add nsw i32 4, 7
  %20 = add nsw i32 %19, 2
  %21 = add nsw i32 %20, 5
  %22 = add nsw i32 %21, 8
  %23 = add nsw i32 %22, 0
  %24 = add nsw i32 %23, 6
  %25 = add nsw i32 %24, 3
  %26 = add nsw i32 %16, %18
  %27 = add nsw i32 %26, %25
  %28 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %27)
  %29 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @putint(...) #2

declare dso_local i32 @putch(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
