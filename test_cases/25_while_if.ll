; ModuleID = '25_while_if.bc'
source_filename = "25_while_if.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @get_one(i32 %0) #0 {
  ret i32 1
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @deepWhileBr(i32 %0, i32 %1) #0 {
  %3 = add nsw i32 %0, %1
  br label %4

4:                                                ; preds = %19, %2
  %.0 = phi i32 [ %3, %2 ], [ %.3, %19 ]
  %5 = icmp slt i32 %.0, 75
  br i1 %5, label %6, label %20

6:                                                ; preds = %4
  %7 = icmp slt i32 %.0, 100
  br i1 %7, label %8, label %19

8:                                                ; preds = %6
  %9 = add nsw i32 %.0, 42
  %10 = icmp sgt i32 %9, 99
  br i1 %10, label %11, label %18

11:                                               ; preds = %8
  %12 = mul nsw i32 42, 2
  %13 = call i32 @get_one(i32 0)
  %14 = icmp eq i32 %13, 1
  br i1 %14, label %15, label %17

15:                                               ; preds = %11
  %16 = mul nsw i32 %12, 2
  br label %17

17:                                               ; preds = %15, %11
  %.1 = phi i32 [ %16, %15 ], [ %9, %11 ]
  br label %18

18:                                               ; preds = %17, %8
  %.2 = phi i32 [ %.1, %17 ], [ %9, %8 ]
  br label %19

19:                                               ; preds = %18, %6
  %.3 = phi i32 [ %.2, %18 ], [ %.0, %6 ]
  br label %4

20:                                               ; preds = %4
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @deepWhileBr(i32 2, i32 2)
  %2 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %1)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
