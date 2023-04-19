; ModuleID = '39_op_priority5.bc'
source_filename = "39_op_priority5.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = dso_local global i32 1, align 4
@b = dso_local global i32 0, align 4
@c = dso_local global i32 1, align 4
@d = dso_local global i32 2, align 4
@e = dso_local global i32 4, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = load i32, i32* @a, align 4
  %2 = load i32, i32* @b, align 4
  %3 = mul nsw i32 %1, %2
  %4 = load i32, i32* @c, align 4
  %5 = sdiv i32 %3, %4
  %6 = load i32, i32* @e, align 4
  %7 = load i32, i32* @d, align 4
  %8 = add nsw i32 %6, %7
  %9 = icmp eq i32 %5, %8
  Br i1 %9, label %10, label %22

10:                                               ; preds = %0
  %11 = load i32, i32* @a, align 4
  %12 = load i32, i32* @a, align 4
  %13 = load i32, i32* @b, align 4
  %14 = add nsw i32 %12, %13
  %15 = mul nsw i32 %11, %14
  %16 = load i32, i32* @c, align 4
  %17 = add nsw i32 %15, %16
  %18 = load i32, i32* @d, align 4
  %19 = load i32, i32* @e, align 4
  %20 = add nsw i32 %18, %19
  %21 = icmp sle i32 %17, %20
  Br i1 %21, label %34, label %22

22:                                               ; preds = %10, %0
  %23 = load i32, i32* @a, align 4
  %24 = load i32, i32* @b, align 4
  %25 = load i32, i32* @c, align 4
  %26 = mul nsw i32 %24, %25
  %27 = sub nsw i32 %23, %26
  %28 = load i32, i32* @d, align 4
  %29 = load i32, i32* @a, align 4
  %30 = load i32, i32* @c, align 4
  %31 = sdiv i32 %29, %30
  %32 = sub nsw i32 %28, %31
  %33 = icmp eq i32 %27, %32
  Br i1 %33, label %34, label %35

34:                                               ; preds = %22, %10
  Br label %35

35:                                               ; preds = %34, %22
  %.0 = phi i32 [ 1, %34 ], [ 0, %22 ]
  %36 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.0)
  ret i32 %.0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
