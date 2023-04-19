; ModuleID = '75_max_flow.bc'
source_filename = "75_max_flow.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@INF = dso_local constant i32 1879048192, align 4
@to = common dso_local global [10 x [10 x i32]] zeroinitializer, align 16
@size = common dso_local global [10 x i32] zeroinitializer, align 16
@cap = common dso_local global [10 x [10 x i32]] zeroinitializer, align 16
@rev = common dso_local global [10 x [10 x i32]] zeroinitializer, align 16
@used = common dso_local global [10 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local void @my_memset(i32* %0, i32 %1, i32 %2) #0 {
  br label %4

4:                                                ; preds = %6, %3
  %.0 = phi i32 [ 0, %3 ], [ %9, %6 ]
  %5 = icmp slt i32 %.0, %2
  br i1 %5, label %6, label %10

6:                                                ; preds = %4
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds i32, i32* %0, i64 %7
  store i32 %1, i32* %8, align 4
  %9 = add nsw i32 %.0, 1
  br label %4

10:                                               ; preds = %4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @add_node(i32 %0, i32 %1, i32 %2) #0 {
  %4 = sext i32 %0 to i64
  %5 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %4
  %6 = sext i32 %0 to i64
  %7 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %6
  %8 = load i32, i32* %7, align 4
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds [10 x i32], [10 x i32]* %5, i64 0, i64 %9
  store i32 %1, i32* %10, align 4
  %11 = sext i32 %0 to i64
  %12 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %11
  %13 = sext i32 %0 to i64
  %14 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %13
  %15 = load i32, i32* %14, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10 x i32], [10 x i32]* %12, i64 0, i64 %16
  store i32 %2, i32* %17, align 4
  %18 = sext i32 %1 to i64
  %19 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %18
  %20 = load i32, i32* %19, align 4
  %21 = sext i32 %0 to i64
  %22 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @rev, i64 0, i64 %21
  %23 = sext i32 %0 to i64
  %24 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = sext i32 %25 to i64
  %27 = getelementptr inbounds [10 x i32], [10 x i32]* %22, i64 0, i64 %26
  store i32 %20, i32* %27, align 4
  %28 = sext i32 %1 to i64
  %29 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %28
  %30 = sext i32 %1 to i64
  %31 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %30
  %32 = load i32, i32* %31, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [10 x i32], [10 x i32]* %29, i64 0, i64 %33
  store i32 %0, i32* %34, align 4
  %35 = sext i32 %1 to i64
  %36 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %35
  %37 = sext i32 %1 to i64
  %38 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %37
  %39 = load i32, i32* %38, align 4
  %40 = sext i32 %39 to i64
  %41 = getelementptr inbounds [10 x i32], [10 x i32]* %36, i64 0, i64 %40
  store i32 0, i32* %41, align 4
  %42 = sext i32 %0 to i64
  %43 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %42
  %44 = load i32, i32* %43, align 4
  %45 = sext i32 %1 to i64
  %46 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @rev, i64 0, i64 %45
  %47 = sext i32 %1 to i64
  %48 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %47
  %49 = load i32, i32* %48, align 4
  %50 = sext i32 %49 to i64
  %51 = getelementptr inbounds [10 x i32], [10 x i32]* %46, i64 0, i64 %50
  store i32 %44, i32* %51, align 4
  %52 = sext i32 %0 to i64
  %53 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %52
  %54 = load i32, i32* %53, align 4
  %55 = add nsw i32 %54, 1
  %56 = sext i32 %0 to i64
  %57 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %56
  store i32 %55, i32* %57, align 4
  %58 = sext i32 %1 to i64
  %59 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %58
  %60 = load i32, i32* %59, align 4
  %61 = add nsw i32 %60, 1
  %62 = sext i32 %1 to i64
  %63 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %62
  store i32 %61, i32* %63, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @dfs(i32 %0, i32 %1, i32 %2) #0 {
  %4 = icmp eq i32 %0, %1
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %101

6:                                                ; preds = %3
  %7 = sext i32 %0 to i64
  %8 = getelementptr inbounds [10 x i32], [10 x i32]* @used, i64 0, i64 %7
  store i32 1, i32* %8, align 4
  br label %9

9:                                                ; preds = %98, %33, %24, %6
  %.02 = phi i32 [ 0, %6 ], [ %25, %24 ], [ %34, %33 ], [ %99, %98 ]
  %10 = sext i32 %0 to i64
  %11 = getelementptr inbounds [10 x i32], [10 x i32]* @size, i64 0, i64 %10
  %12 = load i32, i32* %11, align 4
  %13 = icmp slt i32 %.02, %12
  br i1 %13, label %14, label %100

14:                                               ; preds = %9
  %15 = sext i32 %0 to i64
  %16 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %15
  %17 = sext i32 %.02 to i64
  %18 = getelementptr inbounds [10 x i32], [10 x i32]* %16, i64 0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = sext i32 %19 to i64
  %21 = getelementptr inbounds [10 x i32], [10 x i32]* @used, i64 0, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = icmp ne i32 %22, 0
  br i1 %23, label %24, label %26

24:                                               ; preds = %14
  %25 = add nsw i32 %.02, 1
  br label %9

26:                                               ; preds = %14
  %27 = sext i32 %0 to i64
  %28 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %27
  %29 = sext i32 %.02 to i64
  %30 = getelementptr inbounds [10 x i32], [10 x i32]* %28, i64 0, i64 %29
  %31 = load i32, i32* %30, align 4
  %32 = icmp sle i32 %31, 0
  br i1 %32, label %33, label %35

