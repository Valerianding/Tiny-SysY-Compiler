; ModuleID = '26_while_test1.bc'
source_filename = "26_while_test1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @doubleWhile() #0 {
  br label %1

1:                                                ; preds = %9, %0
  %.01 = phi i32 [ 7, %0 ], [ %10, %9 ]
  %.0 = phi i32 [ 5, %0 ], [ %4, %9 ]
  %2 = icmp slt i32 %.0, 100
  br i1 %2, label %3, label %11

3:                                                ; preds = %1
  %4 = add nsw i32 %.0, 30
  br label %5

5:                                                ; preds = %7, %3
  %.1 = phi i32 [ %.01, %3 ], [ %8, %7 ]
  %6 = icmp slt i32 %.1, 100
  br i1 %6, label %7, label %9

7:                                                ; preds = %5
  %8 = add nsw i32 %.1, 6
  br label %5

9:                                                ; preds = %5
  %10 = sub nsw i32 %.1, 100
  br label %1

11:                                               ; preds = %1
  ret i32 %.01
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @doubleWhile()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
