; ModuleID = '21_if_test2.bc'
source_filename = "21_if_test2.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @ifElseIf() #0 {
  %1 = icmp eq i32 5, 6
  br i1 %1, label %4, label %2

2:                                                ; preds = %0
  %3 = icmp eq i32 10, 11
  br i1 %3, label %4, label %5

4:                                                ; preds = %2, %0
  br label %21

5:                                                ; preds = %2
  %6 = icmp eq i32 10, 10
  br i1 %6, label %7, label %10

7:                                                ; preds = %5
  %8 = icmp eq i32 5, 1
  br i1 %8, label %9, label %10

9:                                                ; preds = %7
  br label %19

10:                                               ; preds = %7, %5
  %11 = icmp eq i32 10, 10
  br i1 %11, label %12, label %16

12:                                               ; preds = %10
  %13 = icmp eq i32 5, -5
  br i1 %13, label %14, label %16

14:                                               ; preds = %12
  %15 = add nsw i32 5, 15
  br label %18

16:                                               ; preds = %12, %10
  %17 = sub nsw i32 0, 5
  br label %18

18:                                               ; preds = %16, %14
  %.01 = phi i32 [ %15, %14 ], [ %17, %16 ]
  br label %19

19:                                               ; preds = %18, %9
  %.1 = phi i32 [ 25, %9 ], [ %.01, %18 ]
  br label %20

20:                                               ; preds = %19
  br label %21

21:                                               ; preds = %20, %4
  %.0 = phi i32 [ 5, %4 ], [ %.1, %20 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  ret i32 0
}

attributes #0 = { noinline nounwind ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 12, i32 0]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
