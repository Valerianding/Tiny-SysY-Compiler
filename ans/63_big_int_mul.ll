; ModuleID = '63_big_int_mul.bc'
source_filename = "63_big_int_mul.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@len = dso_local constant i32 20, align 4
@__const.main.mult1 = private unnamed_addr constant [20 x i32] [i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 0, i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8, i32 9, i32 0], align 16
@__const.main.mult2 = private unnamed_addr constant [20 x i32] [i32 2, i32 3, i32 4, i32 2, i32 5, i32 7, i32 9, i32 9, i32 0, i32 1, i32 9, i32 8, i32 7, i32 6, i32 4, i32 3, i32 2, i32 1, i32 2, i32 2], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [20 x i32], align 16
  %2 = alloca [20 x i32], align 16
  %3 = alloca [25 x i32], align 16
  %4 = alloca [25 x i32], align 16
  %5 = alloca [40 x i32], align 16
  %6 = bitcast [20 x i32]* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %6, i8* align 16 bitcast ([20 x i32]* @__const.main.mult1 to i8*), i64 80, i1 false)
  %7 = bitcast [20 x i32]* %2 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %7, i8* align 16 bitcast ([20 x i32]* @__const.main.mult2 to i8*), i64 80, i1 false)
  %8 = bitcast [40 x i32]* %5 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %8, i8 0, i64 160, i1 false)
  Br label %9

9:                                                ; preds = %11, %0
  %.0 = phi i32 [ 0, %0 ], [ %17, %11 ]
  %10 = icmp slt i32 %.0, 20
  Br i1 %10, label %11, label %18

11:                                               ; preds = %9
  %12 = sext i32 %.0 to i64
  %13 = getelementptr inbounds [20 x i32], [20 x i32]* %1, i64 0, i64 %12
  %14 = load i32, i32* %13, align 4
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [25 x i32], [25 x i32]* %3, i64 0, i64 %15
  store i32 %14, i32* %16, align 4
  %17 = add nsw i32 %.0, 1
  Br label %9

18:                                               ; preds = %9
  Br label %19

19:                                               ; preds = %21, %18
  %.1 = phi i32 [ 0, %18 ], [ %27, %21 ]
  %20 = icmp slt i32 %.1, 20
  Br i1 %20, label %21, label %28

21:                                               ; preds = %19
  %22 = sext i32 %.1 to i64
  %23 = getelementptr inbounds [20 x i32], [20 x i32]* %2, i64 0, i64 %22
  %24 = load i32, i32* %23, align 4
  %25 = sext i32 %.1 to i64
  %26 = getelementptr inbounds [25 x i32], [25 x i32]* %4, i64 0, i64 %25
  store i32 %24, i32* %26, align 4
  %27 = add nsw i32 %.1, 1
  Br label %19

28:                                               ; preds = %19
  %29 = add nsw i32 20, 20
  %30 = sub nsw i32 %29, 1
  Br label %31

31:                                               ; preds = %33, %28
  %.2 = phi i32 [ 0, %28 ], [ %36, %33 ]
  %32 = icmp sle i32 %.2, %30
  Br i1 %32, label %33, label %37

33:                                               ; preds = %31
  %34 = sext i32 %.2 to i64
  %35 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %34
  store i32 0, i32* %35, align 4
  %36 = add nsw i32 %.2, 1
  Br label %31

37:                                               ; preds = %31
  %38 = sub nsw i32 20, 1
  Br label %39

39:                                               ; preds = %76, %37
  %.02 = phi i32 [ %30, %37 ], [ %78, %76 ]
  %.3 = phi i32 [ %38, %37 ], [ %79, %76 ]
  %40 = icmp sgt i32 %.3, -1
  Br i1 %40, label %41, label %80

41:                                               ; preds = %39
  %42 = sext i32 %.3 to i64
  %43 = getelementptr inbounds [25 x i32], [25 x i32]* %4, i64 0, i64 %42
  %44 = load i32, i32* %43, align 4
  %45 = sub nsw i32 20, 1
  Br label %46

46:                                               ; preds = %73, %41
  %.13 = phi i32 [ %.02, %41 ], [ %75, %73 ]
  %.01 = phi i32 [ %45, %41 ], [ %74, %73 ]
  %47 = icmp sgt i32 %.01, -1
  Br i1 %47, label %48, label %76

48:                                               ; preds = %46
  %49 = sext i32 %.13 to i64
  %50 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %49
  %51 = load i32, i32* %50, align 4
  %52 = sext i32 %.01 to i64
  %53 = getelementptr inbounds [25 x i32], [25 x i32]* %3, i64 0, i64 %52
  %54 = load i32, i32* %53, align 4
  %55 = mul nsw i32 %44, %54
  %56 = add nsw i32 %51, %55
  %57 = icmp sge i32 %56, 10
  Br i1 %57, label %58, label %70

58:                                               ; preds = %48
  %59 = sext i32 %.13 to i64
  %60 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %59
  store i32 %56, i32* %60, align 4
  %61 = sub nsw i32 %.13, 1
  %62 = sext i32 %61 to i64
  %63 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %62
  %64 = load i32, i32* %63, align 4
  %65 = sdiv i32 %56, 10
  %66 = add nsw i32 %64, %65
  %67 = sub nsw i32 %.13, 1
  %68 = sext i32 %67 to i64
  %69 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %68
  store i32 %66, i32* %69, align 4
  Br label %73

70:                                               ; preds = %48
  %71 = sext i32 %.13 to i64
  %72 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %71
  store i32 %56, i32* %72, align 4
  Br label %73

73:                                               ; preds = %70, %58
  %74 = sub nsw i32 %.01, 1
  %75 = sub nsw i32 %.13, 1
  Br label %46

76:                                               ; preds = %46
  %77 = add nsw i32 %.13, 20
  %78 = sub nsw i32 %77, 1
  %79 = sub nsw i32 %.3, 1
  Br label %39

80:                                               ; preds = %39
  %81 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 0
  %82 = load i32, i32* %81, align 16
  %83 = icmp ne i32 %82, 0
  Br i1 %83, label %84, label %88

84:                                               ; preds = %80
  %85 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 0
  %86 = load i32, i32* %85, align 16
  %87 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %86)
  Br label %88

88:                                               ; preds = %84, %80
  Br label %89

89:                                               ; preds = %93, %88
  %.4 = phi i32 [ 1, %88 ], [ %98, %93 ]
  %90 = add nsw i32 20, 20
  %91 = sub nsw i32 %90, 1
  %92 = icmp sle i32 %.4, %91
  Br i1 %92, label %93, label %99

93:                                               ; preds = %89
  %94 = sext i32 %.4 to i64
  %95 = getelementptr inbounds [40 x i32], [40 x i32]* %5, i64 0, i64 %94
  %96 = load i32, i32* %95, align 4
  %97 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %96)
  %98 = add nsw i32 %.4, 1
  Br label %89

99:                                               ; preds = %89
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

declare dso_local i32 @putint(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
