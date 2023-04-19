; ModuleID = '76_n_queens.bc'
source_filename = "76_n_queens.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@sum = dso_local global i32 0, align 4
@n = common dso_local global i32 0, align 4
@ans = common dso_local global [50 x i32] zeroinitializer, align 16
@row = common dso_local global [50 x i32] zeroinitializer, align 16
@line1 = common dso_local global [50 x i32] zeroinitializer, align 16
@line2 = common dso_local global [100 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local void @printans() #0 {
  %1 = load i32, i32* @sum, align 4
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @sum, align 4
  Br label %3

3:                                                ; preds = %17, %0
  %.0 = phi i32 [ 1, %0 ], [ %18, %17 ]
  %4 = load i32, i32* @n, align 4
  %5 = icmp sle i32 %.0, %4
  Br i1 %5, label %6, label %19

6:                                                ; preds = %3
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [50 x i32], [50 x i32]* @ans, i64 0, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %9)
  %11 = load i32, i32* @n, align 4
  %12 = icmp eq i32 %.0, %11
  Br i1 %12, label %13, label %15

13:                                               ; preds = %6
  %14 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  Br label %19

15:                                               ; preds = %6
  %16 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  Br label %17

17:                                               ; preds = %15
  %18 = add nsw i32 %.0, 1
  Br label %3

19:                                               ; preds = %13, %3
  ret void
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @f(i32 %0) #0 {
  Br label %2

2:                                                ; preds = %52, %1
  %.0 = phi i32 [ 1, %1 ], [ %53, %52 ]
  %3 = load i32, i32* @n, align 4
  %4 = icmp sle i32 %.0, %3
  Br i1 %4, label %5, label %54

5:                                                ; preds = %2
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [50 x i32], [50 x i32]* @row, i64 0, i64 %6
  %8 = load i32, i32* %7, align 4
  %9 = icmp ne i32 %8, 1
  Br i1 %9, label %10, label %52

10:                                               ; preds = %5
  %11 = add nsw i32 %0, %.0
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds [50 x i32], [50 x i32]* @line1, i64 0, i64 %12
  %14 = load i32, i32* %13, align 4
  %15 = icmp eq i32 %14, 0
  Br i1 %15, label %16, label %52

16:                                               ; preds = %10
  %17 = load i32, i32* @n, align 4
  %18 = add nsw i32 %17, %0
  %19 = sub nsw i32 %18, %.0
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds [100 x i32], [100 x i32]* @line2, i64 0, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = icmp ne i32 %22, 0
  Br i1 %23, label %52, label %24

24:                                               ; preds = %16
  %25 = sext i32 %0 to i64
  %26 = getelementptr inbounds [50 x i32], [50 x i32]* @ans, i64 0, i64 %25
  store i32 %.0, i32* %26, align 4
  %27 = load i32, i32* @n, align 4
  %28 = icmp eq i32 %0, %27
  Br i1 %28, label %29, label %30

29:                                               ; preds = %24
  call void @printans()
  Br label %30

30:                                               ; preds = %29, %24
  %31 = sext i32 %.0 to i64
  %32 = getelementptr inbounds [50 x i32], [50 x i32]* @row, i64 0, i64 %31
  store i32 1, i32* %32, align 4
  %33 = add nsw i32 %0, %.0
  %34 = sext i32 %33 to i64
  %35 = getelementptr inbounds [50 x i32], [50 x i32]* @line1, i64 0, i64 %34
  store i32 1, i32* %35, align 4
  %36 = load i32, i32* @n, align 4
  %37 = add nsw i32 %36, %0
  %38 = sub nsw i32 %37, %.0
  %39 = sext i32 %38 to i64
  %40 = getelementptr inbounds [100 x i32], [100 x i32]* @line2, i64 0, i64 %39
  store i32 1, i32* %40, align 4
  %41 = add nsw i32 %0, 1
  call void @f(i32 %41)
  %42 = sext i32 %.0 to i64
  %43 = getelementptr inbounds [50 x i32], [50 x i32]* @row, i64 0, i64 %42
  store i32 0, i32* %43, align 4
  %44 = add nsw i32 %0, %.0
  %45 = sext i32 %44 to i64
  %46 = getelementptr inbounds [50 x i32], [50 x i32]* @line1, i64 0, i64 %45
  store i32 0, i32* %46, align 4
  %47 = load i32, i32* @n, align 4
  %48 = add nsw i32 %47, %0
  %49 = sub nsw i32 %48, %.0
  %50 = sext i32 %49 to i64
  %51 = getelementptr inbounds [100 x i32], [100 x i32]* @line2, i64 0, i64 %50
  store i32 0, i32* %51, align 4
  Br label %52

52:                                               ; preds = %30, %16, %10, %5
  %53 = add nsw i32 %.0, 1
  Br label %2

54:                                               ; preds = %2
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  Br label %2

2:                                                ; preds = %4, %0
  %.0 = phi i32 [ %1, %0 ], [ %6, %4 ]
  %3 = icmp sgt i32 %.0, 0
  Br i1 %3, label %4, label %7

4:                                                ; preds = %2
  %5 = call i32 (...) @getint()
  store i32 %5, i32* @n, align 4
  call void @f(i32 1)
  %6 = sub nsw i32 %.0, 1
  Br label %2

7:                                                ; preds = %2
  %8 = load i32, i32* @sum, align 4
  ret i32 %8
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
