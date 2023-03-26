; ModuleID = '32_while_if_test2.bc'
source_filename = "32_while_if_test2.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @ifWhile() #0 {
  %1 = icmp eq i32 0, 5
  br i1 %1, label %2, label %9

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %5, %2
  %.01 = phi i32 [ 3, %2 ], [ %6, %5 ]
  %4 = icmp eq i32 %.01, 2
  br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = add nsw i32 %.01, 2
  br label %3, !llvm.loop !4

7:                                                ; preds = %3
  %8 = add nsw i32 %.01, 25
  br label %16

9:                                                ; preds = %0
  br label %10

10:                                               ; preds = %12, %9
  %.1 = phi i32 [ 3, %9 ], [ %13, %12 ]
  %.0 = phi i32 [ 0, %9 ], [ %14, %12 ]
  %11 = icmp slt i32 %.0, 5
  br i1 %11, label %12, label %15

12:                                               ; preds = %10
  %13 = mul nsw i32 %.1, 2
  %14 = add nsw i32 %.0, 1
  br label %10, !llvm.loop !6

15:                                               ; preds = %10
  br label %16

16:                                               ; preds = %15, %7
  %.2 = phi i32 [ %8, %7 ], [ %.1, %15 ]
  ret i32 %.2
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  %1 = call i32 @ifWhile()
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
!6 = distinct !{!6, !5}
