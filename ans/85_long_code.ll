; ModuleID = '85_long_code.bc'
source_filename = "85_long_code.c"
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
define dso_local i32 @insertsort(i32* %0) #0 {
  Br label %2

2:                                                ; preds = %27, %1
  %.01 = phi i32 [ 1, %1 ], [ %31, %27 ]
  %3 = load i32, i32* @n, align 4
  %4 = icmp slt i32 %.01, %3
  Br i1 %4, label %5, label %32

5:                                                ; preds = %2
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds i32, i32* %0, i64 %6
  %8 = load i32, i32* %7, align 4
  %9 = sub nsw i32 %.01, 1
  Br label %10

10:                                               ; preds = %19, %5
  %.0 = phi i32 [ %9, %5 ], [ %26, %19 ]
  %11 = icmp sgt i32 %.0, -1
  Br i1 %11, label %12, label %17

12:                                               ; preds = %10
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  %15 = load i32, i32* %14, align 4
  %16 = icmp slt i32 %8, %15
  Br label %17

17:                                               ; preds = %12, %10
  %18 = phi i1 [ false, %10 ], [ %16, %12 ]
  Br i1 %18, label %19, label %27

19:                                               ; preds = %17
  %20 = sext i32 %.0 to i64
  %21 = getelementptr inbounds i32, i32* %0, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = add nsw i32 %.0, 1
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds i32, i32* %0, i64 %24
  store i32 %22, i32* %25, align 4
  %26 = sub nsw i32 %.0, 1
  Br label %10

27:                                               ; preds = %17
  %28 = add nsw i32 %.0, 1
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds i32, i32* %0, i64 %29
  store i32 %8, i32* %30, align 4
  %31 = add nsw i32 %.01, 1
  Br label %2

32:                                               ; preds = %2
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @QuickSort(i32* %0, i32 %1, i32 %2) #0 {
  %4 = icmp slt i32 %1, %2
  Br i1 %4, label %5, label %62

5:                                                ; preds = %3
  %6 = sext i32 %1 to i64
  %7 = getelementptr inbounds i32, i32* %0, i64 %6
  %8 = load i32, i32* %7, align 4
  Br label %9

9:                                                ; preds = %54, %5
  %.01 = phi i32 [ %1, %5 ], [ %.23, %54 ]
  %.0 = phi i32 [ %2, %5 ], [ %.2, %54 ]
  %10 = icmp slt i32 %.01, %.0
  Br i1 %10, label %11, label %55

11:                                               ; preds = %9
  Br label %12

12:                                               ; preds = %22, %11
  %.1 = phi i32 [ %.0, %11 ], [ %23, %22 ]
  %13 = icmp slt i32 %.01, %.1
  Br i1 %13, label %14, label %20

14:                                               ; preds = %12
  %15 = sext i32 %.1 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  %17 = load i32, i32* %16, align 4
  %18 = sub nsw i32 %8, 1
  %19 = icmp sgt i32 %17, %18
  Br label %20

20:                                               ; preds = %14, %12
  %21 = phi i1 [ false, %12 ], [ %19, %14 ]
  Br i1 %21, label %22, label %24

22:                                               ; preds = %20
  %23 = sub nsw i32 %.1, 1
  Br label %12

24:                                               ; preds = %20
  %25 = icmp slt i32 %.01, %.1
  Br i1 %25, label %26, label %33

26:                                               ; preds = %24
  %27 = sext i32 %.1 to i64
  %28 = getelementptr inbounds i32, i32* %0, i64 %27
  %29 = load i32, i32* %28, align 4
  %30 = sext i32 %.01 to i64
  %31 = getelementptr inbounds i32, i32* %0, i64 %30
  store i32 %29, i32* %31, align 4
  %32 = add nsw i32 %.01, 1
  Br label %33

33:                                               ; preds = %26, %24
  %.12 = phi i32 [ %32, %26 ], [ %.01, %24 ]
  Br label %34

34:                                               ; preds = %43, %33
  %.23 = phi i32 [ %.12, %33 ], [ %44, %43 ]
  %35 = icmp slt i32 %.23, %.1
  Br i1 %35, label %36, label %41

36:                                               ; preds = %34
  %37 = sext i32 %.23 to i64
  %38 = getelementptr inbounds i32, i32* %0, i64 %37
  %39 = load i32, i32* %38, align 4
  %40 = icmp slt i32 %39, %8
  Br label %41

41:                                               ; preds = %36, %34
  %42 = phi i1 [ false, %34 ], [ %40, %36 ]
  Br i1 %42, label %43, label %45

