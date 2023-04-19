; ModuleID = '53_scope2.bc'
source_filename = "53_scope2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@k = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  store i32 3389, i32* @k, align 4
  %1 = load i32, i32* @k, align 4
  %2 = icmp slt i32 %1, 10000
  Br i1 %2, label %3, label %18

3:                                                ; preds = %0
  %4 = load i32, i32* @k, align 4
  %5 = add nsw i32 %4, 1
  store i32 %5, i32* @k, align 4
  Br label %6

6:                                                ; preds = %15, %3
  %.0 = phi i32 [ 112, %3 ], [ %.1, %15 ]
  %7 = icmp sgt i32 %.0, 10
  Br i1 %7, label %8, label %16

8:                                                ; preds = %6
  %9 = sub nsw i32 %.0, 88
  %10 = icmp slt i32 %9, 1000
  Br i1 %10, label %11, label %15

11:                                               ; preds = %8
  %12 = sub nsw i32 %9, 10
  %13 = add nsw i32 %12, 11
  %14 = add nsw i32 %13, 11
  Br label %15

15:                                               ; preds = %11, %8
  %.1 = phi i32 [ %14, %11 ], [ %9, %8 ]
  Br label %6

16:                                               ; preds = %6
  %17 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.0)
  Br label %18

18:                                               ; preds = %16, %0
  %19 = load i32, i32* @k, align 4
  ret i32 %19
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
