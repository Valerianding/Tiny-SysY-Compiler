; ModuleID = '81_skip_spaces.bc'
source_filename = "81_skip_spaces.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [100 x i32], align 16
  Br label %2

2:                                                ; preds = %5, %0
  %.01 = phi i32 [ 0, %0 ], [ %9, %5 ]
  %3 = call i32 (...) @getint()
  %4 = icmp ne i32 %3, 0
  Br i1 %4, label %5, label %10

5:                                                ; preds = %2
  %6 = call i32 (...) @getint()
  %7 = sext i32 %.01 to i64
  %8 = getelementptr inbounds [100 x i32], [100 x i32]* %1, i64 0, i64 %7
  store i32 %6, i32* %8, align 4
  %9 = add nsw i32 %.01, 1
  Br label %2

10:                                               ; preds = %2
  Br label %11

11:                                               ; preds = %13, %10
  %.1 = phi i32 [ %.01, %10 ], [ %14, %13 ]
  %.0 = phi i32 [ 0, %10 ], [ %18, %13 ]
  %12 = icmp ne i32 %.1, 0
  Br i1 %12, label %13, label %19

13:                                               ; preds = %11
  %14 = sub nsw i32 %.1, 1
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds [100 x i32], [100 x i32]* %1, i64 0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = add nsw i32 %.0, %17
  Br label %11

19:                                               ; preds = %11
  %20 = srem i32 %.0, 79
  ret i32 %20
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
