; ModuleID = '59_sort_test5.bc'
source_filename = "59_sort_test5.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@n = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @swap(i32* %0, i32 %1, i32 %2) #0 {
  %4 = sext i32 %1 to i64
  %5 = getelementptr inbounds i32, i32* %0, i64 %4
  %6 = load i32, i32* %5, align 4
  %7 = sext i32 %2 to i64
  %8 = getelementptr inbounds i32, i32* %0, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = sext i32 %1 to i64
  %11 = getelementptr inbounds i32, i32* %0, i64 %10
  store i32 %9, i32* %11, align 4
  %12 = sext i32 %2 to i64
  %13 = getelementptr inbounds i32, i32* %0, i64 %12
  store i32 %6, i32* %13, align 4
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @heap_ajust(i32* %0, i32 %1, i32 %2) #0 {
  %4 = mul nsw i32 %1, 2
  %5 = add nsw i32 %4, 1
  br label %6

6:                                                ; preds = %35, %3
  %.02 = phi i32 [ %1, %3 ], [ %.1, %35 ]
  %.01 = phi i32 [ %5, %3 ], [ %34, %35 ]
  %7 = add nsw i32 %2, 1
  %8 = icmp slt i32 %.01, %7
  br i1 %8, label %9, label %36

9:                                                ; preds = %6
  %10 = icmp slt i32 %.01, %2
  br i1 %10, label %11, label %22

11:                                               ; preds = %9
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, i32* %0, i64 %12
  %14 = load i32, i32* %13, align 4
  %15 = add nsw i32 %.01, 1
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds i32, i32* %0, i64 %16
  %18 = load i32, i32* %17, align 4
  %19 = icmp slt i32 %14, %18
  br i1 %19, label %20, label %22

20:                                               ; preds = %11
  %21 = add nsw i32 %.01, 1
  br label %22

22:                                               ; preds = %20, %11, %9
  %.1 = phi i32 [ %21, %20 ], [ %.01, %11 ], [ %.01, %9 ]
  %23 = sext i32 %.02 to i64
  %24 = getelementptr inbounds i32, i32* %0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = sext i32 %.1 to i64
  %27 = getelementptr inbounds i32, i32* %0, i64 %26
  %28 = load i32, i32* %27, align 4
  %29 = icmp sgt i32 %25, %28
  br i1 %29, label %30, label %31

30:                                               ; preds = %22
  br label %37

31:                                               ; preds = %22
  %32 = call i32 @swap(i32* %0, i32 %.02, i32 %.1)
  %33 = mul nsw i32 %.1, 2
  %34 = add nsw i32 %33, 1
  br label %35

35:                                               ; preds = %31
  br label %6

36:                                               ; preds = %6
  br label %37

37:                                               ; preds = %36, %30
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @heap_sort(i32* %0, i32 %1) #0 {
  %3 = sdiv i32 %1, 2
  %4 = sub nsw i32 %3, 1
  br label %5

5:                                                ; preds = %7, %2
  %.0 = phi i32 [ %4, %2 ], [ %10, %7 ]
  %6 = icmp sgt i32 %.0, -1
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = sub nsw i32 %1, 1
  %9 = call i32 @heap_ajust(i32* %0, i32 %.0, i32 %8)
  %10 = sub nsw i32 %.0, 1
  br label %5

11:                                               ; preds = %5
  %12 = sub nsw i32 %1, 1
  br label %13

13:                                               ; preds = %15, %11
  %.1 = phi i32 [ %12, %11 ], [ %19, %15 ]
  %14 = icmp sgt i32 %.1, 0
  br i1 %14, label %15, label %20

15:                                               ; preds = %13
  %16 = call i32 @swap(i32* %0, i32 0, i32 %.1)
  %17 = sub nsw i32 %.1, 1
  %18 = call i32 @heap_ajust(i32* %0, i32 0, i32 %17)
  %19 = sub nsw i32 %.1, 1
  br label %13

20:                                               ; preds = %13
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
  %14 = call i32 @heap_sort(i32* %12, i32 %13)
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