43:                                               ; preds = %41
  %44 = add nsw i32 %.23, 1
  Br label %34

45:                                               ; preds = %41
  %46 = icmp slt i32 %.23, %.1
  Br i1 %46, label %47, label %54

47:                                               ; preds = %45
  %48 = sext i32 %.23 to i64
  %49 = getelementptr inbounds i32, i32* %0, i64 %48
  %50 = load i32, i32* %49, align 4
  %51 = sext i32 %.1 to i64
  %52 = getelementptr inbounds i32, i32* %0, i64 %51
  store i32 %50, i32* %52, align 4
  %53 = sub nsw i32 %.1, 1
  Br label %54

54:                                               ; preds = %47, %45
  %.2 = phi i32 [ %53, %47 ], [ %.1, %45 ]
  Br label %9

55:                                               ; preds = %9
  %56 = sext i32 %.01 to i64
  %57 = getelementptr inbounds i32, i32* %0, i64 %56
  store i32 %8, i32* %57, align 4
  %58 = sub nsw i32 %.01, 1
  %59 = call i32 @QuickSort(i32* %0, i32 %1, i32 %58)
  %60 = add nsw i32 %.01, 1
  %61 = call i32 @QuickSort(i32* %0, i32 %60, i32 %2)
  Br label %62

62:                                               ; preds = %55, %3
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @getMid(i32* %0) #0 {
  %2 = load i32, i32* @n, align 4
  %3 = srem i32 %2, 2
  %4 = icmp eq i32 %3, 0
  Br i1 %4, label %5, label %17

5:                                                ; preds = %1
  %6 = load i32, i32* @n, align 4
  %7 = sdiv i32 %6, 2
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds i32, i32* %0, i64 %8
  %10 = load i32, i32* %9, align 4
  %11 = sub nsw i32 %7, 1
  %12 = sext i32 %11 to i64
  %13 = getelementptr inbounds i32, i32* %0, i64 %12
  %14 = load i32, i32* %13, align 4
  %15 = add nsw i32 %10, %14
  %16 = sdiv i32 %15, 2
  Br label %23

17:                                               ; preds = %1
  %18 = load i32, i32* @n, align 4
  %19 = sdiv i32 %18, 2
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds i32, i32* %0, i64 %20
  %22 = load i32, i32* %21, align 4
  Br label %23

