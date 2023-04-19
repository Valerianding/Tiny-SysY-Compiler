; ModuleID = '57_sort_test3.bc'
source_filename = "57_sort_test3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@n = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @QuickSort(i32* %0, i32 %1, i32 %2) #0 {
  %4 = icmp slt i32 %1, %2
  br i1 %4, label %5, label %62

5:                                                ; preds = %3
  %6 = sext i32 %1 to i64
  %7 = getelementptr inbounds i32, i32* %0, i64 %6
  %8 = load i32, i32* %7, align 4
  br label %9

9:                                                ; preds = %54, %5
  %.01 = phi i32 [ %1, %5 ], [ %.23, %54 ]
  %.0 = phi i32 [ %2, %5 ], [ %.2, %54 ]
  %10 = icmp slt i32 %.01, %.0
  br i1 %10, label %11, label %55

11:                                               ; preds = %9
  br label %12

12:                                               ; preds = %22, %11
  %.1 = phi i32 [ %.0, %11 ], [ %23, %22 ]
  %13 = icmp slt i32 %.01, %.1
  br i1 %13, label %14, label %20

14:                                               ; preds = %12
  %15 = sext i32 %.1 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = sub nsw i32 %8, 1
  %19 = icmp sgt i32 %17, %18
  br label %20

20:                                               ; preds = %14, %12
  %21 = phi i1 [ false, %12 ], [ %19, %14 ]
  br i1 %21, label %22, label %24

22:                                               ; preds = %20
  %23 = sub nsw i32 %.1, 1
  br label %12

24:                                               ; preds = %20
  %25 = icmp slt i32 %.01, %.1
  br i1 %25, label %26, label %33

26:                                               ; preds = %24
  %27 = sext i32 %.1 to i64
  %28 = getelementptr inbounds i32, i32* %0, i64 %27
  %29 = load i32, i32* %28, align 4
  %30 = sext i32 %.01 to i64
  %31 = getelementptr inbounds i32, i32* %0, i64 %30
  store i32 %29, i32* %31, align 4
  %32 = add nsw i32 %.01, 1
  br label %33

33:                                               ; preds = %26, %24
  %.12 = phi i32 [ %32, %26 ], [ %.01, %24 ]
  br label %34

34:                                               ; preds = %43, %33
  %.23 = phi i32 [ %.12, %33 ], [ %44, %43 ]
  %35 = icmp slt i32 %.23, %.1
  br i1 %35, label %36, label %41

36:                                               ; preds = %34
  %37 = sext i32 %.23 to i64
  %38 = getelementptr inbounds i32, i32* %0, i64 %37
  %39 = load i32, i32* %38, align 4
  %40 = icmp slt i32 %39, %8
  br label %41

41:                                               ; preds = %36, %34
  %42 = phi i1 [ false, %34 ], [ %40, %36 ]
  br i1 %42, label %43, label %45

43:                                               ; preds = %41
  %44 = add nsw i32 %.23, 1
  br label %34

45:                                               ; preds = %41
  %46 = icmp slt i32 %.23, %.1
  br i1 %46, label %47, label %54

47:                                               ; preds = %45
  %48 = sext i32 %.23 to i64
  %49 = getelementptr inbounds i32, i32* %0, i64 %48
  %50 = load i32, i32* %49, align 4
  %51 = sext i32 %.1 to i64
  %52 = getelementptr inbounds i32, i32* %0, i64 %51
  store i32 %50, i32* %52, align 4
  %53 = sub nsw i32 %.1, 1
  br label %54

54:                                               ; preds = %47, %45
  %.2 = phi i32 [ %53, %47 ], [ %.1, %45 ]
  br label %9

55:                                               ; preds = %9
  %56 = sext i32 %.01 to i64
  %57 = getelementptr inbounds i32, i32* %0, i64 %56
  store i32 %8, i32* %57, align 4
  %58 = sub nsw i32 %.01, 1
  %59 = call i32 @QuickSort(i32* %0, i32 %1, i32 %58)
  %60 = add nsw i32 %.01, 1
  %61 = call i32 @QuickSort(i32* %0, i32 %60, i32 %2)
  br label %62

62:                                               ; preds = %55, %3
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  store i32 10, i32* @n, align 4
  %2 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  store i32 4, i32* %2, align 16
  %3 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  store i32 3, i32* %3, align 4
  %4 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 2
  store i32 9, i32* %4, align 8
  %5 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 3
  store i32 2, i32* %5, align 4
  %6 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 4
  store i32 0, i32* %6, align 16
  %7 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 5
  store i32 1, i32* %7, align 4
  %8 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 6
  store i32 6, i32* %8, align 8
  %9 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 7
  store i32 5, i32* %9, align 4
  %10 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 8
  store i32 7, i32* %10, align 16
  %11 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 9
  store i32 8, i32* %11, align 4
  %12 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %13 = call i32 @QuickSort(i32* %12, i32 0, i32 9)
  br label %14

14:                                               ; preds = %17, %0
  %.0 = phi i32 [ %13, %0 ], [ %23, %17 ]
  %15 = load i32, i32* @n, align 4
  %16 = icmp slt i32 %.0, %15
  br i1 %16, label %17, label %24

17:                                               ; preds = %14
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %20)
  %22 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %23 = add nsw i32 %.0, 1
  br label %14

24:                                               ; preds = %14
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
