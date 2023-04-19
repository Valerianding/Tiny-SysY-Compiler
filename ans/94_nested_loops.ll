; ModuleID = '94_nested_loops.bc'
source_filename = "94_nested_loops.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@arr1 = common dso_local global [10 x [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]] zeroinitializer, align 16
@arr2 = common dso_local global [10 x [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local void @loop1(i32 %0, i32 %1) #0 {
  br label %3

3:                                                ; preds = %61, %2
  %.02 = phi i32 [ 0, %2 ], [ %62, %61 ]
  %4 = icmp slt i32 %.02, %0
  br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = icmp slt i32 %.02, %1
  br label %7

7:                                                ; preds = %5, %3
  %8 = phi i1 [ false, %3 ], [ %6, %5 ]
  br i1 %8, label %9, label %63

9:                                                ; preds = %7
  br label %10

10:                                               ; preds = %59, %9
  %.03 = phi i32 [ 0, %9 ], [ %60, %59 ]
  %11 = icmp slt i32 %.03, 2
  br i1 %11, label %12, label %61

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %57, %12
  %.04 = phi i32 [ 0, %12 ], [ %58, %57 ]
  %14 = icmp slt i32 %.04, 3
  br i1 %14, label %15, label %59

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %55, %15
  %.05 = phi i32 [ 0, %15 ], [ %56, %55 ]
  %17 = icmp slt i32 %.05, 4
  br i1 %17, label %18, label %57

18:                                               ; preds = %16
  br label %19

19:                                               ; preds = %53, %18
  %.06 = phi i32 [ 0, %18 ], [ %54, %53 ]
  %20 = icmp slt i32 %.06, 5
  br i1 %20, label %21, label %55

21:                                               ; preds = %19
  br label %22

22:                                               ; preds = %51, %21
  %.01 = phi i32 [ 0, %21 ], [ %52, %51 ]
  %23 = icmp slt i32 %.01, 6
  br i1 %23, label %24, label %53

24:                                               ; preds = %22
  br label %25

25:                                               ; preds = %27, %24
  %.0 = phi i32 [ 0, %24 ], [ %50, %27 ]
  %26 = icmp slt i32 %.0, 2
  br i1 %26, label %27, label %51

27:                                               ; preds = %25
  %28 = add nsw i32 %.02, %.03
  %29 = add nsw i32 %28, %.04
  %30 = add nsw i32 %29, %.05
  %31 = add nsw i32 %30, %.06
  %32 = add nsw i32 %31, %.01
  %33 = add nsw i32 %32, %.0
  %34 = add nsw i32 %33, %0
  %35 = add nsw i32 %34, %1
  %36 = sext i32 %.02 to i64
  %37 = getelementptr inbounds [10 x [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]], [10 x [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]]* @arr1, i64 0, i64 %36
  %38 = sext i32 %.03 to i64
  %39 = getelementptr inbounds [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]], [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]* %37, i64 0, i64 %38
  %40 = sext i32 %.04 to i64
  %41 = getelementptr inbounds [3 x [4 x [5 x [6 x [2 x i32]]]]], [3 x [4 x [5 x [6 x [2 x i32]]]]]* %39, i64 0, i64 %40
  %42 = sext i32 %.05 to i64
  %43 = getelementptr inbounds [4 x [5 x [6 x [2 x i32]]]], [4 x [5 x [6 x [2 x i32]]]]* %41, i64 0, i64 %42
  %44 = sext i32 %.06 to i64
  %45 = getelementptr inbounds [5 x [6 x [2 x i32]]], [5 x [6 x [2 x i32]]]* %43, i64 0, i64 %44
  %46 = sext i32 %.01 to i64
  %47 = getelementptr inbounds [6 x [2 x i32]], [6 x [2 x i32]]* %45, i64 0, i64 %46
  %48 = sext i32 %.0 to i64
  %49 = getelementptr inbounds [2 x i32], [2 x i32]* %47, i64 0, i64 %48
  store i32 %35, i32* %49, align 4
  %50 = add nsw i32 %.0, 1
  br label %25

51:                                               ; preds = %25
  %52 = add nsw i32 %.01, 1
  br label %22

53:                                               ; preds = %22
  %54 = add nsw i32 %.06, 1
  br label %19

55:                                               ; preds = %19
  %56 = add nsw i32 %.05, 1
  br label %16

57:                                               ; preds = %16
  %58 = add nsw i32 %.04, 1
  br label %13

59:                                               ; preds = %13
  %60 = add nsw i32 %.03, 1
  br label %10

61:                                               ; preds = %10
  %62 = add nsw i32 %.02, 1
  br label %3

63:                                               ; preds = %7
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @loop2() #0 {
  br label %1

1:                                                ; preds = %50, %0
  %.0 = phi i32 [ 0, %0 ], [ %51, %50 ]
  %2 = icmp slt i32 %.0, 10
  br i1 %2, label %3, label %52

3:                                                ; preds = %1
  br label %4

4:                                                ; preds = %48, %3
  %.01 = phi i32 [ 0, %3 ], [ %49, %48 ]
  %5 = icmp slt i32 %.01, 2
  br i1 %5, label %6, label %50

6:                                                ; preds = %4
  br label %7

7:                                                ; preds = %46, %6
  %.02 = phi i32 [ 0, %6 ], [ %47, %46 ]
  %8 = icmp slt i32 %.02, 3
  br i1 %8, label %9, label %48

9:                                                ; preds = %7
  br label %10

10:                                               ; preds = %44, %9
  %.03 = phi i32 [ 0, %9 ], [ %45, %44 ]
  %11 = icmp slt i32 %.03, 2
  br i1 %11, label %12, label %46

12:                                               ; preds = %10
  br label %13

13:                                               ; preds = %42, %12
  %.04 = phi i32 [ 0, %12 ], [ %43, %42 ]
  %14 = icmp slt i32 %.04, 4
  br i1 %14, label %15, label %44

15:                                               ; preds = %13
  br label %16

16:                                               ; preds = %40, %15
  %.05 = phi i32 [ 0, %15 ], [ %41, %40 ]
  %17 = icmp slt i32 %.05, 8
  br i1 %17, label %18, label %42

18:                                               ; preds = %16
  br label %19

19:                                               ; preds = %21, %18
  %.06 = phi i32 [ 0, %18 ], [ %39, %21 ]
  %20 = icmp slt i32 %.06, 7
  br i1 %20, label %21, label %40

21:                                               ; preds = %19
  %22 = add nsw i32 %.0, %.01
  %23 = add nsw i32 %22, %.03
  %24 = add nsw i32 %23, %.06
  %25 = sext i32 %.0 to i64
  %26 = getelementptr inbounds [10 x [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]], [10 x [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]]* @arr2, i64 0, i64 %25
  %27 = sext i32 %.01 to i64
  %28 = getelementptr inbounds [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]], [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]* %26, i64 0, i64 %27
  %29 = sext i32 %.02 to i64
  %30 = getelementptr inbounds [3 x [2 x [4 x [8 x [7 x i32]]]]], [3 x [2 x [4 x [8 x [7 x i32]]]]]* %28, i64 0, i64 %29
  %31 = sext i32 %.03 to i64
  %32 = getelementptr inbounds [2 x [4 x [8 x [7 x i32]]]], [2 x [4 x [8 x [7 x i32]]]]* %30, i64 0, i64 %31
  %33 = sext i32 %.04 to i64
  %34 = getelementptr inbounds [4 x [8 x [7 x i32]]], [4 x [8 x [7 x i32]]]* %32, i64 0, i64 %33
  %35 = sext i32 %.05 to i64
  %36 = getelementptr inbounds [8 x [7 x i32]], [8 x [7 x i32]]* %34, i64 0, i64 %35
  %37 = sext i32 %.06 to i64
  %38 = getelementptr inbounds [7 x i32], [7 x i32]* %36, i64 0, i64 %37
  store i32 %24, i32* %38, align 4
  %39 = add nsw i32 %.06, 1
  br label %19

