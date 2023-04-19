; ModuleID = '61_sort_test7.bc'
source_filename = "61_sort_test7.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@buf = common dso_local global [2 x [100 x i32]] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local void @merge_sort(i32 %0, i32 %1) #0 {
  %3 = add nsw i32 %0, 1
  %4 = icmp sge i32 %3, %1
  Br i1 %4, label %5, label %6

5:                                                ; preds = %2
  Br label %71

6:                                                ; preds = %2
  %7 = add nsw i32 %0, %1
  %8 = sdiv i32 %7, 2
  call void @merge_sort(i32 %0, i32 %8)
  call void @merge_sort(i32 %8, i32 %1)
  Br label %9

9:                                                ; preds = %37, %6
  %.05 = phi i32 [ %0, %6 ], [ %.16, %37 ]
  %.02 = phi i32 [ %8, %6 ], [ %.13, %37 ]
  %.01 = phi i32 [ %0, %6 ], [ %38, %37 ]
  %10 = icmp slt i32 %.05, %8
  Br i1 %10, label %11, label %13

11:                                               ; preds = %9
  %12 = icmp slt i32 %.02, %1
  Br label %13

13:                                               ; preds = %11, %9
  %14 = phi i1 [ false, %9 ], [ %12, %11 ]
  Br i1 %14, label %15, label %39

15:                                               ; preds = %13
  %16 = sext i32 %.05 to i64
  %17 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %16
  %18 = load i32, i32* %17, align 4
  %19 = sext i32 %.02 to i64
  %20 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %19
  %21 = load i32, i32* %20, align 4
  %22 = icmp slt i32 %18, %21
  Br i1 %22, label %23, label %30

23:                                               ; preds = %15
  %24 = sext i32 %.05 to i64
  %25 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = sext i32 %.01 to i64
  %28 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 1), i64 0, i64 %27
  store i32 %26, i32* %28, align 4
  %29 = add nsw i32 %.05, 1
  Br label %37

30:                                               ; preds = %15
  %31 = sext i32 %.02 to i64
  %32 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %31
  %33 = load i32, i32* %32, align 4
  %34 = sext i32 %.01 to i64
  %35 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 1), i64 0, i64 %34
  store i32 %33, i32* %35, align 4
  %36 = add nsw i32 %.02, 1
  Br label %37

37:                                               ; preds = %30, %23
  %.16 = phi i32 [ %29, %23 ], [ %.05, %30 ]
  %.13 = phi i32 [ %.02, %23 ], [ %36, %30 ]
  %38 = add nsw i32 %.01, 1
  Br label %9

39:                                               ; preds = %13
  Br label %40

40:                                               ; preds = %42, %39
  %.27 = phi i32 [ %.05, %39 ], [ %48, %42 ]
  %.1 = phi i32 [ %.01, %39 ], [ %49, %42 ]
  %41 = icmp slt i32 %.27, %8
  Br i1 %41, label %42, label %50

42:                                               ; preds = %40
  %43 = sext i32 %.27 to i64
  %44 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %43
  %45 = load i32, i32* %44, align 4
  %46 = sext i32 %.1 to i64
  %47 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 1), i64 0, i64 %46
  store i32 %45, i32* %47, align 4
  %48 = add nsw i32 %.27, 1
  %49 = add nsw i32 %.1, 1
  Br label %40

50:                                               ; preds = %40
  Br label %51

51:                                               ; preds = %53, %50
  %.24 = phi i32 [ %.02, %50 ], [ %59, %53 ]
  %.2 = phi i32 [ %.1, %50 ], [ %60, %53 ]
  %52 = icmp slt i32 %.24, %1
  Br i1 %52, label %53, label %61

53:                                               ; preds = %51
  %54 = sext i32 %.24 to i64
  %55 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %54
  %56 = load i32, i32* %55, align 4
  %57 = sext i32 %.2 to i64
  %58 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 1), i64 0, i64 %57
  store i32 %56, i32* %58, align 4
  %59 = add nsw i32 %.24, 1
  %60 = add nsw i32 %.2, 1
  Br label %51

61:                                               ; preds = %51
  Br label %62

62:                                               ; preds = %64, %61
  %.0 = phi i32 [ %0, %61 ], [ %70, %64 ]
  %63 = icmp slt i32 %.0, %1
  Br i1 %63, label %64, label %71

64:                                               ; preds = %62
  %65 = sext i32 %.0 to i64
  %66 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 1), i64 0, i64 %65
  %67 = load i32, i32* %66, align 4
  %68 = sext i32 %.0 to i64
  %69 = getelementptr inbounds [100 x i32], [100 x i32]* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0), i64 0, i64 %68
  store i32 %67, i32* %69, align 4
  %70 = add nsw i32 %.0, 1
  Br label %62

71:                                               ; preds = %62, %5
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (i32*, ...) bitcast (i32 (...)* @getarray to i32 (i32*, ...)*)(i32* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0, i64 0))
  call void @merge_sort(i32 0, i32 %1)
  %2 = call i32 (i32, i32*, ...) bitcast (i32 (...)* @putarray to i32 (i32, i32*, ...)*)(i32 %1, i32* getelementptr inbounds ([2 x [100 x i32]], [2 x [100 x i32]]* @buf, i64 0, i64 0, i64 0))
  ret i32 0
}

declare dso_local i32 @getarray(...) #1

declare dso_local i32 @putarray(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
