; ModuleID = '55_sort_test1.bc'
source_filename = "55_sort_test1.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@n = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @bubblesort(i32* %0) #0 {
  Br label %2

2:                                                ; preds = %36, %1
  %.01 = phi i32 [ 0, %1 ], [ %37, %36 ]
  %3 = load i32, i32* @n, align 4
  %4 = sub nsw i32 %3, 1
  %5 = icmp slt i32 %.01, %4
  Br i1 %5, label %6, label %38

6:                                                ; preds = %2
  Br label %7

7:                                                ; preds = %34, %6
  %.0 = phi i32 [ 0, %6 ], [ %35, %34 ]
  %8 = load i32, i32* @n, align 4
  %9 = sub nsw i32 %8, %.01
  %10 = sub nsw i32 %9, 1
  %11 = icmp slt i32 %.0, %10
  Br i1 %11, label %12, label %36

12:                                               ; preds = %7
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  %15 = load i32, i32* %14, align 4
  %16 = add nsw i32 %.0, 1
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds i32, i32* %0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = icmp sgt i32 %15, %19
  Br i1 %20, label %21, label %34

21:                                               ; preds = %12
  %22 = add nsw i32 %.0, 1
  %23 = sext i32 %22 to i64
  %24 = getelementptr inbounds i32, i32* %0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = sext i32 %.0 to i64
  %27 = getelementptr inbounds i32, i32* %0, i64 %26
  %28 = load i32, i32* %27, align 4
  %29 = add nsw i32 %.0, 1
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds i32, i32* %0, i64 %30
  store i32 %28, i32* %31, align 4
  %32 = sext i32 %.0 to i64
  %33 = getelementptr inbounds i32, i32* %0, i64 %32
  store i32 %25, i32* %33, align 4
  Br label %34

34:                                               ; preds = %21, %12
  %35 = add nsw i32 %.0, 1
  Br label %7

36:                                               ; preds = %7
  %37 = add nsw i32 %.01, 1
  Br label %2

38:                                               ; preds = %2
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
  %13 = call i32 @bubblesort(i32* %12)
  Br label %14

14:                                               ; preds = %17, %0
  %.0 = phi i32 [ %13, %0 ], [ %23, %17 ]
  %15 = load i32, i32* @n, align 4
  %16 = icmp slt i32 %.0, %15
  Br i1 %16, label %17, label %24

17:                                               ; preds = %14
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %20)
  %22 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %23 = add nsw i32 %.0, 1
  Br label %14

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