40:                                               ; preds = %19
  %41 = add nsw i32 %.05, 1
  br label %16

42:                                               ; preds = %16
  %43 = add nsw i32 %.04, 1
  br label %13

44:                                               ; preds = %13
  %45 = add nsw i32 %.03, 1
  br label %10

46:                                               ; preds = %10
  %47 = add nsw i32 %.02, 1
  br label %7

48:                                               ; preds = %7
  %49 = add nsw i32 %.01, 1
  br label %4

50:                                               ; preds = %4
  %51 = add nsw i32 %.0, 1
  br label %1

52:                                               ; preds = %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @loop3(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6) #0 {
  br label %8

8:                                                ; preds = %95, %7
  %.07 = phi i32 [ 0, %7 ], [ %92, %95 ]
  %.0 = phi i32 [ 0, %7 ], [ %.12, %95 ]
  %9 = icmp slt i32 %.07, 10
  br i1 %9, label %10, label %96

10:                                               ; preds = %8
  br label %11

11:                                               ; preds = %90, %10
  %.06 = phi i32 [ 0, %10 ], [ %87, %90 ]
  %.1 = phi i32 [ %.0, %10 ], [ %.11, %90 ]
  %12 = icmp slt i32 %.06, 100
  br i1 %12, label %13, label %91

