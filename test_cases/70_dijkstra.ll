; ModuleID = '70_dijkstra.bc'
source_filename = "70_dijkstra.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@INF = dso_local constant i32 65535, align 4
@n = dso_local global i32 0, align 4
@e = dso_local global [16 x [16 x i32]] zeroinitializer, align 16
@dis = dso_local global [16 x i32] zeroinitializer, align 16
@book = dso_local global [16 x i32] zeroinitializer, align 16
@m = dso_local global i32 0, align 4
@v1 = dso_local global i32 0, align 4
@v2 = dso_local global i32 0, align 4
@w = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local void @Dijkstra() #0 {
  br label %1

1:                                                ; preds = %4, %0
  %.0 = phi i32 [ 1, %0 ], [ %12, %4 ]
  %2 = load i32, i32* @n, align 4
  %3 = icmp sle i32 %.0, %2
  br i1 %3, label %4, label %13

4:                                                ; preds = %1
  %5 = sext i32 %.0 to i64
  %6 = getelementptr inbounds [16 x i32], [16 x i32]* getelementptr inbounds ([16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 1), i64 0, i64 %5
  %7 = load i32, i32* %6, align 4
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %8
  store i32 %7, i32* %9, align 4
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [16 x i32], [16 x i32]* @book, i64 0, i64 %10
  store i32 0, i32* %11, align 4
  %12 = add nsw i32 %.0, 1
  br label %1

13:                                               ; preds = %1
  store i32 1, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @book, i64 0, i64 1), align 4
  br label %14

14:                                               ; preds = %80, %13
  %.1 = phi i32 [ 1, %13 ], [ %81, %80 ]
  %15 = load i32, i32* @n, align 4
  %16 = sub nsw i32 %15, 1
  %17 = icmp sle i32 %.1, %16
  br i1 %17, label %18, label %82

18:                                               ; preds = %14
  br label %19

19:                                               ; preds = %36, %18
  %.06 = phi i32 [ 1, %18 ], [ %37, %36 ]
  %.04 = phi i32 [ 0, %18 ], [ %.15, %36 ]
  %.02 = phi i32 [ 65535, %18 ], [ %.13, %36 ]
  %20 = load i32, i32* @n, align 4
  %21 = icmp sle i32 %.06, %20
  br i1 %21, label %22, label %38

22:                                               ; preds = %19
  %23 = sext i32 %.06 to i64
  %24 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %23
  %25 = load i32, i32* %24, align 4
  %26 = icmp sgt i32 %.02, %25
  br i1 %26, label %27, label %36

27:                                               ; preds = %22
  %28 = sext i32 %.06 to i64
  %29 = getelementptr inbounds [16 x i32], [16 x i32]* @book, i64 0, i64 %28
  %30 = load i32, i32* %29, align 4
  %31 = icmp eq i32 %30, 0
  br i1 %31, label %32, label %36

32:                                               ; preds = %27
  %33 = sext i32 %.06 to i64
  %34 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %33
  %35 = load i32, i32* %34, align 4
  br label %36

36:                                               ; preds = %32, %27, %22
  %.15 = phi i32 [ %.06, %32 ], [ %.04, %27 ], [ %.04, %22 ]
  %.13 = phi i32 [ %35, %32 ], [ %.02, %27 ], [ %.02, %22 ]
  %37 = add nsw i32 %.06, 1
  br label %19

38:                                               ; preds = %19
  %39 = sext i32 %.04 to i64
  %40 = getelementptr inbounds [16 x i32], [16 x i32]* @book, i64 0, i64 %39
  store i32 1, i32* %40, align 4
  br label %41

41:                                               ; preds = %78, %38
  %.01 = phi i32 [ 1, %38 ], [ %79, %78 ]
  %42 = load i32, i32* @n, align 4
  %43 = icmp sle i32 %.01, %42
  br i1 %43, label %44, label %80

44:                                               ; preds = %41
  %45 = sext i32 %.04 to i64
  %46 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %45
  %47 = sext i32 %.01 to i64
  %48 = getelementptr inbounds [16 x i32], [16 x i32]* %46, i64 0, i64 %47
  %49 = load i32, i32* %48, align 4
  %50 = icmp slt i32 %49, 65535
  br i1 %50, label %51, label %78

