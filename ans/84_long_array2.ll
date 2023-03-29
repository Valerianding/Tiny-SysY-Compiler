; ModuleID = '84_long_array2.bc'
source_filename = "84_long_array2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = common dso_local global [4096 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @f1(i32* %0) #0 {
  store i32 4000, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 5), align 4
  store i32 3, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 4000), align 16
  store i32 7, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 4095), align 4
  %2 = load i32, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 2216), align 16
  %3 = add nsw i32 %2, 9
  %4 = load i32, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 4095), align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds i32, i32* %0, i64 %5
  store i32 %3, i32* %6, align 4
  %7 = load i32, i32* getelementptr inbounds ([4096 x i32], [4096 x i32]* @a, i64 0, i64 5), align 4
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds [4096 x i32], [4096 x i32]* @a, i64 0, i64 %8
  %10 = load i32, i32* %9, align 4
  ret i32 %10
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [4 x [1024 x i32]], align 16
  %2 = alloca [1024 x [4 x i32]], align 16
  %3 = bitcast [4 x [1024 x i32]]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %3, i8 0, i64 16384, i1 false)
  %4 = bitcast i8* %3 to <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>*
  %5 = getelementptr inbounds <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>, <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>* %4, i32 0, i32 1
  %6 = getelementptr inbounds <{ i32, [1023 x i32] }>, <{ i32, [1023 x i32] }>* %5, i32 0, i32 0
  store i32 1, i32* %6, align 16
  %7 = getelementptr inbounds <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>, <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>* %4, i32 0, i32 2
  %8 = getelementptr inbounds <{ i32, i32, [1022 x i32] }>, <{ i32, i32, [1022 x i32] }>* %7, i32 0, i32 0
  store i32 2, i32* %8, align 16
  %9 = getelementptr inbounds <{ i32, i32, [1022 x i32] }>, <{ i32, i32, [1022 x i32] }>* %7, i32 0, i32 1
  store i32 3, i32* %9, align 4
  %10 = getelementptr inbounds <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>, <{ [1024 x i32], <{ i32, [1023 x i32] }>, <{ i32, i32, [1022 x i32] }>, <{ i32, i32, i32, [1021 x i32] }> }>* %4, i32 0, i32 3
  %11 = getelementptr inbounds <{ i32, i32, i32, [1021 x i32] }>, <{ i32, i32, i32, [1021 x i32] }>* %10, i32 0, i32 0
  store i32 4, i32* %11, align 16
  %12 = getelementptr inbounds <{ i32, i32, i32, [1021 x i32] }>, <{ i32, i32, i32, [1021 x i32] }>* %10, i32 0, i32 1
  store i32 5, i32* %12, align 4
  %13 = getelementptr inbounds <{ i32, i32, i32, [1021 x i32] }>, <{ i32, i32, i32, [1021 x i32] }>* %10, i32 0, i32 2
  store i32 6, i32* %13, align 8
  %14 = bitcast [1024 x [4 x i32]]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %14, i8 0, i64 16384, i1 false)
  %15 = bitcast i8* %14 to <{ [4 x i32], [4 x i32], [1022 x [4 x i32]] }>*
  %16 = getelementptr inbounds <{ [4 x i32], [4 x i32], [1022 x [4 x i32]] }>, <{ [4 x i32], [4 x i32], [1022 x [4 x i32]] }>* %15, i32 0, i32 0
  %17 = getelementptr inbounds [4 x i32], [4 x i32]* %16, i32 0, i32 0
  store i32 1, i32* %17, align 16
  %18 = getelementptr inbounds [4 x i32], [4 x i32]* %16, i32 0, i32 1
  store i32 2, i32* %18, align 4
  %19 = getelementptr inbounds <{ [4 x i32], [4 x i32], [1022 x [4 x i32]] }>, <{ [4 x i32], [4 x i32], [1022 x [4 x i32]] }>* %15, i32 0, i32 1
  %20 = getelementptr inbounds [4 x i32], [4 x i32]* %19, i32 0, i32 0
  store i32 3, i32* %20, align 16
  %21 = getelementptr inbounds [4 x i32], [4 x i32]* %19, i32 0, i32 1
  store i32 4, i32* %21, align 4
  %22 = getelementptr inbounds [1024 x [4 x i32]], [1024 x [4 x i32]]* %2, i64 0, i64 0
  %23 = getelementptr inbounds [4 x i32], [4 x i32]* %22, i64 0, i64 0
  %24 = call i32 @f1(i32* %23)
  %25 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %24)
  %26 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %27 = getelementptr inbounds [1024 x [4 x i32]], [1024 x [4 x i32]]* %2, i64 0, i64 2
  %28 = getelementptr inbounds [4 x i32], [4 x i32]* %27, i64 0, i64 0
  %29 = load i32, i32* %28, align 16
  ret i32 %29
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

declare dso_local i32 @putint(...) #2

declare dso_local i32 @putch(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
