; ModuleID = '78_side_effect.bc'
source_filename = "78_side_effect.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = dso_local global i32 -1, align 4
@b = dso_local global i32 1, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @inc_a() #0 {
  %1 = load i32, i32* @a, align 4
  %2 = add nsw i32 %1, 1
  store i32 %2, i32* @a, align 4
  %3 = load i32, i32* @a, align 4
  ret i32 %3
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  br label %1

1:                                                ; preds = %39, %0
  %.0 = phi i32 [ 5, %0 ], [ %40, %39 ]
  %2 = icmp sge i32 %.0, 0
  br i1 %2, label %3, label %41

3:                                                ; preds = %1
  %4 = call i32 @inc_a()
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %19

6:                                                ; preds = %3
  %7 = call i32 @inc_a()
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %19

9:                                                ; preds = %6
  %10 = call i32 @inc_a()
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %12, label %19

12:                                               ; preds = %9
  %13 = load i32, i32* @a, align 4
  %14 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %13)
  %15 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %16 = load i32, i32* @b, align 4
  %17 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %16)
  %18 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %19

19:                                               ; preds = %12, %9, %6, %3
  %20 = call i32 @inc_a()
  %21 = icmp slt i32 %20, 14
  br i1 %21, label %31, label %22

22:                                               ; preds = %19
  %23 = call i32 @inc_a()
  %24 = icmp ne i32 %23, 0
  br i1 %24, label %25, label %37

25:                                               ; preds = %22
  %26 = call i32 @inc_a()
  %27 = call i32 @inc_a()
  %28 = sub nsw i32 %26, %27
  %29 = add nsw i32 %28, 1
  %30 = icmp ne i32 %29, 0
  br i1 %30, label %31, label %37

31:                                               ; preds = %25, %19
  %32 = load i32, i32* @a, align 4
  %33 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %32)
  %34 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %35 = load i32, i32* @b, align 4
  %36 = mul nsw i32 %35, 2
  store i32 %36, i32* @b, align 4
  br label %39

37:                                               ; preds = %25, %22
  %38 = call i32 @inc_a()
  br label %39

39:                                               ; preds = %37, %31
  %40 = sub nsw i32 %.0, 1
  br label %1

41:                                               ; preds = %1
  %42 = load i32, i32* @a, align 4
  %43 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %42)
  %44 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %45 = load i32, i32* @b, align 4
  %46 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %45)
  %47 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %48 = load i32, i32* @a, align 4
  ret i32 %48
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