51:                                               ; preds = %44
  %52 = sext i32 %.01 to i64
  %53 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %52
  %54 = load i32, i32* %53, align 4
  %55 = sext i32 %.04 to i64
  %56 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %55
  %57 = load i32, i32* %56, align 4
  %58 = sext i32 %.04 to i64
  %59 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %58
  %60 = sext i32 %.01 to i64
  %61 = getelementptr inbounds [16 x i32], [16 x i32]* %59, i64 0, i64 %60
  %62 = load i32, i32* %61, align 4
  %63 = add nsw i32 %57, %62
  %64 = icmp sgt i32 %54, %63
  br i1 %64, label %65, label %77

65:                                               ; preds = %51
  %66 = sext i32 %.04 to i64
  %67 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %66
  %68 = load i32, i32* %67, align 4
  %69 = sext i32 %.04 to i64
  %70 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %69
  %71 = sext i32 %.01 to i64
  %72 = getelementptr inbounds [16 x i32], [16 x i32]* %70, i64 0, i64 %71
  %73 = load i32, i32* %72, align 4
  %74 = add nsw i32 %68, %73
  %75 = sext i32 %.01 to i64
  %76 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %75
  store i32 %74, i32* %76, align 4
  br label %77

77:                                               ; preds = %65, %51
  br label %78

78:                                               ; preds = %77, %44
  %79 = add nsw i32 %.01, 1
  br label %41

80:                                               ; preds = %41
  %81 = add nsw i32 %.1, 1
  br label %14

82:                                               ; preds = %14
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  store i32 %1, i32* @n, align 4
  %2 = call i32 (...) @getint()
  store i32 %2, i32* @m, align 4
  br label %3

3:                                                ; preds = %24, %0
  %.01 = phi i32 [ 1, %0 ], [ %25, %24 ]
  %4 = load i32, i32* @n, align 4
  %5 = icmp sle i32 %.01, %4
  br i1 %5, label %6, label %26

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %22, %6
  %.0 = phi i32 [ 1, %6 ], [ %23, %22 ]
  %8 = load i32, i32* @n, align 4
  %9 = icmp sle i32 %.0, %8
  br i1 %9, label %10, label %24

10:                                               ; preds = %7
  %11 = icmp eq i32 %.01, %.0
  br i1 %11, label %12, label %17

12:                                               ; preds = %10
  %13 = sext i32 %.01 to i64
  %14 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %13
  %15 = sext i32 %.0 to i64
  %16 = getelementptr inbounds [16 x i32], [16 x i32]* %14, i64 0, i64 %15
  store i32 0, i32* %16, align 4
  br label %22

17:                                               ; preds = %10
  %18 = sext i32 %.01 to i64
  %19 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %18
  %20 = sext i32 %.0 to i64
  %21 = getelementptr inbounds [16 x i32], [16 x i32]* %19, i64 0, i64 %20
  store i32 65535, i32* %21, align 4
  br label %22

22:                                               ; preds = %17, %12
  %23 = add nsw i32 %.0, 1
  br label %7

24:                                               ; preds = %7
  %25 = add nsw i32 %.01, 1
  br label %3

26:                                               ; preds = %3
  br label %27

27:                                               ; preds = %30, %26
  %.1 = phi i32 [ 1, %26 ], [ %38, %30 ]
  %28 = load i32, i32* @m, align 4
  %29 = icmp sle i32 %.1, %28
  br i1 %29, label %30, label %39

30:                                               ; preds = %27
  %31 = call i32 (...) @getint()
  %32 = call i32 (...) @getint()
  %33 = call i32 (...) @getint()
  %34 = sext i32 %31 to i64
  %35 = getelementptr inbounds [16 x [16 x i32]], [16 x [16 x i32]]* @e, i64 0, i64 %34
  %36 = sext i32 %32 to i64
  %37 = getelementptr inbounds [16 x i32], [16 x i32]* %35, i64 0, i64 %36
  store i32 %33, i32* %37, align 4
  %38 = add nsw i32 %.1, 1
  br label %27

39:                                               ; preds = %27
  call void @Dijkstra()
  br label %40

40:                                               ; preds = %43, %39
  %.2 = phi i32 [ 1, %39 ], [ %49, %43 ]
  %41 = load i32, i32* @n, align 4
  %42 = icmp sle i32 %.2, %41
  br i1 %42, label %43, label %50

43:                                               ; preds = %40
  %44 = sext i32 %.2 to i64
  %45 = getelementptr inbounds [16 x i32], [16 x i32]* @dis, i64 0, i64 %44
  %46 = load i32, i32* %45, align 4
  %47 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %46)
  %48 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %49 = add nsw i32 %.2, 1
  br label %40

50:                                               ; preds = %40
  %51 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
