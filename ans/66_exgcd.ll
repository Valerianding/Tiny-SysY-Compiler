; ModuleID = '66_exgcd.bc'
source_filename = "66_exgcd.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.main.x = private unnamed_addr constant [1 x i32] [i32 1], align 4
@__const.main.y = private unnamed_addr constant [1 x i32] [i32 1], align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @exgcd(i32 %0, i32 %1, i32* %2, i32* %3) #0 {
  %5 = icmp eq i32 %1, 0
  Br i1 %5, label %6, label %9

6:                                                ; preds = %4
  %7 = getelementptr inbounds i32, i32* %2, i64 0
  store i32 1, i32* %7, align 4
  %8 = getelementptr inbounds i32, i32* %3, i64 0
  store i32 0, i32* %8, align 4
  Br label %23

9:                                                ; preds = %4
  %10 = srem i32 %0, %1
  %11 = call i32 @exgcd(i32 %1, i32 %10, i32* %2, i32* %3)
  %12 = getelementptr inbounds i32, i32* %2, i64 0
  %13 = load i32, i32* %12, align 4
  %14 = getelementptr inbounds i32, i32* %3, i64 0
  %15 = load i32, i32* %14, align 4
  %16 = getelementptr inbounds i32, i32* %2, i64 0
  store i32 %15, i32* %16, align 4
  %17 = sdiv i32 %0, %1
  %18 = getelementptr inbounds i32, i32* %3, i64 0
  %19 = load i32, i32* %18, align 4
  %20 = mul nsw i32 %17, %19
  %21 = sub nsw i32 %13, %20
  %22 = getelementptr inbounds i32, i32* %3, i64 0
  store i32 %21, i32* %22, align 4
  Br label %23

23:                                               ; preds = %9, %6
  %.0 = phi i32 [ %0, %6 ], [ %11, %9 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [1 x i32], align 4
  %2 = alloca [1 x i32], align 4
  %3 = bitcast [1 x i32]* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %3, i8* align 4 bitcast ([1 x i32]* @__const.main.x to i8*), i64 4, i1 false)
  %4 = bitcast [1 x i32]* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 4 %4, i8* align 4 bitcast ([1 x i32]* @__const.main.y to i8*), i64 4, i1 false)
  %5 = getelementptr inbounds [1 x i32], [1 x i32]* %1, i64 0, i64 0
  %6 = getelementptr inbounds [1 x i32], [1 x i32]* %2, i64 0, i64 0
  %7 = call i32 @exgcd(i32 7, i32 15, i32* %5, i32* %6)
  %8 = getelementptr inbounds [1 x i32], [1 x i32]* %1, i64 0, i64 0
  %9 = load i32, i32* %8, align 4
  %10 = srem i32 %9, 15
  %11 = add nsw i32 %10, 15
  %12 = srem i32 %11, 15
  %13 = getelementptr inbounds [1 x i32], [1 x i32]* %1, i64 0, i64 0
  store i32 %12, i32* %13, align 4
  %14 = getelementptr inbounds [1 x i32], [1 x i32]* %1, i64 0, i64 0
  %15 = load i32, i32* %14, align 4
  %16 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %15)
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

declare dso_local i32 @putint(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
