; ModuleID = '95_float.bc'
source_filename = "95_float.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@RADIUS = dso_local constant float 5.500000e+00, align 4
@PI = dso_local constant float 0x400921FB60000000, align 4
@EPS = dso_local constant float 0x3EB0C6F7A0000000, align 4
@PI_HEX = dso_local constant float 0x400921FB60000000, align 4
@HEX2 = dso_local constant float 7.812500e-02, align 4
@FACT = dso_local constant float -3.300000e+04, align 4
@EVAL1 = dso_local constant float 0x4057C21FC0000000, align 4
@EVAL2 = dso_local constant float 0x4041475CE0000000, align 4
@EVAL3 = dso_local constant float 0x4041475CE0000000, align 4
@CONV1 = dso_local constant float 2.330000e+02, align 4
@CONV2 = dso_local constant float 4.095000e+03, align 4
@MAX = dso_local constant i32 1000000000, align 4
@TWO = dso_local constant i32 2, align 4
@THREE = dso_local constant i32 3, align 4
@FIVE = dso_local constant i32 5, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local float @float_abs(float %0) #0 {
  %2 = fcmp olt float %0, 0.000000e+00
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = fneg float %0
  br label %6

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %5, %3
  %.0 = phi float [ %4, %3 ], [ %0, %5 ]
  ret float %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local float @circle_area(i32 %0) #0 {
  %2 = sitofp i32 %0 to float
  %3 = fmul float 0x400921FB60000000, %2
  %4 = sitofp i32 %0 to float
  %5 = fmul float %3, %4
  %6 = mul nsw i32 %0, %0
  %7 = sitofp i32 %6 to float
  %8 = fmul float %7, 0x400921FB60000000
  %9 = fadd float %5, %8
  %10 = fdiv float %9, 2.000000e+00
  ret float %10
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @float_eq(float %0, float %1) #0 {
  %3 = fsub float %0, %1
  %4 = call float @float_abs(float %3)
  %5 = fcmp olt float %4, 0x3EB0C6F7A0000000
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %8

7:                                                ; preds = %2
  br label %8

8:                                                ; preds = %7, %6
  %.0 = phi i32 [ 1, %6 ], [ 0, %7 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @error() #0 {
  %1 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 101)
  %2 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 114)
  %3 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 114)
  %4 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 111)
  %5 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 114)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret void
}

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @ok() #0 {
  %1 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 111)
  %2 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 107)
  %3 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @assert(i32 %0) #0 {
  %2 = icmp ne i32 %0, 0
  br i1 %2, label %4, label %3

3:                                                ; preds = %1
  call void @error()
  br label %5

4:                                                ; preds = %1
  call void @ok()
  br label %5

5:                                                ; preds = %4, %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @assert_not(i32 %0) #0 {
  %2 = icmp ne i32 %0, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  call void @error()
  br label %5

4:                                                ; preds = %1
  call void @ok()
  br label %5

5:                                                ; preds = %4, %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x float], align 16
  %2 = call i32 @float_eq(float 7.812500e-02, float -3.300000e+04)
  call void @assert_not(i32 %2)
  %3 = call i32 @float_eq(float 0x4057C21FC0000000, float 0x4041475CE0000000)
  call void @assert_not(i32 %3)
  %4 = call i32 @float_eq(float 0x4041475CE0000000, float 0x4041475CE0000000)
  call void @assert(i32 %4)
  %5 = call float @circle_area(i32 5)
  %6 = call float @circle_area(i32 5)
  %7 = call i32 @float_eq(float %5, float %6)
  call void @assert(i32 %7)
  %8 = call i32 @float_eq(float 2.330000e+02, float 4.095000e+03)
  call void @assert_not(i32 %8)
  br i1 true, label %9, label %10

9:                                                ; preds = %0
  call void @ok()
  br label %10

10:                                               ; preds = %9, %0
  call void @ok()
  call void @ok()
  %11 = bitcast [10 x float]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %11, i8 0, i64 40, i1 false)
  %12 = bitcast i8* %11 to <{ float, float, [8 x float] }>*
  %13 = getelementptr inbounds <{ float, float, [8 x float] }>, <{ float, float, [8 x float] }>* %12, i32 0, i32 0
  store float 1.000000e+00, float* %13, align 16
  %14 = getelementptr inbounds <{ float, float, [8 x float] }>, <{ float, float, [8 x float] }>* %12, i32 0, i32 1
  store float 2.000000e+00, float* %14, align 4
  %15 = getelementptr inbounds [10 x float], [10 x float]* %1, i64 0, i64 0
  %16 = call i32 (float*, ...) bitcast (i32 (...)* @getfarray to i32 (float*, ...)*)(float* %15)
  br label %17

17:                                               ; preds = %19, %10
  %.01 = phi i32 [ 1, %10 ], [ %40, %19 ]
  %.0 = phi i32 [ 0, %10 ], [ %41, %19 ]
  %18 = icmp slt i32 %.01, 1000000000
  br i1 %18, label %19, label %42

19:                                               ; preds = %17
  %20 = call i32 (...) @getfloat()
  %21 = sitofp i32 %20 to float
  %22 = fmul float 0x400921FB60000000, %21
  %23 = fmul float %22, %21
  %24 = fptosi float %21 to i32
  %25 = call float @circle_area(i32 %24)
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds [10 x float], [10 x float]* %1, i64 0, i64 %26
  %28 = load float, float* %27, align 4
  %29 = fadd float %28, %21
  %30 = sext i32 %.0 to i64
  %31 = getelementptr inbounds [10 x float], [10 x float]* %1, i64 0, i64 %30
  store float %29, float* %31, align 4
  %32 = fpext float %23 to double
  %33 = call i32 (double, ...) bitcast (i32 (...)* @putfloat to i32 (double, ...)*)(double %32)
  %34 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %35 = fpext float %25 to double
  %36 = call i32 (double, ...) bitcast (i32 (...)* @putint to i32 (double, ...)*)(double %35)
  %37 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %38 = sitofp i32 %.01 to double
  %39 = fmul double %38, 1.000000e+01
  %40 = fptosi double %39 to i32
  %41 = add nsw i32 %.0, 1
  br label %17

42:                                               ; preds = %17
  %43 = getelementptr inbounds [10 x float], [10 x float]* %1, i64 0, i64 0
  %44 = call i32 (i32, float*, ...) bitcast (i32 (...)* @putfarray to i32 (i32, float*, ...)*)(i32 %16, float* %43)
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

declare dso_local i32 @getfarray(...) #1

declare dso_local i32 @getfloat(...) #1

declare dso_local i32 @putfloat(...) #1

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putfarray(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind willreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
