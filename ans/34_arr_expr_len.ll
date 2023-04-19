; ModuleID = '34_arr_expr_len.bc'
source_filename = "34_arr_expr_len.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 -1, align 4
@arr = dso_local global [6 x i32] [i32 1, i32 2, i32 33, i32 4, i32 5, i32 6], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  Br label %1

1:                                                ; preds = %3, %0
  %.01 = phi i32 [ 0, %0 ], [ %8, %3 ]
  %.0 = phi i32 [ 0, %0 ], [ %7, %3 ]
  %2 = icmp slt i32 %.01, 6
  Br i1 %2, label %3, label %9

3:                                                ; preds = %1
  %4 = sext i32 %.01 to i64
  %5 = getelementptr inbounds [6 x i32], [6 x i32]* @arr, i64 0, i64 %4
  %6 = load i32, i32* %5, align 4
  %7 = add nsw i32 %.0, %6
  %8 = add nsw i32 %.01, 1
  Br label %1

9:                                                ; preds = %1
  ret i32 %.0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
