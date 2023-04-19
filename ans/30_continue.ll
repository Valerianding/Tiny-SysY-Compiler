; ModuleID = '30_continue.bc'
source_filename = "30_continue.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  Br label %1

1:                                                ; preds = %7, %5, %0
  %.01 = phi i32 [ 0, %0 ], [ %6, %5 ], [ %9, %7 ]
  %.0 = phi i32 [ 0, %0 ], [ %.0, %5 ], [ %8, %7 ]
  %2 = icmp slt i32 %.01, 100
  Br i1 %2, label %3, label %10

3:                                                ; preds = %1
  %4 = icmp eq i32 %.01, 50
  Br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = add nsw i32 %.01, 1
  Br label %1

7:                                                ; preds = %3
  %8 = add nsw i32 %.0, %.01
  %9 = add nsw i32 %.01, 1
  Br label %1

10:                                               ; preds = %1
  ret i32 %.0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
