; ModuleID = '33_while_if_test3.bc'
source_filename = "33_while_if_test3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @deepWhileBr(i32 %0, i32 %1) #0 {
  %3 = add nsw i32 %0, %1
  Br label %4

4:                                                ; preds = %15, %2
  %.0 = phi i32 [ %3, %2 ], [ %.2, %15 ]
  %5 = icmp slt i32 %.0, 75
  Br i1 %5, label %6, label %16

6:                                                ; preds = %4
  %7 = icmp slt i32 %.0, 100
  Br i1 %7, label %8, label %15

8:                                                ; preds = %6
  %9 = add nsw i32 %.0, 42
  %10 = icmp sgt i32 %9, 99
  Br i1 %10, label %11, label %14

11:                                               ; preds = %8
  %12 = mul nsw i32 42, 2
  %13 = mul nsw i32 %12, 2
  Br label %14

14:                                               ; preds = %11, %8
  %.1 = phi i32 [ %13, %11 ], [ %9, %8 ]
  Br label %15

15:                                               ; preds = %14, %6
  %.2 = phi i32 [ %.1, %14 ], [ %.0, %6 ]
  Br label %4

16:                                               ; preds = %4
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @deepWhileBr(i32 2, i32 2)
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