13:                                               ; preds = %11
  br label %14

14:                                               ; preds = %85, %13
  %.05 = phi i32 [ 0, %13 ], [ %82, %85 ]
  %.2 = phi i32 [ %.1, %13 ], [ %.10, %85 ]
  %15 = icmp slt i32 %.05, 1000
  br i1 %15, label %16, label %86

16:                                               ; preds = %14
  br label %17

17:                                               ; preds = %80, %16
  %.04 = phi i32 [ 0, %16 ], [ %77, %80 ]
  %.3 = phi i32 [ %.2, %16 ], [ %.9, %80 ]
  %18 = icmp slt i32 %.04, 10000
  br i1 %18, label %19, label %81

19:                                               ; preds = %17
  br label %20

20:                                               ; preds = %75, %19
  %.03 = phi i32 [ 0, %19 ], [ %72, %75 ]
  %.4 = phi i32 [ %.3, %19 ], [ %.8, %75 ]
  %21 = icmp slt i32 %.03, 100000
  br i1 %21, label %22, label %76

22:                                               ; preds = %20
  br label %23

23:                                               ; preds = %70, %22
  %.02 = phi i32 [ 0, %22 ], [ %67, %70 ]
  %.5 = phi i32 [ %.4, %22 ], [ %.7, %70 ]
  %24 = icmp slt i32 %.02, 1000000
  br i1 %24, label %25, label %71

25:                                               ; preds = %23
  br label %26

26:                                               ; preds = %65, %25
  %.01 = phi i32 [ 0, %25 ], [ %62, %65 ]
  %.6 = phi i32 [ %.5, %25 ], [ %61, %65 ]
  %27 = icmp slt i32 %.01, 10000000
  br i1 %27, label %28, label %66

28:                                               ; preds = %26
  %29 = srem i32 %.6, 817
  %30 = sext i32 %.07 to i64
  %31 = getelementptr inbounds [10 x [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]], [10 x [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]]* @arr1, i64 0, i64 %30
  %32 = sext i32 %.06 to i64
  %33 = getelementptr inbounds [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]], [2 x [3 x [4 x [5 x [6 x [2 x i32]]]]]]* %31, i64 0, i64 %32
  %34 = sext i32 %.05 to i64
  %35 = getelementptr inbounds [3 x [4 x [5 x [6 x [2 x i32]]]]], [3 x [4 x [5 x [6 x [2 x i32]]]]]* %33, i64 0, i64 %34
  %36 = sext i32 %.04 to i64
  %37 = getelementptr inbounds [4 x [5 x [6 x [2 x i32]]]], [4 x [5 x [6 x [2 x i32]]]]* %35, i64 0, i64 %36
  %38 = sext i32 %.03 to i64
  %39 = getelementptr inbounds [5 x [6 x [2 x i32]]], [5 x [6 x [2 x i32]]]* %37, i64 0, i64 %38
  %40 = sext i32 %.02 to i64
  %41 = getelementptr inbounds [6 x [2 x i32]], [6 x [2 x i32]]* %39, i64 0, i64 %40
  %42 = sext i32 %.01 to i64
  %43 = getelementptr inbounds [2 x i32], [2 x i32]* %41, i64 0, i64 %42
  %44 = load i32, i32* %43, align 4
  %45 = add nsw i32 %29, %44
  %46 = sext i32 %.07 to i64
  %47 = getelementptr inbounds [10 x [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]], [10 x [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]]* @arr2, i64 0, i64 %46
  %48 = sext i32 %.06 to i64
  %49 = getelementptr inbounds [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]], [2 x [3 x [2 x [4 x [8 x [7 x i32]]]]]]* %47, i64 0, i64 %48
  %50 = sext i32 %.05 to i64
  %51 = getelementptr inbounds [3 x [2 x [4 x [8 x [7 x i32]]]]], [3 x [2 x [4 x [8 x [7 x i32]]]]]* %49, i64 0, i64 %50
  %52 = sext i32 %.04 to i64
  %53 = getelementptr inbounds [2 x [4 x [8 x [7 x i32]]]], [2 x [4 x [8 x [7 x i32]]]]* %51, i64 0, i64 %52
  %54 = sext i32 %.03 to i64
  %55 = getelementptr inbounds [4 x [8 x [7 x i32]]], [4 x [8 x [7 x i32]]]* %53, i64 0, i64 %54
  %56 = sext i32 %.02 to i64
  %57 = getelementptr inbounds [8 x [7 x i32]], [8 x [7 x i32]]* %55, i64 0, i64 %56
  %58 = sext i32 %.01 to i64
  %59 = getelementptr inbounds [7 x i32], [7 x i32]* %57, i64 0, i64 %58
  %60 = load i32, i32* %59, align 4
  %61 = add nsw i32 %45, %60
  %62 = add nsw i32 %.01, 1
  %63 = icmp sge i32 %62, %6
  br i1 %63, label %64, label %65

