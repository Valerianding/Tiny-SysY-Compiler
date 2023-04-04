; ModuleID = '21_if_test2.bc'
source_filename = "test1.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

@size = common global [10 x i32] zeroinitializer, align 16
@to = common global [10 x [10 x i32]] zeroinitializer, align 16
@cap = common global [10 x [10 x i32]] zeroinitializer, align 16
@rev = common global [10 x [10 x i32]] zeroinitializer, align 16
@used = common global [10 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind ssp uwtable
define i32 @test(i32* %0) #0 {
  %2 = getelementptr inbounds i32, i32* %0, i64 1
  %3 = load i32, i32* %2, align 4
  %4 = add nsw i32 %3, 1
  %5 = getelementptr inbounds i32, i32* %0, i64 1
  store i32 %4, i32* %5, align 4
  %6 = getelementptr inbounds i32, i32* %0, i64 1
  %7 = load i32, i32* %6, align 4
  ret i32 %7
}

; Function Attrs: noinline nounwind ssp uwtable
define i32 @dfs(i32* %0) #0 {
  %2 = getelementptr inbounds i32, i32* %0, i64 1
  %3 = load i32, i32* %2, align 4
  %4 = icmp sgt i32 %3, 1
  br i1 %4, label %5, label %7

5:                                                ; preds = %1
  %6 = call i32 @test(i32* %0)
  br label %10

7:                                                ; preds = %1
  %8 = call i32 @test(i32* %0)
  %9 = add nsw i32 %8, 1
  br label %10

10:                                               ; preds = %7, %5
  ret i32 0
}

attributes #0 = { noinline nounwind ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 12, i32 0]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
