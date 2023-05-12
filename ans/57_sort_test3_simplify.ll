; ModuleID = '57_sort_test3.ll'
source_filename = "57_sort_test3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@n = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @QuickSort(i32* %0, i32 %1, i32 %2) #0 {
  %4 = icmp slt i32 %1, %2
  br i1 %4, label %5, label %55

5:                                                ; preds = %3
  %6 = sext i32 %1 to i64
  %7 = getelementptr inbounds i32, i32* %0, i64 %6
  %8 = load i32, i32* %7, align 4
  br label %9

9:                                                ; preds = %47, %5
  %.01 = phi i32 [ %1, %5 ], [ %.23, %47 ]
  %.0 = phi i32 [ %2, %5 ], [ %.2, %47 ]
  %10 = icmp slt i32 %.01, %.0
  br i1 %10, label %11, label %48

11:                                               ; preds = %9, %19
  %.1 = phi i32 [ %20, %19 ], [ %.0, %9 ]
  %12 = icmp slt i32 %.01, %.1
  br i1 %12, label %13, label %.critedge

13:                                               ; preds = %11
  %14 = sext i32 %.1 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = sub nsw i32 %8, 1
  %18 = icmp sgt i32 %16, %17
  br i1 %18, label %19, label %.critedge

19:                                               ; preds = %13
  %20 = sub nsw i32 %.1, 1
  br label %11

.critedge:                                        ; preds = %11, %13
  %21 = icmp slt i32 %.01, %.1
  br i1 %21, label %22, label %29

22:                                               ; preds = %.critedge
  %23 = sext i32 %.1 to i64
  %24 = getelementptr inbounds i32, i32* %0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = sext i32 %.01 to i64
  %27 = getelementptr inbounds i32, i32* %0, i64 %26
  store i32 %25, i32* %27, align 4
  %28 = add nsw i32 %.01, 1
  br label %29

29:                                               ; preds = %22, %.critedge
  %.12 = phi i32 [ %28, %22 ], [ %.01, %.critedge ]
  br label %30

30:                                               ; preds = %37, %29
  %.23 = phi i32 [ %.12, %29 ], [ %38, %37 ]
  %31 = icmp slt i32 %.23, %.1
  br i1 %31, label %32, label %.critedge1

32:                                               ; preds = %30
  %33 = sext i32 %.23 to i64
  %34 = getelementptr inbounds i32, i32* %0, i64 %33
  %35 = load i32, i32* %34, align 4
  %36 = icmp slt i32 %35, %8
  br i1 %36, label %37, label %.critedge1

37:                                               ; preds = %32
  %38 = add nsw i32 %.23, 1
  br label %30

.critedge1:                                       ; preds = %30, %32
  %39 = icmp slt i32 %.23, %.1
  br i1 %39, label %40, label %47

40:                                               ; preds = %.critedge1
  %41 = sext i32 %.23 to i64
  %42 = getelementptr inbounds i32, i32* %0, i64 %41
  %43 = load i32, i32* %42, align 4
  %44 = sext i32 %.1 to i64
  %45 = getelementptr inbounds i32, i32* %0, i64 %44
  store i32 %43, i32* %45, align 4
  %46 = sub nsw i32 %.1, 1
  br label %47

47:                                               ; preds = %40, %.critedge1
  %.2 = phi i32 [ %46, %40 ], [ %.1, %.critedge1 ]
  br label %9

48:                                               ; preds = %9
  %49 = sext i32 %.01 to i64
  %50 = getelementptr inbounds i32, i32* %0, i64 %49
  store i32 %8, i32* %50, align 4
  %51 = sub nsw i32 %.01, 1
  %52 = call i32 @QuickSort(i32* %0, i32 %1, i32 %51)
  %53 = add nsw i32 %.01, 1
  %54 = call i32 @QuickSort(i32* %0, i32 %53, i32 %2)
  br label %55

55:                                               ; preds = %48, %3
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
