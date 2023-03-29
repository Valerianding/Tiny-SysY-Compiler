; ModuleID = '99_matrix_tran.bc'
source_filename = "99_matrix_tran.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = common dso_local global i32 0, align 4
@M = common dso_local global i32 0, align 4
@L = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @tran(float* %0, float* %1, float* %2, float* %3, float* %4, float* %5, float* %6, float* %7, float* %8) #0 {
  %10 = getelementptr inbounds float, float* %2, i64 1
  %11 = load float, float* %10, align 4
  %12 = getelementptr inbounds float, float* %7, i64 2
  store float %11, float* %12, align 4
  %13 = getelementptr inbounds float, float* %1, i64 2
  %14 = load float, float* %13, align 4
  %15 = getelementptr inbounds float, float* %8, i64 1
  store float %14, float* %15, align 4
  %16 = getelementptr inbounds float, float* %1, i64 0
  %17 = load float, float* %16, align 4
  %18 = getelementptr inbounds float, float* %6, i64 1
  store float %17, float* %18, align 4
  %19 = getelementptr inbounds float, float* %2, i64 0
  %20 = load float, float* %19, align 4
  %21 = getelementptr inbounds float, float* %6, i64 2
  store float %20, float* %21, align 4
  %22 = getelementptr inbounds float, float* %0, i64 1
  %23 = load float, float* %22, align 4
  %24 = getelementptr inbounds float, float* %7, i64 0
  store float %23, float* %24, align 4
  %25 = getelementptr inbounds float, float* %0, i64 2
  %26 = load float, float* %25, align 4
  %27 = getelementptr inbounds float, float* %8, i64 0
  store float %26, float* %27, align 4
  %28 = getelementptr inbounds float, float* %1, i64 1
  %29 = load float, float* %28, align 4
  %30 = getelementptr inbounds float, float* %7, i64 1
  store float %29, float* %30, align 4
  %31 = getelementptr inbounds float, float* %2, i64 2
  %32 = load float, float* %31, align 4
  %33 = getelementptr inbounds float, float* %8, i64 2
  store float %32, float* %33, align 4
  %34 = getelementptr inbounds float, float* %0, i64 0
  %35 = load float, float* %34, align 4
  %36 = getelementptr inbounds float, float* %6, i64 0
  store float %35, float* %36, align 4
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
  br label %10

10:                                               ; preds = %13, %0
  %.0 = phi i32 [ 0, %0 ], [ %32, %13 ]
  %11 = load i32, i32* @M, align 4
  %12 = icmp slt i32 %.0, %11
  br i1 %12, label %13, label %33

13:                                               ; preds = %10
  %14 = sitofp i32 %.0 to float
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [3 x float], [3 x float]* %1, i64 0, i64 %15
  store float %14, float* %16, align 4
  %17 = sitofp i32 %.0 to float
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds [3 x float], [3 x float]* %2, i64 0, i64 %18
  store float %17, float* %19, align 4
  %20 = sitofp i32 %.0 to float
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [3 x float], [3 x float]* %3, i64 0, i64 %21
  store float %20, float* %22, align 4
  %23 = sitofp i32 %.0 to float
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds [3 x float], [3 x float]* %4, i64 0, i64 %24
  store float %23, float* %25, align 4
  %26 = sitofp i32 %.0 to float
  %27 = sext i32 %.0 to i64
  %28 = getelementptr inbounds [3 x float], [3 x float]* %5, i64 0, i64 %27
  store float %26, float* %28, align 4
  %29 = sitofp i32 %.0 to float
  %30 = sext i32 %.0 to i64
  %31 = getelementptr inbounds [3 x float], [3 x float]* %6, i64 0, i64 %30
  store float %29, float* %31, align 4
  %32 = add nsw i32 %.0, 1
  br label %10

33:                                               ; preds = %10
  %34 = getelementptr inbounds [3 x float], [3 x float]* %1, i64 0, i64 0
  %35 = getelementptr inbounds [3 x float], [3 x float]* %2, i64 0, i64 0
  %36 = getelementptr inbounds [3 x float], [3 x float]* %3, i64 0, i64 0
  %37 = getelementptr inbounds [3 x float], [3 x float]* %4, i64 0, i64 0
  %38 = getelementptr inbounds [3 x float], [3 x float]* %5, i64 0, i64 0
  %39 = getelementptr inbounds [3 x float], [3 x float]* %6, i64 0, i64 0
  %40 = getelementptr inbounds [6 x float], [6 x float]* %7, i64 0, i64 0
  %41 = getelementptr inbounds [3 x float], [3 x float]* %8, i64 0, i64 0
  %42 = getelementptr inbounds [3 x float], [3 x float]* %9, i64 0, i64 0
  %43 = call i32 @tran(float* %34, float* %35, float* %36, float* %37, float* %38, float* %39, float* %40, float* %41, float* %42)
  br label %44

44:                                               ; preds = %47, %33
  %.1 = phi i32 [ %43, %33 ], [ %53, %47 ]
  %45 = load i32, i32* @N, align 4
  %46 = icmp slt i32 %.1, %45
  br i1 %46, label %47, label %54

47:                                               ; preds = %44
  %48 = sext i32 %.1 to i64
  %49 = getelementptr inbounds [6 x float], [6 x float]* %7, i64 0, i64 %48
  %50 = load float, float* %49, align 4
  %51 = fptosi float %50 to i32
  %52 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %51)
  %53 = add nsw i32 %.1, 1
  br label %44

54:                                               ; preds = %44
  %55 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %56

56:                                               ; preds = %59, %54
  %.2 = phi i32 [ 0, %54 ], [ %65, %59 ]
  %57 = load i32, i32* @N, align 4
  %58 = icmp slt i32 %.2, %57
  br i1 %58, label %59, label %66

59:                                               ; preds = %56
  %60 = sext i32 %.2 to i64
  %61 = getelementptr inbounds [3 x float], [3 x float]* %8, i64 0, i64 %60
  %62 = load float, float* %61, align 4
  %63 = fptosi float %62 to i32
  %64 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %63)
  %65 = add nsw i32 %.2, 1
  br label %56

66:                                               ; preds = %56
  %67 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %68

68:                                               ; preds = %71, %66
  %.3 = phi i32 [ 0, %66 ], [ %77, %71 ]
  %69 = load i32, i32* @N, align 4
  %70 = icmp slt i32 %.3, %69
  br i1 %70, label %71, label %78

71:                                               ; preds = %68
  %72 = sext i32 %.3 to i64
  %73 = getelementptr inbounds [3 x float], [3 x float]* %9, i64 0, i64 %72
  %74 = load float, float* %73, align 4
  %75 = fptosi float %74 to i32
  %76 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %75)
  %77 = add nsw i32 %.3, 1
  br label %68

78:                                               ; preds = %68
  %79 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
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
