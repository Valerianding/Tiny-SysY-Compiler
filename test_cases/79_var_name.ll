; ModuleID = '79_var_name.bc'
source_filename = "79_var_name.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [20 x i32], align 16
  %2 = bitcast [20 x i32]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %2, i8 0, i64 80, i1 false)
  %3 = bitcast i8* %2 to <{ i32, i32, [18 x i32] }>*
  %4 = getelementptr inbounds <{ i32, i32, [18 x i32] }>, <{ i32, i32, [18 x i32] }>* %3, i32 0, i32 0
  store i32 1, i32* %4, align 16
  %5 = getelementptr inbounds <{ i32, i32, [18 x i32] }>, <{ i32, i32, [18 x i32] }>* %3, i32 0, i32 1
  store i32 2, i32* %5, align 4
  br label %6

6:                                                ; preds = %8, %0
  %.01 = phi i32 [ 2, %0 ], [ %33, %8 ]
  %.0 = phi i32 [ 0, %0 ], [ %27, %8 ]
  %7 = icmp slt i32 %.01, 20
  br i1 %7, label %8, label %34

8:                                                ; preds = %6
  %9 = sext i32 %.01 to i64
  %10 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %9
  %11 = load i32, i32* %10, align 4
  %12 = sub nsw i32 %.01, 1
  %13 = sext i32 %12 to i64
  %14 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %13
  %15 = load i32, i32* %14, align 4
  %16 = add nsw i32 %11, %15
  %17 = sub nsw i32 %.01, 2
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = add nsw i32 %16, %20
  %22 = sext i32 %.01 to i64
  %23 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %22
  store i32 %21, i32* %23, align 4
  %24 = sext i32 %.01 to i64
  %25 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = add nsw i32 %.0, %26
  %28 = sext i32 %.01 to i64
  %29 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %28
  %30 = load i32, i32* %29, align 4
  %31 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %30)
  %32 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %33 = add nsw i32 %.01, 1
  br label %6

34:                                               ; preds = %6
  ret i32 %.0
}

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

declare dso_local i32 @putint(...) #2

declare dso_local i32 @putch(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn writeonly }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
