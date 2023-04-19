; ModuleID = '65_color.bc'
source_filename = "65_color.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@maxn = dso_local constant i32 18, align 4
@mod = dso_local constant i32 1000000007, align 4
@dp = common dso_local global [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]] zeroinitializer, align 16
@list = common dso_local global [200 x i32] zeroinitializer, align 16
@cns = common dso_local global [20 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @equal(i32 %0, i32 %1) #0 {
  %3 = icmp eq i32 %0, %1
  Br i1 %3, label %4, label %5

4:                                                ; preds = %2
  Br label %6

5:                                                ; preds = %2
  Br label %6

6:                                                ; preds = %5, %4
  %.0 = phi i32 [ 1, %4 ], [ 0, %5 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @dfs(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5) #0 {
  %7 = sext i32 %0 to i64
  %8 = getelementptr inbounds [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]], [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]]* @dp, i64 0, i64 %7
  %9 = sext i32 %1 to i64
  %10 = getelementptr inbounds [18 x [18 x [18 x [18 x [7 x i32]]]]], [18 x [18 x [18 x [18 x [7 x i32]]]]]* %8, i64 0, i64 %9
  %11 = sext i32 %2 to i64
  %12 = getelementptr inbounds [18 x [18 x [18 x [7 x i32]]]], [18 x [18 x [18 x [7 x i32]]]]* %10, i64 0, i64 %11
  %13 = sext i32 %3 to i64
  %14 = getelementptr inbounds [18 x [18 x [7 x i32]]], [18 x [18 x [7 x i32]]]* %12, i64 0, i64 %13
  %15 = sext i32 %4 to i64
  %16 = getelementptr inbounds [18 x [7 x i32]], [18 x [7 x i32]]* %14, i64 0, i64 %15
  %17 = sext i32 %5 to i64
  %18 = getelementptr inbounds [7 x i32], [7 x i32]* %16, i64 0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = icmp ne i32 %19, -1
  Br i1 %20, label %21, label %35

21:                                               ; preds = %6
  %22 = sext i32 %0 to i64
  %23 = getelementptr inbounds [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]], [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]]* @dp, i64 0, i64 %22
  %24 = sext i32 %1 to i64
  %25 = getelementptr inbounds [18 x [18 x [18 x [18 x [7 x i32]]]]], [18 x [18 x [18 x [18 x [7 x i32]]]]]* %23, i64 0, i64 %24
  %26 = sext i32 %2 to i64
  %27 = getelementptr inbounds [18 x [18 x [18 x [7 x i32]]]], [18 x [18 x [18 x [7 x i32]]]]* %25, i64 0, i64 %26
  %28 = sext i32 %3 to i64
  %29 = getelementptr inbounds [18 x [18 x [7 x i32]]], [18 x [18 x [7 x i32]]]* %27, i64 0, i64 %28
  %30 = sext i32 %4 to i64
  %31 = getelementptr inbounds [18 x [7 x i32]], [18 x [7 x i32]]* %29, i64 0, i64 %30
  %32 = sext i32 %5 to i64
  %33 = getelementptr inbounds [7 x i32], [7 x i32]* %31, i64 0, i64 %32
  %34 = load i32, i32* %33, align 4
  Br label %121

35:                                               ; preds = %6
  %36 = add nsw i32 %0, %1
  %37 = add nsw i32 %36, %2
  %38 = add nsw i32 %37, %3
  %39 = add nsw i32 %38, %4
  %40 = icmp eq i32 %39, 0
  Br i1 %40, label %41, label %42

41:                                               ; preds = %35
  Br label %121

42:                                               ; preds = %35
  %43 = icmp ne i32 %0, 0
  Br i1 %43, label %44, label %52

