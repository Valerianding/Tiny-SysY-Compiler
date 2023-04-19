; ModuleID = 'test36.bc'
source_filename = "test36.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx12.0.0"

; Function Attrs: noinline nounwind ssp uwtable
define i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = bitcast [10 x i32]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %2, i8 0, i64 40, i1 false)
  %3 = bitcast i8* %2 to [10 x i32]*
  %4 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 0
  store i32 1, i32* %4, align 16
  %5 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 1
  store i32 2, i32* %5, align 4
  %6 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 2
  store i32 3, i32* %6, align 8
  %7 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 3
  store i32 4, i32* %7, align 4
  %8 = getelementptr inbounds [10 x i32], [10 x i32]* %3, i32 0, i32 4
  store i32 5, i32* %8, align 16
  %9 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %10 = load i32, i32* %9, align 16
  %11 = icmp sgt i32 %10, 1
  Br i1 %11, label %12, label %17

12:                                               ; preds = %0
  %13 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %14 = load i32, i32* %13, align 16
  %15 = add nsw i32 %14, 1
  %16 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  store i32 %15, i32* %16, align 16
  Br label %22

17:                                               ; preds = %0
  %18 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %19 = load i32, i32* %18, align 16
  %20 = sub nsw i32 %19, 1
  %21 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  store i32 %20, i32* %21, align 16
  Br label %22

22:                                               ; preds = %17, %12
  %23 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %24 = load i32, i32* %23, align 16
  ret i32 %24
}

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

attributes #0 = { noinline nounwind ssp uwtable "darwin-stkchk-strong-link" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "probe-stack"="___chkstk_darwin" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+cx8,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn writeonly }

!llvm.module.flags = !{!0, !1, !2}
!llvm.ident = !{!3}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 12, i32 0]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{!"Apple clang version 13.0.0 (clang-1300.0.29.3)"}
