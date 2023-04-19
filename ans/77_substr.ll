; ModuleID = '77_substr.bc'
source_filename = "77_substr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@__const.main.A = private unnamed_addr constant [15 x i32] [i32 8, i32 7, i32 4, i32 1, i32 2, i32 7, i32 0, i32 1, i32 9, i32 3, i32 4, i32 8, i32 3, i32 7, i32 0], align 16
@__const.main.B = private unnamed_addr constant [13 x i32] [i32 3, i32 9, i32 7, i32 1, i32 4, i32 2, i32 4, i32 3, i32 6, i32 8, i32 0, i32 1, i32 5], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @MAX(i32 %0, i32 %1) #0 {
  %3 = icmp eq i32 %0, %1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %9

5:                                                ; preds = %2
  %6 = icmp sgt i32 %0, %1
  br i1 %6, label %7, label %8

7:                                                ; preds = %5
  br label %9

8:                                                ; preds = %5
  br label %9

9:                                                ; preds = %8, %7, %4
  %.0 = phi i32 [ %0, %4 ], [ %0, %7 ], [ %1, %8 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @max_sum_nonadjacent(i32* %0, i32 %1) #0 {
  %3 = alloca [16 x i32], align 16
  %4 = bitcast [16 x i32]* %3 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %4, i8 0, i64 64, i1 false)
  %5 = getelementptr inbounds i32, i32* %0, i64 0
  %6 = load i32, i32* %5, align 4
  %7 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 0
  store i32 %6, i32* %7, align 16
  %8 = getelementptr inbounds i32, i32* %0, i64 0
  %9 = load i32, i32* %8, align 4
  %10 = getelementptr inbounds i32, i32* %0, i64 1
  %11 = load i32, i32* %10, align 4
  %12 = call i32 @MAX(i32 %9, i32 %11)
  %13 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 1
  store i32 %12, i32* %13, align 4
  br label %14

14:                                               ; preds = %16, %2
  %.0 = phi i32 [ 2, %2 ], [ %32, %16 ]
  %15 = icmp slt i32 %.0, %1
  br i1 %15, label %16, label %33

16:                                               ; preds = %14
  %17 = sub nsw i32 %.0, 2
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds i32, i32* %0, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = add nsw i32 %20, %23
  %25 = sub nsw i32 %.0, 1
  %26 = sext i32 %25 to i64
  %27 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 %26
  %28 = load i32, i32* %27, align 4
  %29 = call i32 @MAX(i32 %24, i32 %28)
  %30 = sext i32 %.0 to i64
  %31 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 %30
  store i32 %29, i32* %31, align 4
  %32 = add nsw i32 %.0, 1
  br label %14

33:                                               ; preds = %14
  %34 = sub nsw i32 %1, 1
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds [16 x i32], [16 x i32]* %3, i64 0, i64 %35
  %37 = load i32, i32* %36, align 4
  ret i32 %37
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @longest_common_subseq(i32* %0, i32 %1, i32* %2, i32 %3) #0 {
  %5 = alloca [16 x [16 x i32]], align 16
  %6 = bitcast [16 x [16 x i32]]* %5 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %6, i8 0, i64 1024, i1 false)
  br label %7

7:                                                ; preds = %55, %4
  %.01 = phi i32 [ 1, %4 ], [ %56, %55 ]
  %8 = icmp sle i32 %.01, %1
  br i1 %8, label %9, label %57

9:                                                ; preds = %7
  br label %10

10:                                               ; preds = %53, %9
  %.0 = phi i32 [ 1, %9 ], [ %54, %53 ]
  %11 = icmp sle i32 %.0, %3
  br i1 %11, label %12, label %55

12:                                               ; preds = %10
  %13 = sub nsw i32 %.01, 1
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = sub nsw i32 %.0, 1
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds i32, i32* %2, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = icmp eq i32 %16, %20
  br i1 %21, label %22, label %35

22:                                               ; preds = %12
  %23 = sub nsw i32 %.01, 1
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %24
  %26 = sub nsw i32 %.0, 1
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [16 x i32], [16 x i32]* %25, i64 0, i64 %27
  %29 = load i32, i32* %28, align 4
  %30 = add nsw i32 %29, 1
  %31 = sext i32 %.01 to i64
  %32 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %31
  %33 = sext i32 %.0 to i64
  %34 = getelementptr inbounds [16 x i32], [16 x i32]* %32, i64 0, i64 %33
  store i32 %30, i32* %34, align 4
  br label %53

35:                                               ; preds = %12
  %36 = sub nsw i32 %.01, 1
  %37 = sext i32 %36 to i64
  %38 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %37
  %39 = sext i32 %.0 to i64
  %40 = getelementptr inbounds [16 x i32], [16 x i32]* %38, i64 0, i64 %39
  %41 = load i32, i32* %40, align 4
  %42 = sext i32 %.01 to i64
  %43 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %42
  %44 = sub nsw i32 %.0, 1
  %45 = sext i32 %44 to i64
  %46 = getelementptr inbounds [16 x i32], [16 x i32]* %43, i64 0, i64 %45
  %47 = load i32, i32* %46, align 4
  %48 = call i32 @MAX(i32 %41, i32 %47)
  %49 = sext i32 %.01 to i64
  %50 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %49
  %51 = sext i32 %.0 to i64
  %52 = getelementptr inbounds [16 x i32], [16 x i32]* %50, i64 0, i64 %51
  store i32 %48, i32* %52, align 4
  br label %53

53:                                               ; preds = %35, %22
  %54 = add nsw i32 %.0, 1
  br label %10

55:                                               ; preds = %10
  %56 = add nsw i32 %.01, 1
  br label %7

57:                                               ; preds = %7
  %58 = sext i32 %1 to i64
  %59 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* %5, i64 0, i64 %58
  %60 = sext i32 %3 to i64
  %61 = getelementptr inbounds [16 x i32], [16 x i32]* %59, i64 0, i64 %60
  %62 = load i32, i32* %61, align 4
  ret i32 %62
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [15 x i32], align 16
  %2 = alloca [13 x i32], align 16
  %3 = bitcast [15 x i32]* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %3, i8* align 16 bitcast ([15 x i32]* @__const.main.A to i8*), i64 60, i1 false)
  %4 = bitcast [13 x i32]* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %4, i8* align 16 bitcast ([13 x i32]* @__const.main.B to i8*), i64 52, i1 false)
  %5 = getelementptr inbounds [15 x i32], [15 x i32]* %1, i64 0, i64 0
  %6 = call i32 @max_sum_nonadjacent(i32* %5, i32 15)
  %7 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %6)
  %8 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %9 = getelementptr inbounds [15 x i32], [15 x i32]* %1, i64 0, i64 0
  %10 = getelementptr inbounds [13 x i32], [13 x i32]* %2, i64 0, i64 0
  %11 = call i32 @longest_common_subseq(i32* %9, i32 15, i32* %10, i32 13)
  %12 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %11)
  %13 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

declare dso_local i32 @putint(...) #2

declare dso_local i32 @putch(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