23:                                               ; preds = %17, %5
  %.0 = phi i32 [ %16, %5 ], [ %22, %17 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @getMost(i32* %0) #0 {
  %2 = alloca [1000 x i32], align 16
  Br label %3

3:                                                ; preds = %5, %1
  %.01 = phi i32 [ 0, %1 ], [ %8, %5 ]
  %4 = icmp slt i32 %.01, 1000
  Br i1 %4, label %5, label %9

5:                                                ; preds = %3
  %6 = sext i32 %.01 to i64
  %7 = getelementptr inbounds [1000 x i32], [1000 x i32]* %2, i64 0, i64 %6
  store i32 0, i32* %7, align 4
  %8 = add nsw i32 %.01, 1
  Br label %3

9:                                                ; preds = %3
  Br label %10

10:                                               ; preds = %31, %9
  %.03 = phi i32 [ 0, %9 ], [ %.14, %31 ]
  %.12 = phi i32 [ 0, %9 ], [ %32, %31 ]
  %.0 = phi i32 [ undef, %9 ], [ %.1, %31 ]
  %11 = load i32, i32* @n, align 4
  %12 = icmp slt i32 %.12, %11
  Br i1 %12, label %13, label %33

13:                                               ; preds = %10
  %14 = sext i32 %.12 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds [1000 x i32], [1000 x i32]* %2, i64 0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = add nsw i32 %19, 1
  %21 = sext i32 %16 to i64
  %22 = getelementptr inbounds [1000 x i32], [1000 x i32]* %2, i64 0, i64 %21
  store i32 %20, i32* %22, align 4
  %23 = sext i32 %16 to i64
  %24 = getelementptr inbounds [1000 x i32], [1000 x i32]* %2, i64 0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = icmp sgt i32 %25, %.03
  Br i1 %26, label %27, label %31

27:                                               ; preds = %13
  %28 = sext i32 %16 to i64
  %29 = getelementptr inbounds [1000 x i32], [1000 x i32]* %2, i64 0, i64 %28
  %30 = load i32, i32* %29, align 4
  Br label %31

31:                                               ; preds = %27, %13
  %.14 = phi i32 [ %30, %27 ], [ %.03, %13 ]
  %.1 = phi i32 [ %16, %27 ], [ %.0, %13 ]
  %32 = add nsw i32 %.12, 1
  Br label %10

33:                                               ; preds = %10
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @revert(i32* %0) #0 {
  Br label %2

2:                                                ; preds = %4, %1
  %.01 = phi i32 [ 0, %1 ], [ %15, %4 ]
  %.0 = phi i32 [ 0, %1 ], [ %16, %4 ]
  %3 = icmp slt i32 %.01, %.0
  Br i1 %3, label %4, label %17

4:                                                ; preds = %2
  %5 = sext i32 %.01 to i64
  %6 = getelementptr inbounds i32, i32* %0, i64 %5
  %7 = load i32, i32* %6, align 4
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds i32, i32* %0, i64 %8
  %10 = load i32, i32* %9, align 4
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, i32* %0, i64 %11
  store i32 %10, i32* %12, align 4
  %13 = sext i32 %.0 to i64
  %14 = getelementptr inbounds i32, i32* %0, i64 %13
  store i32 %7, i32* %14, align 4
  %15 = add nsw i32 %.01, 1
  %16 = sub nsw i32 %.0, 1
  Br label %2

17:                                               ; preds = %2
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @arrCopy(i32* %0, i32* %1) #0 {
  Br label %3

3:                                                ; preds = %6, %2
  %.0 = phi i32 [ 0, %2 ], [ %12, %6 ]
  %4 = load i32, i32* @n, align 4
  %5 = icmp slt i32 %.0, %4
  Br i1 %5, label %6, label %13

6:                                                ; preds = %3
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds i32, i32* %0, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds i32, i32* %1, i64 %10
  store i32 %9, i32* %11, align 4
  %12 = add nsw i32 %.0, 1
  Br label %3

13:                                               ; preds = %3
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @calSum(i32* %0, i32 %1) #0 {
  Br label %3

3:                                                ; preds = %20, %2
  %.01 = phi i32 [ 0, %2 ], [ %.1, %20 ]
  %.0 = phi i32 [ 0, %2 ], [ %21, %20 ]
  %4 = load i32, i32* @n, align 4
  %5 = icmp slt i32 %.0, %4
  Br i1 %5, label %6, label %22

6:                                                ; preds = %3
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds i32, i32* %0, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = add nsw i32 %.01, %9
  %11 = srem i32 %.0, %1
  %12 = sub nsw i32 %1, 1
  %13 = icmp ne i32 %11, %12
  Br i1 %13, label %14, label %17

14:                                               ; preds = %6
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds i32, i32* %0, i64 %15
  store i32 0, i32* %16, align 4
  Br label %20

17:                                               ; preds = %6
  %18 = sext i32 %.0 to i64
  %19 = getelementptr inbounds i32, i32* %0, i64 %18
  store i32 %10, i32* %19, align 4
  Br label %20

20:                                               ; preds = %17, %14
  %.1 = phi i32 [ %10, %14 ], [ 0, %17 ]
  %21 = add nsw i32 %.0, 1
  Br label %3

22:                                               ; preds = %3
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @avgPooling(i32* %0, i32 %1) #0 {
  Br label %3

3:                                                ; preds = %39, %2
  %.03 = phi i32 [ 0, %2 ], [ %.25, %39 ]
  %.01 = phi i32 [ 0, %2 ], [ %40, %39 ]
  %.0 = phi i32 [ undef, %2 ], [ %.2, %39 ]
  %4 = load i32, i32* @n, align 4
  %5 = icmp slt i32 %.01, %4
  Br i1 %5, label %6, label %41

6:                                                ; preds = %3
  %7 = sub nsw i32 %1, 1
  %8 = icmp slt i32 %.01, %7
  Br i1 %8, label %9, label %14

9:                                                ; preds = %6
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds i32, i32* %0, i64 %10
  %12 = load i32, i32* %11, align 4
  %13 = add nsw i32 %.03, %12
  Br label %39

14:                                               ; preds = %6
  %15 = sub nsw i32 %1, 1
  %16 = icmp eq i32 %.01, %15
  Br i1 %16, label %17, label %22

17:                                               ; preds = %14
  %18 = getelementptr inbounds i32, i32* %0, i64 0
  %19 = load i32, i32* %18, align 4
  %20 = sdiv i32 %.03, %1
  %21 = getelementptr inbounds i32, i32* %0, i64 0
  store i32 %20, i32* %21, align 4
  Br label %38

22:                                               ; preds = %14
  %23 = sext i32 %.01 to i64
  %24 = getelementptr inbounds i32, i32* %0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = add nsw i32 %.03, %25
  %27 = sub nsw i32 %26, %.0
  %28 = sub nsw i32 %.01, %1
  %29 = add nsw i32 %28, 1
  %30 = sext i32 %29 to i64
  %31 = getelementptr inbounds i32, i32* %0, i64 %30
  %32 = load i32, i32* %31, align 4
  %33 = sdiv i32 %27, %1
  %34 = sub nsw i32 %.01, %1
  %35 = add nsw i32 %34, 1
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds i32, i32* %0, i64 %36
  store i32 %33, i32* %37, align 4
  Br label %38

38:                                               ; preds = %22, %17
  %.14 = phi i32 [ %.03, %17 ], [ %27, %22 ]
  %.1 = phi i32 [ %19, %17 ], [ %32, %22 ]
  Br label %39

39:                                               ; preds = %38, %9
  %.25 = phi i32 [ %13, %9 ], [ %.14, %38 ]
  %.2 = phi i32 [ %.0, %9 ], [ %.1, %38 ]
  %40 = add nsw i32 %.01, 1
  Br label %3

41:                                               ; preds = %3
  %42 = load i32, i32* @n, align 4
  %43 = sub nsw i32 %42, %1
  %44 = add nsw i32 %43, 1
  Br label %45

45:                                               ; preds = %48, %41
  %.12 = phi i32 [ %44, %41 ], [ %51, %48 ]
  %46 = load i32, i32* @n, align 4
  %47 = icmp slt i32 %.12, %46
  Br i1 %47, label %48, label %52

48:                                               ; preds = %45
  %49 = sext i32 %.12 to i64
  %50 = getelementptr inbounds i32, i32* %0, i64 %49
  store i32 0, i32* %50, align 4
  %51 = add nsw i32 %.12, 1
  Br label %45

52:                                               ; preds = %45
  ret i32 0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [32 x i32], align 16
  %2 = alloca [32 x i32], align 16
  store i32 32, i32* @n, align 4
  %3 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  store i32 7, i32* %3, align 16
  %4 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 1
  store i32 23, i32* %4, align 4
  %5 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 2
  store i32 89, i32* %5, align 8
  %6 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 3
  store i32 26, i32* %6, align 4
  %7 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 4
  store i32 282, i32* %7, align 16
  %8 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 5
  store i32 254, i32* %8, align 4
  %9 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 6
  store i32 27, i32* %9, align 8
  %10 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 7
  store i32 5, i32* %10, align 4
  %11 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 8
  store i32 83, i32* %11, align 16
  %12 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 9
  store i32 273, i32* %12, align 4
  %13 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 10
  store i32 574, i32* %13, align 8
  %14 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 11
  store i32 905, i32* %14, align 4
  %15 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 12
  store i32 354, i32* %15, align 16
  %16 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 13
  store i32 657, i32* %16, align 4
  %17 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 14
  store i32 935, i32* %17, align 8
  %18 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 15
  store i32 264, i32* %18, align 4
  %19 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 16
  store i32 639, i32* %19, align 16
  %20 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 17
  store i32 459, i32* %20, align 4
  %21 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 18
  store i32 29, i32* %21, align 8
  %22 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 19
  store i32 68, i32* %22, align 4
  %23 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 20
  store i32 929, i32* %23, align 16
  %24 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 21
  store i32 756, i32* %24, align 4
  %25 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 22
  store i32 452, i32* %25, align 8
  %26 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 23
  store i32 279, i32* %26, align 4
  %27 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 24
  store i32 58, i32* %27, align 16
  %28 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 25
  store i32 87, i32* %28, align 4
  %29 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 26
  store i32 96, i32* %29, align 8
  %30 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 27
  store i32 36, i32* %30, align 4
  %31 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 28
  store i32 39, i32* %31, align 16
  %32 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 29
  store i32 28, i32* %32, align 4
  %33 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 30
  store i32 1, i32* %33, align 8
  %34 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 31
  store i32 290, i32* %34, align 4
  %35 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %36 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %37 = call i32 @arrCopy(i32* %35, i32* %36)
  %38 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %39 = call i32 @revert(i32* %38)
  Br label %40

40:                                               ; preds = %42, %0
  %.0 = phi i32 [ 0, %0 ], [ %47, %42 ]
  %41 = icmp slt i32 %.0, 32
  Br i1 %41, label %42, label %48

42:                                               ; preds = %40
  %43 = sext i32 %.0 to i64
  %44 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %43
  %45 = load i32, i32* %44, align 4
  %46 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %45)
  %47 = add nsw i32 %.0, 1
  Br label %40

