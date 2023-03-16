; ModuleID = '58_sort_test4.bc'
source_filename = "58_sort_test4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@n = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @select_sort(i32* %0, i32 %1) #0 {
  br label %3

3:                                                ; preds = %34, %2
  %.02 = phi i32 [ 0, %2 ], [ %35, %34 ]
  %4 = sub nsw i32 %1, 1
  %5 = icmp slt i32 %.02, %4
  br i1 %5, label %6, label %36

6:                                                ; preds = %3
  %7 = add nsw i32 %.02, 1
  br label %8

8:                                                ; preds = %19, %6
  %.01 = phi i32 [ %7, %6 ], [ %20, %19 ]
  %.0 = phi i32 [ %.02, %6 ], [ %.1, %19 ]
  %9 = icmp slt i32 %.01, %1
  br i1 %9, label %10, label %21

10:                                               ; preds = %8
  %11 = sext i32 %.0 to i64
  %12 = getelementptr inbounds i32, i32* %0, i64 %11
  %13 = load i32, i32* %12, align 4
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = icmp sgt i32 %13, %16
  br i1 %17, label %18, label %19

18:                                               ; preds = %10
  br label %19

19:                                               ; preds = %18, %10
  %.1 = phi i32 [ %.01, %18 ], [ %.0, %10 ]
  %20 = add nsw i32 %.01, 1
  br label %8

21:                                               ; preds = %8
  %22 = icmp ne i32 %.0, %.02
  br i1 %22, label %23, label %34

23:                                               ; preds = %21
  %24 = sext i32 %.0 to i64
  %25 = getelementptr inbounds i32, i32* %0, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = sext i32 %.02 to i64
  %28 = getelementptr inbounds i32, i32* %0, i64 %27
  %29 = load i32, i32* %28, align 4
  %30 = sext i32 %.0 to i64
  %31 = getelementptr inbounds i32, i32* %0, i64 %30
  store i32 %29, i32* %31, align 4
  %32 = sext i32 %.02 to i64
  %33 = getelementptr inbounds i32, i32* %0, i64 %32
  store i32 %26, i32* %33, align 4
  br label %34

34:                                               ; preds = %23, %21
  %35 = add nsw i32 %.02, 1
  br label %3

36:                                               ; preds = %3
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
  %13 = load i32, i32* @n, align 4
  %14 = call i32 @select_sort(i32* %12, i32 %13)
  br label %15

15:                                               ; preds = %18, %0
  %.0 = phi i32 [ %14, %0 ], [ %24, %18 ]
  %16 = load i32, i32* @n, align 4
  %17 = icmp slt i32 %.0, %16
  br i1 %17, label %18, label %25

18:                                               ; preds = %15
  %19 = sext i32 %.0 to i64
  %20 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %19
  %21 = load i32, i32* %20, align 4
  %22 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %21)
  %23 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %24 = add nsw i32 %.0, 1
  br label %15

25:                                               ; preds = %15
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
