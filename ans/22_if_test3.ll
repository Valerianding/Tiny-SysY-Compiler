; ModuleID = '22_if_test3.bc'
source_filename = "22_if_test3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @ififElse() #0 {
  %1 = icmp eq i32 5, 5
  Br i1 %1, label %2, label %8

2:                                                ; preds = %0
  %3 = icmp eq i32 10, 10
  Br i1 %3, label %4, label %5

4:                                                ; preds = %2
  Br label %7

5:                                                ; preds = %2
  %6 = add nsw i32 5, 15
  Br label %7

7:                                                ; preds = %5, %4
  %.0 = phi i32 [ 25, %4 ], [ %6, %5 ]
  Br label %8

8:                                                ; preds = %7, %0
  %.1 = phi i32 [ %.0, %7 ], [ 5, %0 ]
  ret i32 %.1
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @ififElse()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
