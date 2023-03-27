; ModuleID = '28_while_test3.bc'
source_filename = "28_while_test3.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

@e = common global i32 0, align 4
@f = common global i32 0, align 4
@g = common global i32 0, align 4
@h = common global i32 0, align 4

; Function Attrs: noinline nounwind ssp uwtable
define i32 @EightWhile() #0 {
  br label %1

1:                                                ; preds = %57, %0
  %.04 = phi i32 [ 10, %0 ], [ %.15, %57 ]
  %.02 = phi i32 [ 7, %0 ], [ %.13, %57 ]
  %.01 = phi i32 [ 6, %0 ], [ %58, %57 ]
  %.0 = phi i32 [ 5, %0 ], [ %4, %57 ]
  %2 = icmp slt i32 %.0, 20
  br i1 %2, label %3, label %59

3:                                                ; preds = %1
  %4 = add nsw i32 %.0, 3
  br label %5

5:                                                ; preds = %55, %3
  %.15 = phi i32 [ %.04, %3 ], [ %.26, %55 ]
  %.13 = phi i32 [ %.02, %3 ], [ %56, %55 ]
  %.1 = phi i32 [ %.01, %3 ], [ %8, %55 ]
  %6 = icmp slt i32 %.1, 10
  br i1 %6, label %7, label %57

7:                                                ; preds = %5
  %8 = add nsw i32 %.1, 1
  br label %9

9:                                                ; preds = %53, %7
  %.26 = phi i32 [ %.15, %7 ], [ %54, %53 ]
  %.2 = phi i32 [ %.13, %7 ], [ %12, %53 ]
  %10 = icmp eq i32 %.2, 7
  br i1 %10, label %11, label %55

11:                                               ; preds = %9
  %12 = sub nsw i32 %.2, 1
  br label %13

13:                                               ; preds = %50, %11
  %.3 = phi i32 [ %.26, %11 ], [ %16, %50 ]
  %14 = icmp slt i32 %.3, 20
  br i1 %14, label %15, label %53

15:                                               ; preds = %13
  %16 = add nsw i32 %.3, 3
  br label %17

17:                                               ; preds = %47, %15
  %18 = load i32, i32* @e, align 4
  %19 = icmp sgt i32 %18, 1
  br i1 %19, label %20, label %50

20:                                               ; preds = %17
  %21 = load i32, i32* @e, align 4
  %22 = sub nsw i32 %21, 1
  store i32 %22, i32* @e, align 4
  br label %23

23:                                               ; preds = %44, %20
  %24 = load i32, i32* @f, align 4
  %25 = icmp sgt i32 %24, 2
  br i1 %25, label %26, label %47

26:                                               ; preds = %23
  %27 = load i32, i32* @f, align 4
  %28 = sub nsw i32 %27, 2
  store i32 %28, i32* @f, align 4
  br label %29

29:                                               ; preds = %41, %26
  %30 = load i32, i32* @g, align 4
  %31 = icmp slt i32 %30, 3
  br i1 %31, label %32, label %44

32:                                               ; preds = %29
  %33 = load i32, i32* @g, align 4
  %34 = add nsw i32 %33, 10
  store i32 %34, i32* @g, align 4
  br label %35

35:                                               ; preds = %38, %32
  %36 = load i32, i32* @h, align 4
  %37 = icmp slt i32 %36, 10
  br i1 %37, label %38, label %41

38:                                               ; preds = %35
  %39 = load i32, i32* @h, align 4
  %40 = add nsw i32 %39, 8
  store i32 %40, i32* @h, align 4
  br label %35, !llvm.loop !4

41:                                               ; preds = %35
  %42 = load i32, i32* @h, align 4
  %43 = sub nsw i32 %42, 1
  store i32 %43, i32* @h, align 4
  br label %29, !llvm.loop !6

44:                                               ; preds = %29
  %45 = load i32, i32* @g, align 4
  %46 = sub nsw i32 %45, 8
  store i32 %46, i32* @g, align 4
  br label %23, !llvm.loop !7

47:                                               ; preds = %23
  %48 = load i32, i32* @f, align 4
  %49 = add nsw i32 %48, 1
  store i32 %49, i32* @f, align 4
  br label %17, !llvm.loop !8

50:                                               ; preds = %17
  %51 = load i32, i32* @e, align 4
  %52 = add nsw i32 %51, 1
  store i32 %52, i32* @e, align 4
  br label %13, !llvm.loop !9

53:                                               ; preds = %13
  %54 = sub nsw i32 %.3, 1
  br label %9, !llvm.loop !10

55:                                               ; preds = %9
  %56 = add nsw i32 %.2, 1
  br label %5, !llvm.loop !11

57:                                               ; preds = %5
  %58 = sub nsw i32 %.1, 2
  br label %1, !llvm.loop !12

59:                                               ; preds = %1
  %60 = add nsw i32 %.01, %.04
  %61 = add nsw i32 %.0, %60
  %62 = add nsw i32 %61, %.02
  %63 = load i32, i32* @e, align 4
  %64 = add nsw i32 %63, %.04
  %65 = load i32, i32* @g, align 4
  %66 = sub nsw i32 %64, %65
  %67 = load i32, i32* @h, align 4
  %68 = add nsw i32 %66, %67
  %69 = sub nsw i32 %62, %68
  ret i32 %69
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  store i32 1, i32* @g, align 4
  store i32 2, i32* @h, align 4
  store i32 4, i32* @e, align 4
  store i32 6, i32* @f, align 4
  %1 = call i32 @EightWhile()
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
!7 = distinct !{!7, !5}
!8 = distinct !{!8, !5}
!9 = distinct !{!9, !5}
!10 = distinct !{!10, !5}
!11 = distinct !{!11, !5}
!12 = distinct !{!12, !5}