48:                                               ; preds = %40
  %49 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %50 = call i32 @bubblesort(i32* %49)
  Br label %51

51:                                               ; preds = %53, %48
  %.1 = phi i32 [ 0, %48 ], [ %58, %53 ]
  %52 = icmp slt i32 %.1, 32
  Br i1 %52, label %53, label %59

53:                                               ; preds = %51
  %54 = sext i32 %.1 to i64
  %55 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %54
  %56 = load i32, i32* %55, align 4
  %57 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %56)
  %58 = add nsw i32 %.1, 1
  Br label %51

59:                                               ; preds = %51
  %60 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %61 = call i32 @getMid(i32* %60)
  %62 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %61)
  %63 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %64 = call i32 @getMost(i32* %63)
  %65 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %64)
  %66 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %67 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %68 = call i32 @arrCopy(i32* %66, i32* %67)
  %69 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %70 = call i32 @bubblesort(i32* %69)
  Br label %71

71:                                               ; preds = %73, %59
  %.2 = phi i32 [ 0, %59 ], [ %78, %73 ]
  %72 = icmp slt i32 %.2, 32
  Br i1 %72, label %73, label %79

73:                                               ; preds = %71
  %74 = sext i32 %.2 to i64
  %75 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %74
  %76 = load i32, i32* %75, align 4
  %77 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %76)
  %78 = add nsw i32 %.2, 1
  Br label %71

