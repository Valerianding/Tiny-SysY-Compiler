; ModuleID = '27_while_test2.bc'
source_filename = "27_while_test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @FourWhile() #0 {
  Br label %1

1:                                                ; preds = %21, %0
  %.04 = phi i32 [ 10, %0 ], [ %.15, %21 ]
  %.02 = phi i32 [ 7, %0 ], [ %.13, %21 ]
  %.01 = phi i32 [ 6, %0 ], [ %22, %21 ]
  %.0 = phi i32 [ 5, %0 ], [ %4, %21 ]
  %2 = icmp slt i32 %.0, 20
  Br i1 %2, label %3, label %23

3:                                                ; preds = %1
  %4 = add nsw i32 %.0, 3
  Br label %5

5:                                                ; preds = %19, %3
  %.15 = phi i32 [ %.04, %3 ], [ %.26, %19 ]
  %.13 = phi i32 [ %.02, %3 ], [ %20, %19 ]
  %.1 = phi i32 [ %.01, %3 ], [ %8, %19 ]
  %6 = icmp slt i32 %.1, 10
  Br i1 %6, label %7, label %21

7:                                                ; preds = %5
  %8 = add nsw i32 %.1, 1
  Br label %9

9:                                                ; preds = %17, %7
  %.26 = phi i32 [ %.15, %7 ], [ %18, %17 ]
  %.2 = phi i32 [ %.13, %7 ], [ %12, %17 ]
  %10 = icmp eq i32 %.2, 7
  Br i1 %10, label %11, label %19

11:                                               ; preds = %9
  %12 = sub nsw i32 %.2, 1
  Br label %13

13:                                               ; preds = %15, %11
  %.3 = phi i32 [ %.26, %11 ], [ %16, %15 ]
  %14 = icmp slt i32 %.3, 20
  Br i1 %14, label %15, label %17

15:                                               ; preds = %13
  %16 = add nsw i32 %.3, 3
  Br label %13

17:                                               ; preds = %13
  %18 = sub nsw i32 %.3, 1
  Br label %9

19:                                               ; preds = %9
  %20 = add nsw i32 %.2, 1
  Br label %5

21:                                               ; preds = %5
  %22 = sub nsw i32 %.1, 2
  Br label %1

23:                                               ; preds = %1
  %24 = add nsw i32 %.01, %.04
  %25 = add nsw i32 %.0, %24
  %26 = add nsw i32 %25, %.02
  ret i32 %26
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @FourWhile()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
