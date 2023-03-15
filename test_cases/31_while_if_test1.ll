; ModuleID = '31_while_if_test1.bc'
source_filename = "31_while_if_test1.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @whileIf() #0 {
  br label %1

1:                                                ; preds = %12, %0
  %.01 = phi i32 [ 0, %0 ], [ %.2, %12 ]
  %.0 = phi i32 [ 0, %0 ], [ %13, %12 ]
  %2 = icmp slt i32 %.0, 100
  br i1 %2, label %3, label %14

3:                                                ; preds = %1
  %4 = icmp eq i32 %.0, 5
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %12

6:                                                ; preds = %3
  %7 = icmp eq i32 %.0, 10
  br i1 %7, label %8, label %9

8:                                                ; preds = %6
  br label %11

9:                                                ; preds = %6
  %10 = mul nsw i32 %.0, 2
  br label %11

11:                                               ; preds = %9, %8
  %.1 = phi i32 [ 42, %8 ], [ %10, %9 ]
  br label %12

12:                                               ; preds = %11, %5
  %.2 = phi i32 [ 25, %5 ], [ %.1, %11 ]
  %13 = add nsw i32 %.0, 1
  br label %1, !llvm.loop !4

14:                                               ; preds = %1
  ret i32 %.01
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  %1 = call i32 @whileIf()
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