64:                                               ; preds = %28
  br label %66

65:                                               ; preds = %28
  br label %26

66:                                               ; preds = %64, %26
  %.7 = phi i32 [ %61, %64 ], [ %.6, %26 ]
  %67 = add nsw i32 %.02, 1
  %68 = icmp sge i32 %67, %5
  br i1 %68, label %69, label %70

69:                                               ; preds = %66
  br label %71

70:                                               ; preds = %66
  br label %23

71:                                               ; preds = %69, %23
  %.8 = phi i32 [ %.7, %69 ], [ %.5, %23 ]
  %72 = add nsw i32 %.03, 1
  %73 = icmp sge i32 %72, %4
  br i1 %73, label %74, label %75

74:                                               ; preds = %71
  br label %76

75:                                               ; preds = %71
  br label %20

76:                                               ; preds = %74, %20
  %.9 = phi i32 [ %.8, %74 ], [ %.4, %20 ]
  %77 = add nsw i32 %.04, 1
  %78 = icmp sge i32 %77, %3
  br i1 %78, label %79, label %80

79:                                               ; preds = %76
  br label %81

80:                                               ; preds = %76
  br label %17

81:                                               ; preds = %79, %17
  %.10 = phi i32 [ %.9, %79 ], [ %.3, %17 ]
  %82 = add nsw i32 %.05, 1
  %83 = icmp sge i32 %82, %2
  br i1 %83, label %84, label %85

84:                                               ; preds = %81
  br label %86

85:                                               ; preds = %81
  br label %14

86:                                               ; preds = %84, %14
  %.11 = phi i32 [ %.10, %84 ], [ %.2, %14 ]
  %87 = add nsw i32 %.06, 1
  %88 = icmp sge i32 %87, %1
  br i1 %88, label %89, label %90

89:                                               ; preds = %86
  br label %91

90:                                               ; preds = %86
  br label %11

91:                                               ; preds = %89, %11
  %.12 = phi i32 [ %.11, %89 ], [ %.1, %11 ]
  %92 = add nsw i32 %.07, 1
  %93 = icmp sge i32 %92, %0
  br i1 %93, label %94, label %95

94:                                               ; preds = %91
  br label %96

95:                                               ; preds = %91
  br label %8

96:                                               ; preds = %94, %8
  %.13 = phi i32 [ %.12, %94 ], [ %.0, %8 ]
  ret i32 %.13
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  %2 = call i32 (...) @getint()
  %3 = call i32 (...) @getint()
  %4 = call i32 (...) @getint()
  %5 = call i32 (...) @getint()
  %6 = call i32 (...) @getint()
  %7 = call i32 (...) @getint()
  %8 = call i32 (...) @getint()
  %9 = call i32 (...) @getint()
  call void @loop1(i32 %1, i32 %2)
  call void @loop2()
  %10 = call i32 @loop3(i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9)
  ret i32 %10
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
