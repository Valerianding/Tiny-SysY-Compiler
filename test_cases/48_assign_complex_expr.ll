; ModuleID = '48_assign_complex_expr.bc'
source_filename = "48_assign_complex_expr.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = mul nsw i32 -2, 1
  %2 = sdiv i32 %1, 2
  %3 = sub nsw i32 5, 5
  %4 = add nsw i32 %2, %3
  %5 = add nsw i32 1, 3
  %6 = sub nsw i32 0, %5
  %7 = srem i32 %6, 2
  %8 = sub nsw i32 %4, %7
  %9 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %8)
  %10 = srem i32 -2, 2
  %11 = add nsw i32 %10, 67
  %12 = sub nsw i32 5, 5
  %13 = sub nsw i32 0, %12
  %14 = add nsw i32 %11, %13
  %15 = add nsw i32 1, 2
  %16 = srem i32 %15, 2
  %17 = sub nsw i32 0, %16
  %18 = sub nsw i32 %14, %17
  %19 = add nsw i32 %18, 3
  %20 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %19)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
