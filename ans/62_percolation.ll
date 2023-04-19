; ModuleID = '62_percolation.bc'
source_filename = "62_percolation.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@array = common dso_local global [110 x i32] zeroinitializer, align 16
@n = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local void @init(i32 %0) #0 {
  br label %2

2:                                                ; preds = %6, %1
  %.0 = phi i32 [ 1, %1 ], [ %9, %6 ]
  %3 = mul nsw i32 %0, %0
  %4 = add nsw i32 %3, 1
  %5 = icmp sle i32 %.0, %4
  br i1 %5, label %6, label %10

6:                                                ; preds = %2
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %7
  store i32 -1, i32* %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %2

10:                                               ; preds = %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @findfa(i32 %0) #0 {
  %2 = sext i32 %0 to i64
  %3 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %2
  %4 = load i32, i32* %3, align 4
  %5 = icmp eq i32 %4, %0
  br i1 %5, label %6, label %7

6:                                                ; preds = %1
  br label %17

7:                                                ; preds = %1
  %8 = sext i32 %0 to i64
  %9 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %8
  %10 = load i32, i32* %9, align 4
  %11 = call i32 @findfa(i32 %10)
  %12 = sext i32 %0 to i64
  %13 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %12
  store i32 %11, i32* %13, align 4
  %14 = sext i32 %0 to i64
  %15 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %14
  %16 = load i32, i32* %15, align 4
  br label %17

