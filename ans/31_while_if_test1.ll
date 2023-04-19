; ModuleID = '31_while_if_test1.bc'
source_filename = "31_while_if_test1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @whileIf() #0 {
  Br label %1

1:                                                ; preds = %12, %0
  %.01 = phi i32 [ 0, %0 ], [ %.2, %12 ]
  %.0 = phi i32 [ 0, %0 ], [ %13, %12 ]
  %2 = icmp slt i32 %.0, 100
  Br i1 %2, label %3, label %14

3:                                                ; preds = %1
  %4 = icmp eq i32 %.0, 5
  Br i1 %4, label %5, label %6

5:                                                ; preds = %3
  Br label %12

6:                                                ; preds = %3
  %7 = icmp eq i32 %.0, 10
  Br i1 %7, label %8, label %9

8:                                                ; preds = %6
  Br label %11

9:                                                ; preds = %6
  %10 = mul nsw i32 %.0, 2
  Br label %11

11:                                               ; preds = %9, %8
  %.1 = phi i32 [ 42, %8 ], [ %10, %9 ]
  Br label %12

12:                                               ; preds = %11, %5
  %.2 = phi i32 [ 25, %5 ], [ %.1, %11 ]
  %13 = add nsw i32 %.0, 1
  Br label %1

14:                                               ; preds = %1
  ret i32 %.01
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @whileIf()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
