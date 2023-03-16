; ModuleID = '88_many_params2.bc'
source_filename = "88_many_params2.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func(i32 %0, [59 x i32]* %1, i32 %2, i32* %3, i32 %4, i32 %5, i32* %6, i32 %7, i32 %8) #0 {
  br label %10

10:                                               ; preds = %12, %9
  %.0 = phi i32 [ 0, %9 ], [ %19, %12 ]
  %11 = icmp slt i32 %.0, 10
  br i1 %11, label %12, label %20

12:                                               ; preds = %10
  %13 = sext i32 %0 to i64
  %14 = getelementptr inbounds [59 x i32], [59 x i32]* %1, i64 %13
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [59 x i32], [59 x i32]* %14, i64 0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %17)
  %19 = add nsw i32 %.0, 1
  br label %10

20:                                               ; preds = %10
  %21 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %22 = sext i32 %2 to i64
  %23 = getelementptr inbounds i32, i32* %3, i64 %22
  %24 = load i32, i32* %23, align 4
  %25 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %24)
  %26 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %27

27:                                               ; preds = %29, %20
  %.02 = phi i32 [ %7, %20 ], [ %35, %29 ]
  %.01 = phi i32 [ %8, %20 ], [ %34, %29 ]
  %28 = icmp slt i32 %.01, 10
  br i1 %28, label %29, label %36

29:                                               ; preds = %27
  %30 = mul nsw i32 %.02, 128875
  %31 = srem i32 %30, 3724
  %32 = sext i32 %.01 to i64
  %33 = getelementptr inbounds i32, i32* %6, i64 %32
  store i32 %31, i32* %33, align 4
  %34 = add nsw i32 %.01, 1
  %35 = add nsw i32 %.02, 7
  br label %27

36:                                               ; preds = %27
  %37 = add nsw i32 %4, %5
  ret i32 %37
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [61 x [67 x i32]], align 16
  %2 = alloca [53 x [59 x i32]], align 16
  %3 = bitcast [61 x [67 x i32]]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %3, i8 0, i64 16348, i1 false)
  %4 = bitcast [53 x [59 x i32]]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %4, i8 0, i64 12508, i1 false)
  %5 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %6 = getelementptr inbounds [67 x i32], [67 x i32]* %5, i64 0, i64 1
  store i32 6, i32* %6, align 4
  %7 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %8 = getelementptr inbounds [67 x i32], [67 x i32]* %7, i64 0, i64 3
  store i32 7, i32* %8, align 4
  %9 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %10 = getelementptr inbounds [67 x i32], [67 x i32]* %9, i64 0, i64 4
  store i32 4, i32* %10, align 4
  %11 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %12 = getelementptr inbounds [67 x i32], [67 x i32]* %11, i64 0, i64 7
  store i32 9, i32* %12, align 4
  %13 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %14 = getelementptr inbounds [67 x i32], [67 x i32]* %13, i64 0, i64 11
  store i32 11, i32* %14, align 4
  %15 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %16 = getelementptr inbounds [59 x i32], [59 x i32]* %15, i64 0, i64 1
  store i32 1, i32* %16, align 4
  %17 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %18 = getelementptr inbounds [59 x i32], [59 x i32]* %17, i64 0, i64 2
  store i32 2, i32* %18, align 8
  %19 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %20 = getelementptr inbounds [59 x i32], [59 x i32]* %19, i64 0, i64 3
  store i32 3, i32* %20, align 4
  %21 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %22 = getelementptr inbounds [59 x i32], [59 x i32]* %21, i64 0, i64 9
  store i32 9, i32* %22, align 4
  %23 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %24 = getelementptr inbounds [67 x i32], [67 x i32]* %23, i64 0, i64 1
  %25 = load i32, i32* %24, align 4
  %26 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 0
  %27 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %28 = getelementptr inbounds [67 x i32], [67 x i32]* %27, i64 0, i64 3
  %29 = load i32, i32* %28, align 4
  %30 = getelementptr inbounds [61 x [67 x i32]], [61 x [67 x i32]]* %1, i64 0, i64 17
  %31 = getelementptr inbounds [67 x i32], [67 x i32]* %30, i64 0, i64 0
  %32 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %33 = getelementptr inbounds [59 x i32], [59 x i32]* %32, i64 0, i64 3
  %34 = load i32, i32* %33, align 4
  %35 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %36 = getelementptr inbounds [59 x i32], [59 x i32]* %35, i64 0, i64 0
  %37 = load i32, i32* %36, align 8
  %38 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %39 = getelementptr inbounds [59 x i32], [59 x i32]* %38, i64 0, i64 0
  %40 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 34
  %41 = getelementptr inbounds [59 x i32], [59 x i32]* %40, i64 0, i64 4
  %42 = load i32, i32* %41, align 8
  %43 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 51
  %44 = getelementptr inbounds [59 x i32], [59 x i32]* %43, i64 0, i64 18
  %45 = load i32, i32* %44, align 4
  %46 = call i32 @func(i32 %25, [59 x i32]* %26, i32 %29, i32* %31, i32 %34, i32 %37, i32* %39, i32 %42, i32 %45)
  %47 = mul nsw i32 %46, 3
  br label %48

48:                                               ; preds = %50, %0
  %.0 = phi i32 [ %47, %0 ], [ %57, %50 ]
  %49 = icmp sge i32 %.0, 0
  br i1 %49, label %50, label %58

50:                                               ; preds = %48
  %51 = getelementptr inbounds [53 x [59 x i32]], [53 x [59 x i32]]* %2, i64 0, i64 6
  %52 = sext i32 %.0 to i64
  %53 = getelementptr inbounds [59 x i32], [59 x i32]* %51, i64 0, i64 %52
  %54 = load i32, i32* %53, align 4
  %55 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %54)
  %56 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %57 = sub nsw i32 %.0, 1
  br label %48

58:                                               ; preds = %48
  %59 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind willreturn writeonly }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
