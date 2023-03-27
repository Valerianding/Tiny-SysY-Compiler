; ModuleID = '33_while_if_test3.bc'
source_filename = "33_while_if_test3.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @deepWhileBr(i32 %0, i32 %1) #0 {
  %3 = add nsw i32 %0, %1
  br label %4

4:                                                ; preds = %15, %2
  %.0 = phi i32 [ %3, %2 ], [ %.2, %15 ]
  %5 = icmp slt i32 %.0, 75
  br i1 %5, label %6, label %16

6:                                                ; preds = %4
  %7 = icmp slt i32 %.0, 100
  br i1 %7, label %8, label %15

8:                                                ; preds = %6
  %9 = add nsw i32 %.0, 42
  %10 = icmp sgt i32 %9, 99
  br i1 %10, label %11, label %14

11:                                               ; preds = %8
  %12 = mul nsw i32 42, 2
  %13 = mul nsw i32 %12, 2
  br label %14

14:                                               ; preds = %11, %8
  %.1 = phi i32 [ %13, %11 ], [ %9, %8 ]
  br label %15

15:                                               ; preds = %14, %6
  %.2 = phi i32 [ %.1, %14 ], [ %.0, %6 ]
  br label %4, !llvm.loop !4

16:                                               ; preds = %4
  ret i32 %.0
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  %1 = call i32 @deepWhileBr(i32 2, i32 2)
  ret i32 %1
}

attributes #0 = { noinline nounwind ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 12, i32 0]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
!4 = distinct !{!4, !5}
!5 = !{!"llvm.loop.mustprogress"}
