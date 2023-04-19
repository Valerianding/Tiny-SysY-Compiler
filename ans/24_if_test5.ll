; ModuleID = '24_if_test5.bc'
source_filename = "24_if_test5.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @if_if_Else() #0 {
  %1 = icmp eq i32 5, 5
  br i1 %1, label %2, label %6

2:                                                ; preds = %0
  %3 = icmp eq i32 10, 10
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %4, %2
  %.0 = phi i32 [ 25, %4 ], [ 5, %2 ]
  br label %8

6:                                                ; preds = %0
  %7 = add nsw i32 5, 15
  br label %8

8:                                                ; preds = %6, %5
  %.1 = phi i32 [ %.0, %5 ], [ %7, %6 ]
  ret i32 %.1
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @if_if_Else()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
