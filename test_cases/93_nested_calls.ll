; ModuleID = '93_nested_calls.bc'
source_filename = "93_nested_calls.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func1(i32 %0, i32 %1, i32 %2) #0 {
  %4 = icmp eq i32 %2, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = mul nsw i32 %0, %1
  br label %10

7:                                                ; preds = %3
  %8 = sub nsw i32 %1, %2
  %9 = call i32 @func1(i32 %0, i32 %8, i32 0)
  br label %10

10:                                               ; preds = %7, %5
  %.0 = phi i32 [ %6, %5 ], [ %9, %7 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func2(i32 %0, i32 %1) #0 {
  %3 = icmp ne i32 %1, 0
  br i1 %3, label %4, label %7

4:                                                ; preds = %2
  %5 = srem i32 %0, %1
  %6 = call i32 @func2(i32 %5, i32 0)
  br label %8

7:                                                ; preds = %2
  br label %8

8:                                                ; preds = %7, %4
  %.0 = phi i32 [ %6, %4 ], [ %0, %7 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func3(i32 %0, i32 %1) #0 {
  %3 = icmp eq i32 %1, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %2
  %5 = add nsw i32 %0, 1
  br label %9

6:                                                ; preds = %2
  %7 = add nsw i32 %0, %1
  %8 = call i32 @func3(i32 %7, i32 0)
  br label %9

9:                                                ; preds = %6, %4
  %.0 = phi i32 [ %5, %4 ], [ %8, %6 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func4(i32 %0, i32 %1, i32 %2) #0 {
  %4 = icmp ne i32 %0, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %7

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %6, %5
  %.0 = phi i32 [ %1, %5 ], [ %2, %6 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func5(i32 %0) #0 {
  %2 = sub nsw i32 0, %0
  ret i32 %2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func6(i32 %0, i32 %1) #0 {
  %3 = icmp ne i32 %0, 0
  br i1 %3, label %4, label %7

4:                                                ; preds = %2
  %5 = icmp ne i32 %1, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %4
  br label %8

7:                                                ; preds = %4, %2
  br label %8

8:                                                ; preds = %7, %6
  %.0 = phi i32 [ 1, %6 ], [ 0, %7 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func7(i32 %0) #0 {
  %2 = icmp ne i32 %0, 0
  br i1 %2, label %4, label %3

3:                                                ; preds = %1
  br label %5

4:                                                ; preds = %1
  br label %5

5:                                                ; preds = %4, %3
  %.0 = phi i32 [ 0, %4 ], [ 1, %3 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [10 x i32], align 16
  %2 = call i32 (...) @getint()
  %3 = call i32 (...) @getint()
  %4 = call i32 (...) @getint()
  %5 = call i32 (...) @getint()
  br label %6

6:                                                ; preds = %8, %0
  %.0 = phi i32 [ 0, %0 ], [ %12, %8 ]
  %7 = icmp slt i32 %.0, 10
  br i1 %7, label %8, label %13

8:                                                ; preds = %6
  %9 = call i32 (...) @getint()
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 %10
  store i32 %9, i32* %11, align 4
  %12 = add nsw i32 %.0, 1
  br label %6

13:                                               ; preds = %6
  %14 = call i32 @func7(i32 %2)
  %15 = call i32 @func5(i32 %3)
  %16 = call i32 @func6(i32 %14, i32 %15)
  %17 = call i32 @func2(i32 %16, i32 %4)
  %18 = call i32 @func3(i32 %17, i32 %5)
  %19 = call i32 @func5(i32 %18)
  %20 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %21 = load i32, i32* %20, align 16
  %22 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  %23 = load i32, i32* %22, align 4
  %24 = call i32 @func5(i32 %23)
  %25 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 2
  %26 = load i32, i32* %25, align 8
  %27 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 3
  %28 = load i32, i32* %27, align 4
  %29 = call i32 @func7(i32 %28)
  %30 = call i32 @func6(i32 %26, i32 %29)
  %31 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 4
  %32 = load i32, i32* %31, align 16
  %33 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 5
  %34 = load i32, i32* %33, align 4
  %35 = call i32 @func7(i32 %34)
  %36 = call i32 @func2(i32 %32, i32 %35)
  %37 = call i32 @func4(i32 %24, i32 %30, i32 %36)
  %38 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 6
  %39 = load i32, i32* %38, align 8
  %40 = call i32 @func3(i32 %37, i32 %39)
  %41 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 7
  %42 = load i32, i32* %41, align 4
  %43 = call i32 @func2(i32 %40, i32 %42)
  %44 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 8
  %45 = load i32, i32* %44, align 16
  %46 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 9
  %47 = load i32, i32* %46, align 4
  %48 = call i32 @func7(i32 %47)
  %49 = call i32 @func3(i32 %45, i32 %48)
  %50 = call i32 @func1(i32 %43, i32 %49, i32 %2)
  %51 = call i32 @func4(i32 %19, i32 %21, i32 %50)
  %52 = call i32 @func7(i32 %4)
  %53 = call i32 @func3(i32 %52, i32 %5)
  %54 = call i32 @func2(i32 %3, i32 %53)
  %55 = call i32 @func3(i32 %51, i32 %54)
  %56 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
  %57 = load i32, i32* %56, align 16
  %58 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
  %59 = load i32, i32* %58, align 4
  %60 = call i32 @func1(i32 %55, i32 %57, i32 %59)
  %61 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 2
  %62 = load i32, i32* %61, align 8
  %63 = call i32 @func2(i32 %60, i32 %62)
  %64 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 3
  %65 = load i32, i32* %64, align 4
  %66 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 4
  %67 = load i32, i32* %66, align 16
  %68 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 5
  %69 = load i32, i32* %68, align 4
  %70 = call i32 @func5(i32 %69)
  %71 = call i32 @func3(i32 %67, i32 %70)
  %72 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 6
  %73 = load i32, i32* %72, align 8
  %74 = call i32 @func5(i32 %73)
  %75 = call i32 @func2(i32 %71, i32 %74)
  %76 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 7
  %77 = load i32, i32* %76, align 4
  %78 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 8
  %79 = load i32, i32* %78, align 16
  %80 = call i32 @func7(i32 %79)
  %81 = call i32 @func1(i32 %75, i32 %77, i32 %80)
  %82 = getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 9
  %83 = load i32, i32* %82, align 4
  %84 = call i32 @func5(i32 %83)
  %85 = call i32 @func2(i32 %81, i32 %84)
  %86 = call i32 @func3(i32 %85, i32 %2)
  %87 = call i32 @func1(i32 %63, i32 %65, i32 %86)
  ret i32 %87
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
