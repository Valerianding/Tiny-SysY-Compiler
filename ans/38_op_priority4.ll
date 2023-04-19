; ModuleID = '38_op_priority4.bc'
source_filename = "38_op_priority4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = common dso_local global i32 0, align 4
@b = common dso_local global i32 0, align 4
@c = common dso_local global i32 0, align 4
@d = common dso_local global i32 0, align 4
@e = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  store i32 %1, i32* @a, align 4
  %2 = call i32 (...) @getint()
  store i32 %2, i32* @b, align 4
  %3 = call i32 (...) @getint()
  store i32 %3, i32* @c, align 4
  %4 = call i32 (...) @getint()
  store i32 %4, i32* @d, align 4
  %5 = call i32 (...) @getint()
  store i32 %5, i32* @e, align 4
  %6 = load i32, i32* @a, align 4
  %7 = load i32, i32* @b, align 4
  %8 = load i32, i32* @c, align 4
  %9 = mul nsw i32 %7, %8
  %10 = sub nsw i32 %6, %9
  %11 = load i32, i32* @d, align 4
  %12 = load i32, i32* @a, align 4
  %13 = load i32, i32* @c, align 4
  %14 = sdiv i32 %12, %13
  %15 = sub nsw i32 %11, %14
  %16 = icmp ne i32 %10, %15
  Br i1 %16, label %37, label %17

17:                                               ; preds = %0
  %18 = load i32, i32* @a, align 4
  %19 = load i32, i32* @b, align 4
  %20 = mul nsw i32 %18, %19
  %21 = load i32, i32* @c, align 4
  %22 = sdiv i32 %20, %21
  %23 = load i32, i32* @e, align 4
  %24 = load i32, i32* @d, align 4
  %25 = add nsw i32 %23, %24
  %26 = icmp eq i32 %22, %25
  Br i1 %26, label %37, label %27

27:                                               ; preds = %17
  %28 = load i32, i32* @a, align 4
  %29 = load i32, i32* @b, align 4
  %30 = add nsw i32 %28, %29
  %31 = load i32, i32* @c, align 4
  %32 = add nsw i32 %30, %31
  %33 = load i32, i32* @d, align 4
  %34 = load i32, i32* @e, align 4
  %35 = add nsw i32 %33, %34
  %36 = icmp eq i32 %32, %35
  Br i1 %36, label %37, label %38

37:                                               ; preds = %27, %17, %0
  Br label %38

38:                                               ; preds = %37, %27
  %.0 = phi i32 [ 1, %37 ], [ 0, %27 ]
  ret i32 %.0
}

declare dso_local i32 @getint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
