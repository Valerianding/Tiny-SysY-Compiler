; ModuleID = '60_sort_test6.bc'
source_filename = "60_sort_test6.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@n = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @counting_sort(i32* %0, i32* %1, i32 %2) #0 {
  %4 = alloca [10 x i32], align 16
  Br label %5

5:                                                ; preds = %7, %3
  %.0 = phi i32 [ 0, %3 ], [ %10, %7 ]
  %6 = icmp slt i32 %.0, 10
  Br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %8
  store i32 0, i32* %9, align 4
  %10 = add nsw i32 %.0, 1
  Br label %5

11:                                               ; preds = %5
  Br label %12

12:                                               ; preds = %14, %11
  %.02 = phi i32 [ 0, %11 ], [ %27, %14 ]
  %13 = icmp slt i32 %.02, %2
  Br i1 %13, label %14, label %28

14:                                               ; preds = %12
  %15 = sext i32 %.02 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = add nsw i32 %20, 1
  %22 = sext i32 %.02 to i64
  %23 = getelementptr inbounds i32, i32* %0, i64 %22
  %24 = load i32, i32* %23, align 4
  %25 = sext i32 %24 to i64
  %26 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %25
  store i32 %21, i32* %26, align 4
  %27 = add nsw i32 %.02, 1
  Br label %12

28:                                               ; preds = %12
  Br label %29

29:                                               ; preds = %31, %28
  %.1 = phi i32 [ 1, %28 ], [ %42, %31 ]
  %30 = icmp slt i32 %.1, 10
  Br i1 %30, label %31, label %43

31:                                               ; preds = %29
  %32 = sext i32 %.1 to i64
  %33 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %32
  %34 = load i32, i32* %33, align 4
  %35 = sub nsw i32 %.1, 1
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %36
  %38 = load i32, i32* %37, align 4
  %39 = add nsw i32 %34, %38
  %40 = sext i32 %.1 to i64
  %41 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %40
  store i32 %39, i32* %41, align 4
  %42 = add nsw i32 %.1, 1
  Br label %29

43:                                               ; preds = %29
  Br label %44

44:                                               ; preds = %46, %43
  %.01 = phi i32 [ %2, %43 ], [ %74, %46 ]
  %45 = icmp sgt i32 %.01, 0
  Br i1 %45, label %46, label %75

46:                                               ; preds = %44
  %47 = sub nsw i32 %.01, 1
  %48 = sext i32 %47 to i64
  %49 = getelementptr inbounds i32, i32* %0, i64 %48
  %50 = load i32, i32* %49, align 4
  %51 = sext i32 %50 to i64
  %52 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %51
  %53 = load i32, i32* %52, align 4
  %54 = sub nsw i32 %53, 1
  %55 = sub nsw i32 %.01, 1
  %56 = sext i32 %55 to i64
  %57 = getelementptr inbounds i32, i32* %0, i64 %56
  %58 = load i32, i32* %57, align 4
  %59 = sext i32 %58 to i64
  %60 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %59
  store i32 %54, i32* %60, align 4
  %61 = sub nsw i32 %.01, 1
  %62 = sext i32 %61 to i64
  %63 = getelementptr inbounds i32, i32* %0, i64 %62
  %64 = load i32, i32* %63, align 4
  %65 = sub nsw i32 %.01, 1
  %66 = sext i32 %65 to i64
  %67 = getelementptr inbounds i32, i32* %0, i64 %66
  %68 = load i32, i32* %67, align 4
  %69 = sext i32 %68 to i64
  %70 = getelementptr inbounds [10 x i32], [10 x i32]* %4, i64 0, i64 %69
  %71 = load i32, i32* %70, align 4
  %72 = sext i32 %71 to i64
  %73 = getelementptr inbounds i32, i32* %1, i64 %72
  store i32 %64, i32* %73, align 4
  %74 = sub nsw i32 %.01, 1
  Br label %44

75:                                               ; preds = %44
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = alloca [10 x i32], align 16
  store i32 10, i32* @n, align 4
  %3 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  store i32 4, i32* %3, align 16
  %4 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  store i32 3, i32* %4, align 4
  %5 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 2
  store i32 9, i32* %5, align 8
  %6 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 3
  store i32 2, i32* %6, align 4
  %7 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 4
  store i32 0, i32* %7, align 16
  %8 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 5
  store i32 1, i32* %8, align 4
  %9 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 6
  store i32 6, i32* %9, align 8
  %10 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 7
  store i32 5, i32* %10, align 4
  %11 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 8
  store i32 7, i32* %11, align 16
  %12 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 9
  store i32 8, i32* %12, align 4
  %13 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %14 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i64 0, i64 0
  %15 = load i32, i32* @n, align 4
  %16 = call i32 @counting_sort(i32* %13, i32* %14, i32 %15)
  Br label %17

17:                                               ; preds = %20, %0
  %.0 = phi i32 [ %16, %0 ], [ %26, %20 ]
  %18 = load i32, i32* @n, align 4
  %19 = icmp slt i32 %.0, %18
  Br i1 %19, label %20, label %27

20:                                               ; preds = %17
  %21 = sext i32 %.0 to i64
  %22 = getelementptr inbounds [10 x i32], [10 x i32]* %2, i64 0, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %23)
  %25 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %26 = add nsw i32 %.0, 1
  Br label %17

27:                                               ; preds = %17
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