33:                                               ; preds = %26
  %34 = add nsw i32 %.02, 1
  br label %9

35:                                               ; preds = %26
  %36 = sext i32 %0 to i64
  %37 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %36
  %38 = sext i32 %.02 to i64
  %39 = getelementptr inbounds [10 x i32], [10 x i32]* %37, i64 0, i64 %38
  %40 = load i32, i32* %39, align 4
  %41 = icmp slt i32 %2, %40
  br i1 %41, label %42, label %43

42:                                               ; preds = %35
  br label %49

43:                                               ; preds = %35
  %44 = sext i32 %0 to i64
  %45 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %44
  %46 = sext i32 %.02 to i64
  %47 = getelementptr inbounds [10 x i32], [10 x i32]* %45, i64 0, i64 %46
  %48 = load i32, i32* %47, align 4
  br label %49

49:                                               ; preds = %43, %42
  %.01 = phi i32 [ %2, %42 ], [ %48, %43 ]
  %50 = sext i32 %0 to i64
  %51 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %50
  %52 = sext i32 %.02 to i64
  %53 = getelementptr inbounds [10 x i32], [10 x i32]* %51, i64 0, i64 %52
  %54 = load i32, i32* %53, align 4
  %55 = call i32 @dfs(i32 %54, i32 %1, i32 %.01)
  %56 = icmp sgt i32 %55, 0
  br i1 %56, label %57, label %98

57:                                               ; preds = %49
  %58 = sext i32 %0 to i64
  %59 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %58
  %60 = sext i32 %.02 to i64
  %61 = getelementptr inbounds [10 x i32], [10 x i32]* %59, i64 0, i64 %60
  %62 = load i32, i32* %61, align 4
  %63 = sub nsw i32 %62, %55
  %64 = sext i32 %0 to i64
  %65 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %64
  %66 = sext i32 %.02 to i64
  %67 = getelementptr inbounds [10 x i32], [10 x i32]* %65, i64 0, i64 %66
  store i32 %63, i32* %67, align 4
  %68 = sext i32 %0 to i64
  %69 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %68
  %70 = sext i32 %.02 to i64
  %71 = getelementptr inbounds [10 x i32], [10 x i32]* %69, i64 0, i64 %70
  %72 = load i32, i32* %71, align 4
  %73 = sext i32 %72 to i64
  %74 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %73
  %75 = sext i32 %0 to i64
  %76 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @rev, i64 0, i64 %75
  %77 = sext i32 %.02 to i64
  %78 = getelementptr inbounds [10 x i32], [10 x i32]* %76, i64 0, i64 %77
  %79 = load i32, i32* %78, align 4
  %80 = sext i32 %79 to i64
  %81 = getelementptr inbounds [10 x i32], [10 x i32]* %74, i64 0, i64 %80
  %82 = load i32, i32* %81, align 4
  %83 = add nsw i32 %82, %55
  %84 = sext i32 %0 to i64
  %85 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @to, i64 0, i64 %84
  %86 = sext i32 %.02 to i64
  %87 = getelementptr inbounds [10 x i32], [10 x i32]* %85, i64 0, i64 %86
  %88 = load i32, i32* %87, align 4
  %89 = sext i32 %88 to i64
  %90 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @cap, i64 0, i64 %89
  %91 = sext i32 %0 to i64
  %92 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* @rev, i64 0, i64 %91
  %93 = sext i32 %.02 to i64
  %94 = getelementptr inbounds [10 x i32], [10 x i32]* %92, i64 0, i64 %93
  %95 = load i32, i32* %94, align 4
  %96 = sext i32 %95 to i64
  %97 = getelementptr inbounds [10 x i32], [10 x i32]* %90, i64 0, i64 %96
  store i32 %83, i32* %97, align 4
  br label %101

98:                                               ; preds = %49
  %99 = add nsw i32 %.02, 1
  br label %9

100:                                              ; preds = %9
  br label %101

101:                                              ; preds = %100, %57, %5
  %.0 = phi i32 [ %2, %5 ], [ %55, %57 ], [ 0, %100 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @max_flow(i32 %0, i32 %1) #0 {
  br label %3

3:                                                ; preds = %7, %2
  %.0 = phi i32 [ 0, %2 ], [ %8, %7 ]
  call void @my_memset(i32* getelementptr inbounds ([10 x i32], [10 x i32]* @used, i64 0, i64 0), i32 0, i32 10)
  %4 = call i32 @dfs(i32 %0, i32 %1, i32 1879048192)
  %5 = icmp eq i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  ret i32 %.0

7:                                                ; preds = %3
  %8 = add nsw i32 %.0, %4
  br label %3
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  %2 = call i32 (...) @getint()
  call void @my_memset(i32* getelementptr inbounds ([10 x i32], [10 x i32]* @size, i64 0, i64 0), i32 0, i32 10)
  br label %3

3:                                                ; preds = %5, %0
  %.0 = phi i32 [ %2, %0 ], [ %9, %5 ]
  %4 = icmp sgt i32 %.0, 0
  br i1 %4, label %5, label %10

5:                                                ; preds = %3
  %6 = call i32 (...) @getint()
  %7 = call i32 (...) @getint()
  %8 = call i32 (...) @getint()
  call void @add_node(i32 %6, i32 %7, i32 %8)
  %9 = sub nsw i32 %.0, 1
  br label %3

10:                                               ; preds = %3
  %11 = call i32 @max_flow(i32 1, i32 %1)
  %12 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %11)
  %13 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