79:                                               ; preds = %71
  %80 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %81 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %82 = call i32 @arrCopy(i32* %80, i32* %81)
  %83 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %84 = call i32 @insertsort(i32* %83)
  Br label %85

85:                                               ; preds = %87, %79
  %.3 = phi i32 [ 0, %79 ], [ %92, %87 ]
  %86 = icmp slt i32 %.3, 32
  Br i1 %86, label %87, label %93

87:                                               ; preds = %85
  %88 = sext i32 %.3 to i64
  %89 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %88
  %90 = load i32, i32* %89, align 4
  %91 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %90)
  %92 = add nsw i32 %.3, 1
  Br label %85

93:                                               ; preds = %85
  %94 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %95 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %96 = call i32 @arrCopy(i32* %94, i32* %95)
  %97 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %98 = call i32 @QuickSort(i32* %97, i32 0, i32 31)
  Br label %99

99:                                               ; preds = %101, %93
  %.4 = phi i32 [ 0, %93 ], [ %106, %101 ]
  %100 = icmp slt i32 %.4, 32
  Br i1 %100, label %101, label %107

101:                                              ; preds = %99
  %102 = sext i32 %.4 to i64
  %103 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %102
  %104 = load i32, i32* %103, align 4
  %105 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %104)
  %106 = add nsw i32 %.4, 1
  Br label %99

107:                                              ; preds = %99
  %108 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %109 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %110 = call i32 @arrCopy(i32* %108, i32* %109)
  %111 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %112 = call i32 @calSum(i32* %111, i32 4)
  Br label %113

113:                                              ; preds = %115, %107
  %.5 = phi i32 [ 0, %107 ], [ %120, %115 ]
  %114 = icmp slt i32 %.5, 32
  Br i1 %114, label %115, label %121

115:                                              ; preds = %113
  %116 = sext i32 %.5 to i64
  %117 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %116
  %118 = load i32, i32* %117, align 4
  %119 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %118)
  %120 = add nsw i32 %.5, 1
  Br label %113

121:                                              ; preds = %113
  %122 = getelementptr inbounds [32 x i32], [32 x i32]* %1, i64 0, i64 0
  %123 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %124 = call i32 @arrCopy(i32* %122, i32* %123)
  %125 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 0
  %126 = call i32 @avgPooling(i32* %125, i32 3)
  Br label %127

127:                                              ; preds = %129, %121
  %.6 = phi i32 [ 0, %121 ], [ %134, %129 ]
  %128 = icmp slt i32 %.6, 32
  Br i1 %128, label %129, label %135

129:                                              ; preds = %127
  %130 = sext i32 %.6 to i64
  %131 = getelementptr inbounds [32 x i32], [32 x i32]* %2, i64 0, i64 %130
  %132 = load i32, i32* %131, align 4
  %133 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %132)
  %134 = add nsw i32 %.6, 1
  Br label %127

135:                                              ; preds = %127
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
