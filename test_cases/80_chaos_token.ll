; ModuleID = '80_chaos_token.bc'
source_filename = "80_chaos_token.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__HELLO = dso_local global <{ [28 x i32], [72 x i32] }> <{ [28 x i32] [i32 87, i32 101, i32 108, i32 99, i32 111, i32 109, i32 101, i32 32, i32 116, i32 111, i32 32, i32 116, i32 104, i32 101, i32 32, i32 74, i32 97, i32 112, i32 97, i32 114, i32 105, i32 32, i32 80, i32 97, i32 114, i32 107, i32 33, i32 10], [72 x i32] zeroinitializer }>, align 16
@N4__mE___ = dso_local global <{ <{ i32, i32, i32, i32, i32, i32, i32, [43 x i32] }>, <{ i32, i32, i32, i32, i32, [45 x i32] }>, <{ [12 x i32], [38 x i32] }>, <{ [8 x i32], [42 x i32] }>, <{ [16 x i32], [34 x i32] }>, <{ [14 x i32], [36 x i32] }> }> <{ <{ i32, i32, i32, i32, i32, i32, i32, [43 x i32] }> <{ i32 83, i32 97, i32 97, i32 98, i32 97, i32 114, i32 117, [43 x i32] zeroinitializer }>, <{ i32, i32, i32, i32, i32, [45 x i32] }> <{ i32 75, i32 97, i32 98, i32 97, i32 110, [45 x i32] zeroinitializer }>, <{ [12 x i32], [38 x i32] }> <{ [12 x i32] [i32 72, i32 97, i32 115, i32 104, i32 105, i32 98, i32 105, i32 114, i32 111, i32 107, i32 111, i32 117], [38 x i32] zeroinitializer }>, <{ [8 x i32], [42 x i32] }> <{ [8 x i32] [i32 65, i32 114, i32 97, i32 105, i32 103, i32 117, i32 109, i32 97], [42 x i32] zeroinitializer }>, <{ [16 x i32], [34 x i32] }> <{ [16 x i32] [i32 72, i32 117, i32 110, i32 98, i32 111, i32 114, i32 117, i32 116, i32 111, i32 32, i32 80, i32 101, i32 110, i32 103, i32 105, i32 110], [34 x i32] zeroinitializer }>, <{ [14 x i32], [36 x i32] }> <{ [14 x i32] [i32 84, i32 97, i32 105, i32 114, i32 105, i32 107, i32 117, i32 32, i32 79, i32 111, i32 107, i32 97, i32 109, i32 105], [36 x i32] zeroinitializer }> }>, align 16
@saY_HeI10_To = dso_local global <{ [15 x i32], [25 x i32] }> <{ [15 x i32] [i32 32, i32 115, i32 97, i32 121, i32 115, i32 32, i32 104, i32 101, i32 108, i32 108, i32 111, i32 32, i32 116, i32 111, i32 32], [25 x i32] zeroinitializer }>, align 16
@RET = dso_local global [5 x i32] [i32 10, i32 0, i32 0, i32 0, i32 0], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @putstr(i32* %0) #0 {
  br label %2

2:                                                ; preds = %7, %1
  %.0 = phi i32 [ 0, %1 ], [ %12, %7 ]
  %3 = sext i32 %.0 to i64
  %4 = getelementptr inbounds i32, i32* %0, i64 %3
  %5 = load i32, i32* %4, align 4
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %7, label %13

7:                                                ; preds = %2
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds i32, i32* %0, i64 %8
  %10 = load i32, i32* %9, align 4
  %11 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 %10)
  %12 = add nsw i32 %.0, 1
  br label %2

13:                                               ; preds = %2
  ret i32 %.0
}

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @putstr(i32* getelementptr inbounds ([100 x i32], [100 x i32]* bitcast (<{ [28 x i32], [72 x i32] }>* @__HELLO to [100 x i32]*), i64 0, i64 0))
  br label %2

2:                                                ; preds = %23, %0
  %.0 = phi i32 [ 0, %0 ], [ %20, %23 ]
  %3 = sdiv i32 %.0, 6
  %4 = srem i32 %.0, 6
  %5 = icmp ne i32 %3, %4
  br i1 %5, label %6, label %17

6:                                                ; preds = %2
  %7 = sext i32 %3 to i64
  %8 = getelementptr inbounds [6 x [50 x i32]], [6 x [50 x i32]]* bitcast (<{ <{ i32, i32, i32, i32, i32, i32, i32, [43 x i32] }>, <{ i32, i32, i32, i32, i32, [45 x i32] }>, <{ [12 x i32], [38 x i32] }>, <{ [8 x i32], [42 x i32] }>, <{ [16 x i32], [34 x i32] }>, <{ [14 x i32], [36 x i32] }> }>* @N4__mE___ to [6 x [50 x i32]]*), i64 0, i64 %7
  %9 = getelementptr inbounds [50 x i32], [50 x i32]* %8, i64 0, i64 0
  %10 = call i32 @putstr(i32* %9)
  %11 = call i32 @putstr(i32* getelementptr inbounds ([40 x i32], [40 x i32]* bitcast (<{ [15 x i32], [25 x i32] }>* @saY_HeI10_To to [40 x i32]*), i64 0, i64 0))
  %12 = sext i32 %4 to i64
  %13 = getelementptr inbounds [6 x [50 x i32]], [6 x [50 x i32]]* bitcast (<{ <{ i32, i32, i32, i32, i32, i32, i32, [43 x i32] }>, <{ i32, i32, i32, i32, i32, [45 x i32] }>, <{ [12 x i32], [38 x i32] }>, <{ [8 x i32], [42 x i32] }>, <{ [16 x i32], [34 x i32] }>, <{ [14 x i32], [36 x i32] }> }>* @N4__mE___ to [6 x [50 x i32]]*), i64 0, i64 %12
  %14 = getelementptr inbounds [50 x i32], [50 x i32]* %13, i64 0, i64 0
  %15 = call i32 @putstr(i32* %14)
  %16 = call i32 @putstr(i32* getelementptr inbounds ([5 x i32], [5 x i32]* @RET, i64 0, i64 0))
  br label %17

17:                                               ; preds = %6, %2
  %18 = mul nsw i32 %.0, 17
  %19 = add nsw i32 %18, 23
  %20 = srem i32 %19, 32
  %21 = icmp eq i32 %20, 0
  br i1 %21, label %22, label %23

22:                                               ; preds = %17
  br label %24

23:                                               ; preds = %17
  br label %2

24:                                               ; preds = %22
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
