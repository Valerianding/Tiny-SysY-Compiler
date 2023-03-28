; ModuleID = '74_kmp.bc'
source_filename = "74_kmp.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @get_next(i32* %0, i32* %1) #0 {
  %3 = getelementptr inbounds i32, i32* %1, i64 0
  store i32 -1, i32* %3, align 4
  br label %4

4:                                                ; preds = %28, %2
  %.01 = phi i32 [ 0, %2 ], [ %.12, %28 ]
  %.0 = phi i32 [ -1, %2 ], [ %.1, %28 ]
  %5 = sext i32 %.01 to i64
  %6 = getelementptr inbounds i32, i32* %0, i64 %5
  %7 = load i32, i32* %6, align 4
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %29

9:                                                ; preds = %4
  %10 = icmp eq i32 %.0, -1
  br i1 %10, label %19, label %11

11:                                               ; preds = %9
  %12 = sext i32 %.01 to i64
  %13 = getelementptr inbounds i32, i32* %0, i64 %12
  %14 = load i32, i32* %13, align 4
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = icmp eq i32 %14, %17
  br i1 %18, label %19, label %24

19:                                               ; preds = %11, %9
  %20 = add nsw i32 %.0, 1
  %21 = add nsw i32 %.01, 1
  %22 = sext i32 %21 to i64
  %23 = getelementptr inbounds i32, i32* %1, i64 %22
  store i32 %20, i32* %23, align 4
  br label %28

24:                                               ; preds = %11
  %25 = sext i32 %.0 to i64
  %26 = getelementptr inbounds i32, i32* %1, i64 %25
  %27 = load i32, i32* %26, align 4
  br label %28

28:                                               ; preds = %24, %19
  %.12 = phi i32 [ %21, %19 ], [ %.01, %24 ]
  %.1 = phi i32 [ %20, %19 ], [ %27, %24 ]
  br label %4

29:                                               ; preds = %4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @KMP(i32* %0, i32* %1) #0 {
  %3 = alloca [4096 x i32], align 16
  %4 = getelementptr inbounds [4096 x i32], [4096 x i32]* %3, i64 0, i64 0
  call void @get_next(i32* %0, i32* %4)
  br label %5

5:                                                ; preds = %36, %2
  %.02 = phi i32 [ 0, %2 ], [ %.24, %36 ]
  %.01 = phi i32 [ 0, %2 ], [ %.2, %36 ]
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds i32, i32* %1, i64 %6
  %8 = load i32, i32* %7, align 4
  %9 = icmp ne i32 %8, 0
  br i1 %9, label %10, label %37

10:                                               ; preds = %5
  %11 = sext i32 %.02 to i64
  %12 = getelementptr inbounds i32, i32* %0, i64 %11
  %13 = load i32, i32* %12, align 4
  %14 = sext i32 %.01 to i64
  %15 = getelementptr inbounds i32, i32* %1, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = icmp eq i32 %13, %16
  br i1 %17, label %18, label %27

18:                                               ; preds = %10
  %19 = add nsw i32 %.02, 1
  %20 = add nsw i32 %.01, 1
  %21 = sext i32 %19 to i64
  %22 = getelementptr inbounds i32, i32* %0, i64 %21
  %23 = load i32, i32* %22, align 4
  %24 = icmp ne i32 %23, 0
  br i1 %24, label %26, label %25

25:                                               ; preds = %18
  br label %38

26:                                               ; preds = %18
  br label %36

27:                                               ; preds = %10
  %28 = sext i32 %.02 to i64
  %29 = getelementptr inbounds [4096 x i32], [4096 x i32]* %3, i64 0, i64 %28
  %30 = load i32, i32* %29, align 4
  %31 = icmp eq i32 %30, -1
  br i1 %31, label %32, label %35

32:                                               ; preds = %27
  %33 = add nsw i32 %30, 1
  %34 = add nsw i32 %.01, 1
  br label %35

35:                                               ; preds = %32, %27
  %.13 = phi i32 [ %33, %32 ], [ %30, %27 ]
  %.1 = phi i32 [ %34, %32 ], [ %.01, %27 ]
  br label %36

36:                                               ; preds = %35, %26
  %.24 = phi i32 [ %19, %26 ], [ %.13, %35 ]
  %.2 = phi i32 [ %20, %26 ], [ %.1, %35 ]
  br label %5

37:                                               ; preds = %5
  br label %38

38:                                               ; preds = %37, %25
  %.0 = phi i32 [ %20, %25 ], [ -1, %37 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @read_str(i32* %0) #0 {
  br label %2

2:                                                ; preds = %11, %1
  %.0 = phi i32 [ 0, %1 ], [ %12, %11 ]
  %3 = call i32 (...) @getch()
  %4 = sext i32 %.0 to i64
  %5 = getelementptr inbounds i32, i32* %0, i64 %4
  store i32 %3, i32* %5, align 4
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds i32, i32* %0, i64 %6
  %8 = load i32, i32* %7, align 4
  %9 = icmp eq i32 %8, 10
  br i1 %9, label %10, label %11

10:                                               ; preds = %2
  br label %13

11:                                               ; preds = %2
  %12 = add nsw i32 %.0, 1
  br label %2

13:                                               ; preds = %10
  %14 = sext i32 %.0 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  store i32 0, i32* %15, align 4
  ret i32 %.0
}

declare dso_local i32 @getch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [4096 x i32], align 16
  %2 = alloca [4096 x i32], align 16
  %3 = getelementptr inbounds [4096 x i32], [4096 x i32]* %1, i64 0, i64 0
  %4 = call i32 @read_str(i32* %3)
  %5 = getelementptr inbounds [4096 x i32], [4096 x i32]* %2, i64 0, i64 0
  %6 = call i32 @read_str(i32* %5)
  %7 = getelementptr inbounds [4096 x i32], [4096 x i32]* %1, i64 0, i64 0
  %8 = getelementptr inbounds [4096 x i32], [4096 x i32]* %2, i64 0, i64 0
  %9 = call i32 @KMP(i32* %7, i32* %8)
  %10 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %9)
  %11 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
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
