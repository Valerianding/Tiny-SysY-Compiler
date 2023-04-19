; ModuleID = '97_matrix_sub.bc'
source_filename = "97_matrix_sub.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = common dso_local global i32 0, align 4
@M = common dso_local global i32 0, align 4
@L = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @sub(float* %0, float* %1, float* %2, float* %3, float* %4, float* %5, float* %6, float* %7, float* %8) #0 {
  Br label %10

10:                                               ; preds = %12, %9
  %.0 = phi i32 [ 0, %9 ], [ %40, %12 ]
  %11 = icmp slt i32 %.0, 3
  Br i1 %11, label %12, label %41

12:                                               ; preds = %10
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds float, float* %0, i64 %13
  %15 = load float, float* %14, align 4
  %16 = sext i32 %.0 to i64
  %17 = getelementptr inbounds float, float* %3, i64 %16
  %18 = load float, float* %17, align 4
  %19 = fsub float %15, %18
  %20 = sext i32 %.0 to i64
  %21 = getelementptr inbounds float, float* %6, i64 %20
  store float %19, float* %21, align 4
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds float, float* %1, i64 %22
  %24 = load float, float* %23, align 4
  %25 = sext i32 %.0 to i64
  %26 = getelementptr inbounds float, float* %4, i64 %25
  %27 = load float, float* %26, align 4
  %28 = fsub float %24, %27
  %29 = sext i32 %.0 to i64
  %30 = getelementptr inbounds float, float* %7, i64 %29
  store float %28, float* %30, align 4
  %31 = sext i32 %.0 to i64
  %32 = getelementptr inbounds float, float* %2, i64 %31
  %33 = load float, float* %32, align 4
  %34 = sext i32 %.0 to i64
  %35 = getelementptr inbounds float, float* %5, i64 %34
  %36 = load float, float* %35, align 4
  %37 = fsub float %33, %36
  %38 = sext i32 %.0 to i64
  %39 = getelementptr inbounds float, float* %8, i64 %38
  store float %37, float* %39, align 4
  %40 = add nsw i32 %.0, 1
  Br label %10

41:                                               ; preds = %10
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [3 x float], align 4
  %2 = alloca [3 x float], align 4
  %3 = alloca [3 x float], align 4
  %4 = alloca [3 x float], align 4
  %5 = alloca [3 x float], align 4
  %6 = alloca [3 x float], align 4
  %7 = alloca [6 x float], align 16
  %8 = alloca [3 x float], align 4
  %9 = alloca [3 x float], align 4
  store i32 3, i32* @N, align 4
  store i32 3, i32* @M, align 4
  store i32 3, i32* @L, align 4
  Br label %10

10:                                               ; preds = %12, %0
  %.0 = phi i32 [ 0, %0 ], [ %31, %12 ]
  %11 = icmp slt i32 %.0, 3
  Br i1 %11, label %12, label %32

12:                                               ; preds = %10
  %13 = sitofp i32 %.0 to float
  %14 = sext i32 %.0 to i64
  %15 = getelementptr inbounds [3 x float], [3 x float]* %1, i64 0, i64 %14
  store float %13, float* %15, align 4
  %16 = sitofp i32 %.0 to float
  %17 = sext i32 %.0 to i64
  %18 = getelementptr inbounds [3 x float], [3 x float]* %2, i64 0, i64 %17
  store float %16, float* %18, align 4
  %19 = sitofp i32 %.0 to float
  %20 = sext i32 %.0 to i64
  %21 = getelementptr inbounds [3 x float], [3 x float]* %3, i64 0, i64 %20
  store float %19, float* %21, align 4
  %22 = sitofp i32 %.0 to float
  %23 = sext i32 %.0 to i64
  %24 = getelementptr inbounds [3 x float], [3 x float]* %4, i64 0, i64 %23
  store float %22, float* %24, align 4
  %25 = sitofp i32 %.0 to float
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds [3 x float], [3 x float]* %5, i64 0, i64 %26
  store float %25, float* %27, align 4
  %28 = sitofp i32 %.0 to float
  %29 = sext i32 %.0 to i64
  %30 = getelementptr inbounds [3 x float], [3 x float]* %6, i64 0, i64 %29
  store float %28, float* %30, align 4
  %31 = add nsw i32 %.0, 1
  Br label %10

32:                                               ; preds = %10
  %33 = getelementptr inbounds [3 x float], [3 x float]* %1, i64 0, i64 0
  %34 = getelementptr inbounds [3 x float], [3 x float]* %2, i64 0, i64 0
  %35 = getelementptr inbounds [3 x float], [3 x float]* %3, i64 0, i64 0
  %36 = getelementptr inbounds [3 x float], [3 x float]* %4, i64 0, i64 0
  %37 = getelementptr inbounds [3 x float], [3 x float]* %5, i64 0, i64 0
  %38 = getelementptr inbounds [3 x float], [3 x float]* %6, i64 0, i64 0
  %39 = getelementptr inbounds [6 x float], [6 x float]* %7, i64 0, i64 0
  %40 = getelementptr inbounds [3 x float], [3 x float]* %8, i64 0, i64 0
  %41 = getelementptr inbounds [3 x float], [3 x float]* %9, i64 0, i64 0
  %42 = call i32 @sub(float* %33, float* %34, float* %35, float* %36, float* %37, float* %38, float* %39, float* %40, float* %41)
  Br label %43

43:                                               ; preds = %45, %32
  %.1 = phi i32 [ %42, %32 ], [ %51, %45 ]
  %44 = icmp slt i32 %.1, 3
  Br i1 %44, label %45, label %52

45:                                               ; preds = %43
  %46 = sext i32 %.1 to i64
  %47 = getelementptr inbounds [6 x float], [6 x float]* %7, i64 0, i64 %46
  %48 = load float, float* %47, align 4
  %49 = fptosi float %48 to i32
  %50 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %49)
  %51 = add nsw i32 %.1, 1
  Br label %43

52:                                               ; preds = %43
  %53 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  Br label %54

54:                                               ; preds = %56, %52
  %.2 = phi i32 [ 0, %52 ], [ %62, %56 ]
  %55 = icmp slt i32 %.2, 3
  Br i1 %55, label %56, label %63

56:                                               ; preds = %54
  %57 = sext i32 %.2 to i64
  %58 = getelementptr inbounds [3 x float], [3 x float]* %8, i64 0, i64 %57
  %59 = load float, float* %58, align 4
  %60 = fptosi float %59 to i32
  %61 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %60)
  %62 = add nsw i32 %.2, 1
  Br label %54

63:                                               ; preds = %54
  %64 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  Br label %65

65:                                               ; preds = %67, %63
  %.3 = phi i32 [ 0, %63 ], [ %73, %67 ]
  %66 = icmp slt i32 %.3, 3
  Br i1 %66, label %67, label %74

67:                                               ; preds = %65
  %68 = sext i32 %.3 to i64
  %69 = getelementptr inbounds [3 x float], [3 x float]* %9, i64 0, i64 %68
  %70 = load float, float* %69, align 4
  %71 = fptosi float %70 to i32
  %72 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %71)
  %73 = add nsw i32 %.3, 1
  Br label %65

74:                                               ; preds = %65
  %75 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
