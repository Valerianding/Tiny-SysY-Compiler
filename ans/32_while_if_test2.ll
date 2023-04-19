; ModuleID = '32_while_if_test2.bc'
source_filename = "32_while_if_test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @ifWhile() #0 {
  %1 = icmp eq i32 0, 5
  br i1 %1, label %2, label %9

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %5, %2
  %.01 = phi i32 [ 3, %2 ], [ %6, %5 ]
  %4 = icmp eq i32 %.01, 2
  br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = add nsw i32 %.01, 2
  br label %3

7:                                                ; preds = %3
  %8 = add nsw i32 %.01, 25
  br label %16

9:                                                ; preds = %0
  br label %10

10:                                               ; preds = %12, %9
  %.1 = phi i32 [ 3, %9 ], [ %13, %12 ]
  %.0 = phi i32 [ 0, %9 ], [ %14, %12 ]
  %11 = icmp slt i32 %.0, 5
  br i1 %11, label %12, label %15

12:                                               ; preds = %10
  %13 = mul nsw i32 %.1, 2
  %14 = add nsw i32 %.0, 1
  br label %10

15:                                               ; preds = %10
  br label %16

16:                                               ; preds = %15, %7
  %.2 = phi i32 [ %8, %7 ], [ %.1, %15 ]
  ret i32 %.2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @ifWhile()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