44:                                               ; preds = %42
  %45 = call i32 @equal(i32 %5, i32 2)
  %46 = sub nsw i32 %0, %45
  %47 = sub nsw i32 %0, 1
  %48 = call i32 @dfs(i32 %47, i32 %1, i32 %2, i32 %3, i32 %4, i32 1)
  %49 = mul nsw i32 %46, %48
  %50 = add nsw i32 0, %49
  %51 = srem i32 %50, 1000000007
  Br label %52

52:                                               ; preds = %44, %42
  %.01 = phi i32 [ %51, %44 ], [ 0, %42 ]
  %53 = icmp ne i32 %1, 0
  Br i1 %53, label %54, label %63

54:                                               ; preds = %52
  %55 = call i32 @equal(i32 %5, i32 3)
  %56 = sub nsw i32 %1, %55
  %57 = add nsw i32 %0, 1
  %58 = sub nsw i32 %1, 1
  %59 = call i32 @dfs(i32 %57, i32 %58, i32 %2, i32 %3, i32 %4, i32 2)
  %60 = mul nsw i32 %56, %59
  %61 = add nsw i32 %.01, %60
  %62 = srem i32 %61, 1000000007
  Br label %63

63:                                               ; preds = %54, %52
  %.1 = phi i32 [ %62, %54 ], [ %.01, %52 ]
  %64 = icmp ne i32 %2, 0
  Br i1 %64, label %65, label %74

65:                                               ; preds = %63
  %66 = call i32 @equal(i32 %5, i32 4)
  %67 = sub nsw i32 %2, %66
  %68 = add nsw i32 %1, 1
  %69 = sub nsw i32 %2, 1
  %70 = call i32 @dfs(i32 %0, i32 %68, i32 %69, i32 %3, i32 %4, i32 3)
  %71 = mul nsw i32 %67, %70
  %72 = add nsw i32 %.1, %71
  %73 = srem i32 %72, 1000000007
  Br label %74

74:                                               ; preds = %65, %63
  %.2 = phi i32 [ %73, %65 ], [ %.1, %63 ]
  %75 = icmp ne i32 %3, 0
  Br i1 %75, label %76, label %85

76:                                               ; preds = %74
  %77 = call i32 @equal(i32 %5, i32 5)
  %78 = sub nsw i32 %3, %77
  %79 = add nsw i32 %2, 1
  %80 = sub nsw i32 %3, 1
  %81 = call i32 @dfs(i32 %0, i32 %1, i32 %79, i32 %80, i32 %4, i32 4)
  %82 = mul nsw i32 %78, %81
  %83 = add nsw i32 %.2, %82
  %84 = srem i32 %83, 1000000007
  Br label %85

85:                                               ; preds = %76, %74
  %.3 = phi i32 [ %84, %76 ], [ %.2, %74 ]
  %86 = icmp ne i32 %4, 0
  Br i1 %86, label %87, label %94

87:                                               ; preds = %85
  %88 = add nsw i32 %3, 1
  %89 = sub nsw i32 %4, 1
  %90 = call i32 @dfs(i32 %0, i32 %1, i32 %2, i32 %88, i32 %89, i32 5)
  %91 = mul nsw i32 %4, %90
  %92 = add nsw i32 %.3, %91
  %93 = srem i32 %92, 1000000007
  Br label %94

