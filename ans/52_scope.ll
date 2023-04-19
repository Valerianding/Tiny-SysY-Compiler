; ModuleID = '52_scope.bc'
source_filename = "52_scope.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = dso_local global i32 7, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func() #0 {
  %1 = load i32, i32* @a, align 4
  %2 = icmp eq i32 1, %1
  br i1 %2, label %3, label %5

3:                                                ; preds = %0
  %4 = add nsw i32 1, 1
  br label %6

5:                                                ; preds = %0
  br label %6

6:                                                ; preds = %5, %3
  %.0 = phi i32 [ 1, %3 ], [ 0, %5 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  br label %1

1:                                                ; preds = %8, %0
  %.01 = phi i32 [ 0, %0 ], [ %.1, %8 ]
  %.0 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %2 = icmp slt i32 %.0, 100
  br i1 %2, label %3, label %10

3:                                                ; preds = %1
  %4 = call i32 @func()
  %5 = icmp eq i32 %4, 1
  br i1 %5, label %6, label %8

6:                                                ; preds = %3
  %7 = add nsw i32 %.01, 1
  br label %8

8:                                                ; preds = %6, %3
  %.1 = phi i32 [ %7, %6 ], [ %.01, %3 ]
  %9 = add nsw i32 %.0, 1
  br label %1

10:                                               ; preds = %1
  %11 = icmp slt i32 %.01, 100
  br i1 %11, label %12, label %14

12:                                               ; preds = %10
  %13 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 1)
  br label %16

14:                                               ; preds = %10
  %15 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 0)
  br label %16

16:                                               ; preds = %14, %12
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
