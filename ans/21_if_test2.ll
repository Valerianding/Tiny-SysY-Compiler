; ModuleID = '21_if_test2.bc'
source_filename = "21_if_test2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @ifElseIf() #0 {
  %1 = icmp eq i32 5, 6
  Br i1 %1, label %4, label %2

2:                                                ; preds = %0
  %3 = icmp eq i32 10, 11
  Br i1 %3, label %4, label %5

4:                                                ; preds = %2, %0
  Br label %21

5:                                                ; preds = %2
  %6 = icmp eq i32 10, 10
  Br i1 %6, label %7, label %10

7:                                                ; preds = %5
  %8 = icmp eq i32 5, 1
  Br i1 %8, label %9, label %10

9:                                                ; preds = %7
  Br label %19

10:                                               ; preds = %7, %5
  %11 = icmp eq i32 10, 10
  Br i1 %11, label %12, label %16

12:                                               ; preds = %10
  %13 = icmp eq i32 5, -5
  Br i1 %13, label %14, label %16

14:                                               ; preds = %12
  %15 = add nsw i32 5, 15
  Br label %18

16:                                               ; preds = %12, %10
  %17 = sub nsw i32 0, 5
  Br label %18

18:                                               ; preds = %16, %14
  %.01 = phi i32 [ %15, %14 ], [ %17, %16 ]
  Br label %19

19:                                               ; preds = %18, %9
  %.1 = phi i32 [ 25, %9 ], [ %.01, %18 ]
  Br label %20

20:                                               ; preds = %19
  Br label %21

21:                                               ; preds = %20, %4
  %.0 = phi i32 [ 5, %4 ], [ %.1, %20 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @ifElseIf()
  %2 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %1)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
