; ModuleID = '73_int_io.bc'
source_filename = "73_int_io.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@ascii_0 = dso_local constant i32 48, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @my_getint() #0 {
  br label %1

1:                                                ; preds = %7, %0
  %2 = call i32 (...) @getch()
  %3 = sub nsw i32 %2, 48
  %4 = icmp slt i32 %3, 0
  br i1 %4, label %7, label %5

5:                                                ; preds = %1
  %6 = icmp sgt i32 %3, 9
  br i1 %6, label %7, label %8

7:                                                ; preds = %5, %1
  br label %1

8:                                                ; preds = %5
  br label %9

9:                                                ; preds = %8
  br label %10

10:                                               ; preds = %20, %9
  %.0 = phi i32 [ %3, %9 ], [ %18, %20 ]
  %11 = call i32 (...) @getch()
  %12 = sub nsw i32 %11, 48
  %13 = icmp sge i32 %12, 0
  br i1 %13, label %14, label %19

14:                                               ; preds = %10
  %15 = icmp sle i32 %12, 9
  br i1 %15, label %16, label %19

16:                                               ; preds = %14
  %17 = mul nsw i32 %.0, 10
  %18 = add nsw i32 %17, %12
  br label %20

19:                                               ; preds = %14, %10
  br label %21

20:                                               ; preds = %16
  br label %10

21:                                               ; preds = %19
  ret i32 %.0
}

declare dso_local i32 @getch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @my_putint(i32 %0) #0 {
  %2 = alloca [16 x i32], align 16
  br label %3

3:                                                ; preds = %5, %1
  %.01 = phi i32 [ 0, %1 ], [ %11, %5 ]
  %.0 = phi i32 [ %0, %1 ], [ %10, %5 ]
  %4 = icmp sgt i32 %.0, 0
  br i1 %4, label %5, label %12

5:                                                ; preds = %3
  %6 = srem i32 %.0, 10
  %7 = add nsw i32 %6, 48
  %8 = sext i32 %.01 to i64
  %9 = getelementptr inbounds [16 x i32], [16 x i32]* %2, i64 0, i64 %8
  store i32 %7, i32* %9, align 4
  %10 = sdiv i32 %.0, 10
  %11 = add nsw i32 %.01, 1
  br label %3

12:                                               ; preds = %3
  br label %13

13:                                               ; preds = %15, %12
  %.1 = phi i32 [ %.01, %12 ], [ %16, %15 ]
  %14 = icmp sgt i32 %.1, 0
  br i1 %14, label %15, label %21

15:                                               ; preds = %13
  %16 = sub nsw i32 %.1, 1
  %17 = sext i32 %16 to i64
  %18 = getelementptr inbounds [16 x i32], [16 x i32]* %2, i64 0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 %19)
  br label %13

21:                                               ; preds = %13
  ret void
}

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @my_getint()
  br label %2

2:                                                ; preds = %4, %0
  %.0 = phi i32 [ %1, %0 ], [ %7, %4 ]
  %3 = icmp sgt i32 %.0, 0
  br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = call i32 @my_getint()
  call void @my_putint(i32 %5)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %7 = sub nsw i32 %.0, 1
  br label %2

8:                                                ; preds = %2
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
