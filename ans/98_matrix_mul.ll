; ModuleID = '98_matrix_mul.bc'
source_filename = "98_matrix_mul.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@N = dso_local global i32 0, align 4
@M = dso_local global i32 0, align 4
@L = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @mul(float* %0, float* %1, float* %2, float* %3, float* %4, float* %5, float* %6, float* %7, float* %8) #0 {
  %10 = getelementptr inbounds float, float* %0, i64 0
  %11 = load float, float* %10, align 4
  %12 = getelementptr inbounds float, float* %3, i64 0
  %13 = load float, float* %12, align 4
  %14 = fmul float %11, %13
  %15 = getelementptr inbounds float, float* %0, i64 1
  %16 = load float, float* %15, align 4
  %17 = getelementptr inbounds float, float* %4, i64 0
  %18 = load float, float* %17, align 4
  %19 = fmul float %16, %18
  %20 = fadd float %14, %19
  %21 = getelementptr inbounds float, float* %0, i64 2
  %22 = load float, float* %21, align 4
  %23 = getelementptr inbounds float, float* %5, i64 0
  %24 = load float, float* %23, align 4
  %25 = fmul float %22, %24
  %26 = fadd float %20, %25
  %27 = getelementptr inbounds float, float* %6, i64 0
  store float %26, float* %27, align 4
  %28 = getelementptr inbounds float, float* %0, i64 0
  %29 = load float, float* %28, align 4
  %30 = getelementptr inbounds float, float* %3, i64 1
  %31 = load float, float* %30, align 4
  %32 = fmul float %29, %31
  %33 = getelementptr inbounds float, float* %0, i64 1
  %34 = load float, float* %33, align 4
  %35 = getelementptr inbounds float, float* %4, i64 1
  %36 = load float, float* %35, align 4
  %37 = fmul float %34, %36
  %38 = fadd float %32, %37
  %39 = getelementptr inbounds float, float* %0, i64 2
  %40 = load float, float* %39, align 4
  %41 = getelementptr inbounds float, float* %5, i64 1
  %42 = load float, float* %41, align 4
  %43 = fmul float %40, %42
  %44 = fadd float %38, %43
  %45 = getelementptr inbounds float, float* %6, i64 1
  store float %44, float* %45, align 4
  %46 = getelementptr inbounds float, float* %0, i64 0
  %47 = load float, float* %46, align 4
  %48 = getelementptr inbounds float, float* %3, i64 2
  %49 = load float, float* %48, align 4
  %50 = fmul float %47, %49
  %51 = getelementptr inbounds float, float* %0, i64 1
  %52 = load float, float* %51, align 4
  %53 = getelementptr inbounds float, float* %4, i64 2
  %54 = load float, float* %53, align 4
  %55 = fmul float %52, %54
  %56 = fadd float %50, %55
  %57 = getelementptr inbounds float, float* %0, i64 2
  %58 = load float, float* %57, align 4
  %59 = getelementptr inbounds float, float* %5, i64 2
  %60 = load float, float* %59, align 4
  %61 = fmul float %58, %60
  %62 = fadd float %56, %61
  %63 = getelementptr inbounds float, float* %6, i64 2
  store float %62, float* %63, align 4
  %64 = getelementptr inbounds float, float* %1, i64 0
  %65 = load float, float* %64, align 4
  %66 = getelementptr inbounds float, float* %3, i64 0
  %67 = load float, float* %66, align 4
  %68 = fmul float %65, %67
  %69 = getelementptr inbounds float, float* %1, i64 1
  %70 = load float, float* %69, align 4
  %71 = getelementptr inbounds float, float* %4, i64 0
  %72 = load float, float* %71, align 4
  %73 = fmul float %70, %72
  %74 = fadd float %68, %73
  %75 = getelementptr inbounds float, float* %1, i64 2
  %76 = load float, float* %75, align 4
  %77 = getelementptr inbounds float, float* %5, i64 0
  %78 = load float, float* %77, align 4
  %79 = fmul float %76, %78
  %80 = fadd float %74, %79
  %81 = getelementptr inbounds float, float* %7, i64 0
  store float %80, float* %81, align 4
  %82 = getelementptr inbounds float, float* %1, i64 0
  %83 = load float, float* %82, align 4
  %84 = getelementptr inbounds float, float* %3, i64 1
  %85 = load float, float* %84, align 4
  %86 = fmul float %83, %85
  %87 = getelementptr inbounds float, float* %1, i64 1
  %88 = load float, float* %87, align 4
  %89 = getelementptr inbounds float, float* %4, i64 1
  %90 = load float, float* %89, align 4
  %91 = fmul float %88, %90
  %92 = fadd float %86, %91
  %93 = getelementptr inbounds float, float* %1, i64 2
  %94 = load float, float* %93, align 4
  %95 = getelementptr inbounds float, float* %5, i64 1
  %96 = load float, float* %95, align 4
  %97 = fmul float %94, %96
  %98 = fadd float %92, %97
  %99 = getelementptr inbounds float, float* %7, i64 1
  store float %98, float* %99, align 4
  %100 = getelementptr inbounds float, float* %1, i64 0
  %101 = load float, float* %100, align 4
  %102 = getelementptr inbounds float, float* %3, i64 2
  %103 = load float, float* %102, align 4
  %104 = fmul float %101, %103
  %105 = getelementptr inbounds float, float* %1, i64 1
  %106 = load float, float* %105, align 4
  %107 = getelementptr inbounds float, float* %4, i64 2
  %108 = load float, float* %107, align 4
  %109 = fmul float %106, %108
  %110 = fadd float %104, %109
  %111 = getelementptr inbounds float, float* %1, i64 2
  %112 = load float, float* %111, align 4
  %113 = getelementptr inbounds float, float* %5, i64 2
  %114 = load float, float* %113, align 4
  %115 = fmul float %112, %114
  %116 = fadd float %110, %115
  %117 = getelementptr inbounds float, float* %7, i64 2
  store float %116, float* %117, align 4
  %118 = getelementptr inbounds float, float* %2, i64 0
  %119 = load float, float* %118, align 4
  %120 = getelementptr inbounds float, float* %3, i64 0
  %121 = load float, float* %120, align 4
  %122 = fmul float %119, %121
  %123 = getelementptr inbounds float, float* %2, i64 1
  %124 = load float, float* %123, align 4
  %125 = getelementptr inbounds float, float* %4, i64 0
  %126 = load float, float* %125, align 4
  %127 = fmul float %124, %126
  %128 = fadd float %122, %127
  %129 = getelementptr inbounds float, float* %2, i64 2
  %130 = load float, float* %129, align 4
  %131 = getelementptr inbounds float, float* %5, i64 0
  %132 = load float, float* %131, align 4
  %133 = fmul float %130, %132
  %134 = fadd float %128, %133
  %135 = getelementptr inbounds float, float* %8, i64 0
  store float %134, float* %135, align 4
  %136 = getelementptr inbounds float, float* %2, i64 0
  %137 = load float, float* %136, align 4
  %138 = getelementptr inbounds float, float* %3, i64 1
  %139 = load float, float* %138, align 4
  %140 = fmul float %137, %139
  %141 = getelementptr inbounds float, float* %2, i64 1
  %142 = load float, float* %141, align 4
  %143 = getelementptr inbounds float, float* %4, i64 1
  %144 = load float, float* %143, align 4
  %145 = fmul float %142, %144
  %146 = fadd float %140, %145
  %147 = getelementptr inbounds float, float* %2, i64 2
  %148 = load float, float* %147, align 4
  %149 = getelementptr inbounds float, float* %5, i64 1
  %150 = load float, float* %149, align 4
  %151 = fmul float %148, %150
  %152 = fadd float %146, %151
  %153 = getelementptr inbounds float, float* %8, i64 1
  store float %152, float* %153, align 4
  %154 = getelementptr inbounds float, float* %2, i64 0
  %155 = load float, float* %154, align 4
  %156 = getelementptr inbounds float, float* %3, i64 2
  %157 = load float, float* %156, align 4
  %158 = fmul float %155, %157
  %159 = getelementptr inbounds float, float* %2, i64 1
  %160 = load float, float* %159, align 4
  %161 = getelementptr inbounds float, float* %4, i64 2
  %162 = load float, float* %161, align 4
  %163 = fmul float %160, %162
  %164 = fadd float %158, %163
  %165 = getelementptr inbounds float, float* %2, i64 2
  %166 = load float, float* %165, align 4
  %167 = getelementptr inbounds float, float* %5, i64 2
  %168 = load float, float* %167, align 4
  %169 = fmul float %166, %168
  %170 = fadd float %164, %169
  %171 = getelementptr inbounds float, float* %8, i64 2
  store float %170, float* %171, align 4
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
  %43 = call i32 @mul(float* %34, float* %35, float* %36, float* %37, float* %38, float* %39, float* %40, float* %41, float* %42)
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

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