94:                                               ; preds = %87, %85
  %.4 = phi i32 [ %93, %87 ], [ %.3, %85 ]
  %95 = srem i32 %.4, 1000000007
  %96 = sext i32 %0 to i64
  %97 = getelementptr inbounds [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]], [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]]* @dp, i64 0, i64 %96
  %98 = sext i32 %1 to i64
  %99 = getelementptr inbounds [18 x [18 x [18 x [18 x [7 x i32]]]]], [18 x [18 x [18 x [18 x [7 x i32]]]]]* %97, i64 0, i64 %98
  %100 = sext i32 %2 to i64
  %101 = getelementptr inbounds [18 x [18 x [18 x [7 x i32]]]], [18 x [18 x [18 x [7 x i32]]]]* %99, i64 0, i64 %100
  %102 = sext i32 %3 to i64
  %103 = getelementptr inbounds [18 x [18 x [7 x i32]]], [18 x [18 x [7 x i32]]]* %101, i64 0, i64 %102
  %104 = sext i32 %4 to i64
  %105 = getelementptr inbounds [18 x [7 x i32]], [18 x [7 x i32]]* %103, i64 0, i64 %104
  %106 = sext i32 %5 to i64
  %107 = getelementptr inbounds [7 x i32], [7 x i32]* %105, i64 0, i64 %106
  store i32 %95, i32* %107, align 4
  %108 = sext i32 %0 to i64
  %109 = getelementptr inbounds [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]], [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]]* @dp, i64 0, i64 %108
  %110 = sext i32 %1 to i64
  %111 = getelementptr inbounds [18 x [18 x [18 x [18 x [7 x i32]]]]], [18 x [18 x [18 x [18 x [7 x i32]]]]]* %109, i64 0, i64 %110
  %112 = sext i32 %2 to i64
  %113 = getelementptr inbounds [18 x [18 x [18 x [7 x i32]]]], [18 x [18 x [18 x [7 x i32]]]]* %111, i64 0, i64 %112
  %114 = sext i32 %3 to i64
  %115 = getelementptr inbounds [18 x [18 x [7 x i32]]], [18 x [18 x [7 x i32]]]* %113, i64 0, i64 %114
  %116 = sext i32 %4 to i64
  %117 = getelementptr inbounds [18 x [7 x i32]], [18 x [7 x i32]]* %115, i64 0, i64 %116
  %118 = sext i32 %5 to i64
  %119 = getelementptr inbounds [7 x i32], [7 x i32]* %117, i64 0, i64 %118
  %120 = load i32, i32* %119, align 4
  Br label %121

121:                                              ; preds = %94, %41, %21
  %.0 = phi i32 [ %34, %21 ], [ 1, %41 ], [ %120, %94 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  Br label %2

2:                                                ; preds = %41, %0
  %.02 = phi i32 [ 0, %0 ], [ %42, %41 ]
  %3 = icmp slt i32 %.02, 18
  Br i1 %3, label %4, label %43

4:                                                ; preds = %2
  Br label %5

5:                                                ; preds = %39, %4
  %.03 = phi i32 [ 0, %4 ], [ %40, %39 ]
  %6 = icmp slt i32 %.03, 18
  Br i1 %6, label %7, label %41

7:                                                ; preds = %5
  Br label %8

8:                                                ; preds = %37, %7
  %.04 = phi i32 [ 0, %7 ], [ %38, %37 ]
  %9 = icmp slt i32 %.04, 18
  Br i1 %9, label %10, label %39

10:                                               ; preds = %8
  Br label %11

11:                                               ; preds = %35, %10
  %.05 = phi i32 [ 0, %10 ], [ %36, %35 ]
  %12 = icmp slt i32 %.05, 18
  Br i1 %12, label %13, label %37

13:                                               ; preds = %11
  Br label %14

14:                                               ; preds = %33, %13
  %.01 = phi i32 [ 0, %13 ], [ %34, %33 ]
  %15 = icmp slt i32 %.01, 18
  Br i1 %15, label %16, label %35

16:                                               ; preds = %14
  Br label %17

17:                                               ; preds = %19, %16
  %.0 = phi i32 [ 0, %16 ], [ %32, %19 ]
  %18 = icmp slt i32 %.0, 7
  Br i1 %18, label %19, label %33

19:                                               ; preds = %17
  %20 = sext i32 %.02 to i64
  %21 = getelementptr inbounds [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]], [18 x [18 x [18 x [18 x [18 x [7 x i32]]]]]]* @dp, i64 0, i64 %20
  %22 = sext i32 %.03 to i64
  %23 = getelementptr inbounds [18 x [18 x [18 x [18 x [7 x i32]]]]], [18 x [18 x [18 x [18 x [7 x i32]]]]]* %21, i64 0, i64 %22
  %24 = sext i32 %.04 to i64
  %25 = getelementptr inbounds [18 x [18 x [18 x [7 x i32]]]], [18 x [18 x [18 x [7 x i32]]]]* %23, i64 0, i64 %24
  %26 = sext i32 %.05 to i64
  %27 = getelementptr inbounds [18 x [18 x [7 x i32]]], [18 x [18 x [7 x i32]]]* %25, i64 0, i64 %26
  %28 = sext i32 %.01 to i64
  %29 = getelementptr inbounds [18 x [7 x i32]], [18 x [7 x i32]]* %27, i64 0, i64 %28
  %30 = sext i32 %.0 to i64
  %31 = getelementptr inbounds [7 x i32], [7 x i32]* %29, i64 0, i64 %30
  store i32 -1, i32* %31, align 4
  %32 = add nsw i32 %.0, 1
  Br label %17

