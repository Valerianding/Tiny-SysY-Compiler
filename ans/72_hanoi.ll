; ModuleID = '72_hanoi.bc'
source_filename = "72_hanoi.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @move(i32 %0, i32 %1) #0 {
  %3 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %0)
  %4 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %5 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %1)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 44)
  %7 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  ret void
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @hanoi(i32 %0, i32 %1, i32 %2, i32 %3) #0 {
  %5 = icmp eq i32 %0, 1
  Br i1 %5, label %6, label %7

6:                                                ; preds = %4
  call void @move(i32 %1, i32 %3)
  Br label %10

7:                                                ; preds = %4
  %8 = sub nsw i32 %0, 1
  call void @hanoi(i32 %8, i32 %1, i32 %3, i32 %2)
  call void @move(i32 %1, i32 %3)
  %9 = sub nsw i32 %0, 1
  call void @hanoi(i32 %9, i32 %2, i32 %1, i32 %3)
  Br label %10

10:                                               ; preds = %7, %6
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  Br label %2

2:                                                ; preds = %4, %0
  %.0 = phi i32 [ %1, %0 ], [ %7, %4 ]
  %3 = icmp sgt i32 %.0, 0
  Br i1 %3, label %4, label %8

4:                                                ; preds = %2
  %5 = call i32 (...) @getint()
  call void @hanoi(i32 %5, i32 1, i32 2, i32 3)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %7 = sub nsw i32 %.0, 1
  Br label %2

8:                                                ; preds = %2
  ret i32 0
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
