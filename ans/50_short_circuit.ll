; ModuleID = '50_short_circuit.bc'
source_filename = "50_short_circuit.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@g = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @func(i32 %0) #0 {
  %2 = load i32, i32* @g, align 4
  %3 = add nsw i32 %2, %0
  store i32 %3, i32* @g, align 4
  %4 = load i32, i32* @g, align 4
  %5 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %4)
  %6 = load i32, i32* @g, align 4
  ret i32 %6
}

declare dso_local i32 @putint(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  %2 = icmp sgt i32 %1, 10
  br i1 %2, label %3, label %7

3:                                                ; preds = %0
  %4 = call i32 @func(i32 %1)
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  br label %8

7:                                                ; preds = %3, %0
  br label %8

8:                                                ; preds = %7, %6
  %9 = call i32 (...) @getint()
  %10 = icmp sgt i32 %9, 11
  br i1 %10, label %11, label %15

11:                                               ; preds = %8
  %12 = call i32 @func(i32 %9)
  %13 = icmp ne i32 %12, 0
  br i1 %13, label %14, label %15

14:                                               ; preds = %11
  br label %16

15:                                               ; preds = %11, %8
  br label %16

16:                                               ; preds = %15, %14
  %17 = call i32 (...) @getint()
  %18 = icmp sle i32 %17, 99
  br i1 %18, label %22, label %19

19:                                               ; preds = %16
  %20 = call i32 @func(i32 %17)
  %21 = icmp ne i32 %20, 0
  br i1 %21, label %22, label %23

22:                                               ; preds = %19, %16
  br label %24

23:                                               ; preds = %19
  br label %24

24:                                               ; preds = %23, %22
  %25 = call i32 (...) @getint()
  %26 = icmp sle i32 %25, 100
  br i1 %26, label %30, label %27

27:                                               ; preds = %24
  %28 = call i32 @func(i32 %25)
  %29 = icmp ne i32 %28, 0
  br i1 %29, label %30, label %31

30:                                               ; preds = %27, %24
  br label %32

31:                                               ; preds = %27
  br label %32

32:                                               ; preds = %31, %30
  %33 = call i32 @func(i32 99)
  %34 = icmp ne i32 %33, 0
  br i1 %34, label %39, label %35

35:                                               ; preds = %32
  %36 = call i32 @func(i32 100)
  %37 = icmp ne i32 %36, 0
  br i1 %37, label %38, label %39

38:                                               ; preds = %35
  br label %40

39:                                               ; preds = %35, %32
  br label %40

40:                                               ; preds = %39, %38
  ret i32 0
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