17:                                               ; preds = %7, %6
  %.0 = phi i32 [ %0, %6 ], [ %16, %7 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @mmerge(i32 %0, i32 %1) #0 {
  %3 = call i32 @findfa(i32 %0)
  %4 = call i32 @findfa(i32 %1)
  %5 = icmp ne i32 %3, %4
  br i1 %5, label %6, label %9

6:                                                ; preds = %2
  %7 = sext i32 %3 to i64
  %8 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %7
  store i32 %4, i32* %8, align 4
  br label %9

9:                                                ; preds = %6, %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  br label %1

1:                                                ; preds = %101, %0
  %.01 = phi i32 [ 1, %0 ], [ %4, %101 ]
  %2 = icmp ne i32 %.01, 0
  br i1 %2, label %3, label %102

3:                                                ; preds = %1
  %4 = sub nsw i32 %.01, 1
  store i32 4, i32* @n, align 4
  %5 = load i32, i32* @n, align 4
  call void @init(i32 %5)
  %6 = load i32, i32* @n, align 4
  %7 = load i32, i32* @n, align 4
  %8 = mul nsw i32 %6, %7
  %9 = add nsw i32 %8, 1
  br label %10

10:                                               ; preds = %94, %3
  %.02 = phi i32 [ 0, %3 ], [ %95, %94 ]
  %.0 = phi i32 [ 0, %3 ], [ %.2, %94 ]
  %11 = icmp slt i32 %.02, 10
  br i1 %11, label %12, label %96

12:                                               ; preds = %10
  %13 = call i32 (...) @getint()
  %14 = call i32 (...) @getint()
  %15 = icmp ne i32 %.0, 0
  br i1 %15, label %94, label %16

16:                                               ; preds = %12
  %17 = load i32, i32* @n, align 4
  %18 = sub nsw i32 %13, 1
  %19 = mul nsw i32 %17, %18
  %20 = add nsw i32 %19, %14
  %21 = sext i32 %20 to i64
  %22 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %21
  store i32 %20, i32* %22, align 4
  %23 = icmp eq i32 %13, 1
  br i1 %23, label %24, label %25

24:                                               ; preds = %16
  store i32 0, i32* getelementptr inbounds ([110 x i32], [110 x i32]* @array, i64 0, i64 0), align 16
  call void @mmerge(i32 %20, i32 0)
  br label %25

25:                                               ; preds = %24, %16
  %26 = load i32, i32* @n, align 4
  %27 = icmp eq i32 %13, %26
  br i1 %27, label %28, label %31

28:                                               ; preds = %25
  %29 = sext i32 %9 to i64
  %30 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %29
  store i32 %9, i32* %30, align 4
  call void @mmerge(i32 %20, i32 %9)
  br label %31

31:                                               ; preds = %28, %25
  %32 = load i32, i32* @n, align 4
  %33 = icmp slt i32 %14, %32
  br i1 %33, label %34, label %42

34:                                               ; preds = %31
  %35 = add nsw i32 %20, 1
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %36
  %38 = load i32, i32* %37, align 4
  %39 = icmp ne i32 %38, -1
  br i1 %39, label %40, label %42

40:                                               ; preds = %34
  %41 = add nsw i32 %20, 1
  call void @mmerge(i32 %20, i32 %41)
  br label %42

42:                                               ; preds = %40, %34, %31
  %43 = icmp sgt i32 %14, 1
  br i1 %43, label %44, label %52

44:                                               ; preds = %42
  %45 = sub nsw i32 %20, 1
  %46 = sext i32 %45 to i64
  %47 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %46
  %48 = load i32, i32* %47, align 4
  %49 = icmp ne i32 %48, -1
  br i1 %49, label %50, label %52

50:                                               ; preds = %44
  %51 = sub nsw i32 %20, 1
  call void @mmerge(i32 %20, i32 %51)
  br label %52

52:                                               ; preds = %50, %44, %42
  %53 = load i32, i32* @n, align 4
  %54 = icmp slt i32 %13, %53
  br i1 %54, label %55, label %65

55:                                               ; preds = %52
  %56 = load i32, i32* @n, align 4
  %57 = add nsw i32 %20, %56
  %58 = sext i32 %57 to i64
  %59 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %58
  %60 = load i32, i32* %59, align 4
  %61 = icmp ne i32 %60, -1
  br i1 %61, label %62, label %65

62:                                               ; preds = %55
  %63 = load i32, i32* @n, align 4
  %64 = add nsw i32 %20, %63
  call void @mmerge(i32 %20, i32 %64)
  br label %65

65:                                               ; preds = %62, %55, %52
  %66 = icmp sgt i32 %13, 1
  br i1 %66, label %67, label %77

67:                                               ; preds = %65
  %68 = load i32, i32* @n, align 4
  %69 = sub nsw i32 %20, %68
  %70 = sext i32 %69 to i64
  %71 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %70
  %72 = load i32, i32* %71, align 4
  %73 = icmp ne i32 %72, -1
  br i1 %73, label %74, label %77

74:                                               ; preds = %67
  %75 = load i32, i32* @n, align 4
  %76 = sub nsw i32 %20, %75
  call void @mmerge(i32 %20, i32 %76)
  br label %77

77:                                               ; preds = %74, %67, %65
  %78 = load i32, i32* getelementptr inbounds ([110 x i32], [110 x i32]* @array, i64 0, i64 0), align 16
  %79 = icmp ne i32 %78, -1
  br i1 %79, label %80, label %93

80:                                               ; preds = %77
  %81 = sext i32 %9 to i64
  %82 = getelementptr inbounds [110 x i32], [110 x i32]* @array, i64 0, i64 %81
  %83 = load i32, i32* %82, align 4
  %84 = icmp ne i32 %83, -1
  br i1 %84, label %85, label %93

85:                                               ; preds = %80
  %86 = call i32 @findfa(i32 0)
  %87 = call i32 @findfa(i32 %9)
  %88 = icmp eq i32 %86, %87
  br i1 %88, label %89, label %93

89:                                               ; preds = %85
  %90 = add nsw i32 %.02, 1
  %91 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %90)
  %92 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %93

93:                                               ; preds = %89, %85, %80, %77
  %.1 = phi i32 [ 1, %89 ], [ %.0, %85 ], [ %.0, %80 ], [ %.0, %77 ]
  br label %94

94:                                               ; preds = %93, %12
  %.2 = phi i32 [ %.0, %12 ], [ %.1, %93 ]
  %95 = add nsw i32 %.02, 1
  br label %10

96:                                               ; preds = %10
  %97 = icmp ne i32 %.0, 0
  br i1 %97, label %101, label %98

98:                                               ; preds = %96
  %99 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 -1)
  %100 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %101

101:                                              ; preds = %98, %96
  br label %1

102:                                              ; preds = %1
  ret i32 0
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