33:                                               ; preds = %17
  %34 = add nsw i32 %.01, 1
  Br label %14

35:                                               ; preds = %14
  %36 = add nsw i32 %.05, 1
  Br label %11

37:                                               ; preds = %11
  %38 = add nsw i32 %.04, 1
  Br label %8

39:                                               ; preds = %8
  %40 = add nsw i32 %.03, 1
  Br label %5

41:                                               ; preds = %5
  %42 = add nsw i32 %.02, 1
  Br label %2

43:                                               ; preds = %2
  Br label %44

44:                                               ; preds = %46, %43
  %.1 = phi i32 [ 0, %43 ], [ %62, %46 ]
  %45 = icmp slt i32 %.1, %1
  Br i1 %45, label %46, label %63

46:                                               ; preds = %44
  %47 = call i32 (...) @getint()
  %48 = sext i32 %.1 to i64
  %49 = getelementptr inbounds [200 x i32], [200 x i32]* @list, i64 0, i64 %48
  store i32 %47, i32* %49, align 4
  %50 = sext i32 %.1 to i64
  %51 = getelementptr inbounds [200 x i32], [200 x i32]* @list, i64 0, i64 %50
  %52 = load i32, i32* %51, align 4
  %53 = sext i32 %52 to i64
  %54 = getelementptr inbounds [20 x i32], [20 x i32]* @cns, i64 0, i64 %53
  %55 = load i32, i32* %54, align 4
  %56 = add nsw i32 %55, 1
  %57 = sext i32 %.1 to i64
  %58 = getelementptr inbounds [200 x i32], [200 x i32]* @list, i64 0, i64 %57
  %59 = load i32, i32* %58, align 4
  %60 = sext i32 %59 to i64
  %61 = getelementptr inbounds [20 x i32], [20 x i32]* @cns, i64 0, i64 %60
  store i32 %56, i32* %61, align 4
  %62 = add nsw i32 %.1, 1
  Br label %44

63:                                               ; preds = %44
  %64 = load i32, i32* getelementptr inbounds ([20 x i32], [20 x i32]* @cns, i64 0, i64 1), align 4
  %65 = load i32, i32* getelementptr inbounds ([20 x i32], [20 x i32]* @cns, i64 0, i64 2), align 8
  %66 = load i32, i32* getelementptr inbounds ([20 x i32], [20 x i32]* @cns, i64 0, i64 3), align 4
  %67 = load i32, i32* getelementptr inbounds ([20 x i32], [20 x i32]* @cns, i64 0, i64 4), align 16
  %68 = load i32, i32* getelementptr inbounds ([20 x i32], [20 x i32]* @cns, i64 0, i64 5), align 4
  %69 = call i32 @dfs(i32 %64, i32 %65, i32 %66, i32 %67, i32 %68, i32 0)
  %70 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %69)
  ret i32 %69
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
