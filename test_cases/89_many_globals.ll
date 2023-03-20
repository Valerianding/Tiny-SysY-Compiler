; ModuleID = '89_many_globals.bc'
source_filename = "89_many_globals.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@a0 = dso_local global i32 0, align 4
@a1 = dso_local global i32 0, align 4
@a2 = dso_local global i32 0, align 4
@a3 = dso_local global i32 0, align 4
@a4 = dso_local global i32 0, align 4
@a5 = dso_local global i32 0, align 4
@a6 = dso_local global i32 0, align 4
@a7 = dso_local global i32 0, align 4
@a8 = dso_local global i32 0, align 4
@a9 = dso_local global i32 0, align 4
@a10 = dso_local global i32 0, align 4
@a11 = dso_local global i32 0, align 4
@a12 = dso_local global i32 0, align 4
@a13 = dso_local global i32 0, align 4
@a14 = dso_local global i32 0, align 4
@a15 = dso_local global i32 0, align 4
@a16 = dso_local global i32 0, align 4
@a17 = dso_local global i32 0, align 4
@a18 = dso_local global i32 0, align 4
@a19 = dso_local global i32 0, align 4
@a20 = dso_local global i32 0, align 4
@a21 = dso_local global i32 0, align 4
@a22 = dso_local global i32 0, align 4
@a23 = dso_local global i32 0, align 4
@a24 = dso_local global i32 0, align 4
@a25 = dso_local global i32 0, align 4
@a26 = dso_local global i32 0, align 4
@a27 = dso_local global i32 0, align 4
@a28 = dso_local global i32 0, align 4
@a29 = dso_local global i32 0, align 4
@a30 = dso_local global i32 0, align 4
@a31 = dso_local global i32 0, align 4
@a32 = dso_local global i32 0, align 4
@a33 = dso_local global i32 0, align 4
@a34 = dso_local global i32 0, align 4
@a35 = dso_local global i32 0, align 4
@a36 = dso_local global i32 0, align 4
@a37 = dso_local global i32 0, align 4
@a38 = dso_local global i32 0, align 4
@a39 = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @testParam8(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7) #0 {
  %9 = add nsw i32 %0, %1
  %10 = add nsw i32 %9, %2
  %11 = add nsw i32 %10, %3
  %12 = add nsw i32 %11, %4
  %13 = add nsw i32 %12, %5
  %14 = add nsw i32 %13, %6
  %15 = add nsw i32 %14, %7
  ret i32 %15
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @testParam16(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15) #0 {
  %17 = add nsw i32 %0, %1
  %18 = add nsw i32 %17, %2
  %19 = sub nsw i32 %18, %3
  %20 = sub nsw i32 %19, %4
  %21 = sub nsw i32 %20, %5
  %22 = sub nsw i32 %21, %6
  %23 = sub nsw i32 %22, %7
  %24 = add nsw i32 %23, %8
  %25 = add nsw i32 %24, %9
  %26 = add nsw i32 %25, %10
  %27 = add nsw i32 %26, %11
  %28 = add nsw i32 %27, %12
  %29 = add nsw i32 %28, %13
  %30 = add nsw i32 %29, %14
  %31 = add nsw i32 %30, %15
  ret i32 %31
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @testParam32(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15, i32 %16, i32 %17, i32 %18, i32 %19, i32 %20, i32 %21, i32 %22, i32 %23, i32 %24, i32 %25, i32 %26, i32 %27, i32 %28, i32 %29, i32 %30, i32 %31) #0 {
  %33 = add nsw i32 %0, %1
  %34 = add nsw i32 %33, %2
  %35 = add nsw i32 %34, %3
  %36 = add nsw i32 %35, %4
  %37 = add nsw i32 %36, %5
  %38 = add nsw i32 %37, %6
  %39 = add nsw i32 %38, %7
  %40 = add nsw i32 %39, %8
  %41 = add nsw i32 %40, %9
  %42 = add nsw i32 %41, %10
  %43 = add nsw i32 %42, %11
  %44 = add nsw i32 %43, %12
  %45 = add nsw i32 %44, %13
  %46 = add nsw i32 %45, %14
  %47 = add nsw i32 %46, %15
  %48 = add nsw i32 %47, %16
  %49 = add nsw i32 %48, %17
  %50 = sub nsw i32 %49, %18
  %51 = sub nsw i32 %50, %19
  %52 = sub nsw i32 %51, %20
  %53 = sub nsw i32 %52, %21
  %54 = sub nsw i32 %53, %22
  %55 = add nsw i32 %54, %23
  %56 = add nsw i32 %55, %24
  %57 = add nsw i32 %56, %25
  %58 = add nsw i32 %57, %26
  %59 = add nsw i32 %58, %27
  %60 = add nsw i32 %59, %28
  %61 = add nsw i32 %60, %29
  %62 = add nsw i32 %61, %30
  %63 = add nsw i32 %62, %31
  ret i32 %63
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  store i32 0, i32* @a0, align 4
  store i32 1, i32* @a1, align 4
  store i32 2, i32* @a2, align 4
  store i32 3, i32* @a3, align 4
  store i32 4, i32* @a4, align 4
  store i32 5, i32* @a5, align 4
  store i32 6, i32* @a6, align 4
  store i32 7, i32* @a7, align 4
  store i32 8, i32* @a8, align 4
  store i32 9, i32* @a9, align 4
  store i32 0, i32* @a10, align 4
  store i32 1, i32* @a11, align 4
  store i32 2, i32* @a12, align 4
  store i32 3, i32* @a13, align 4
  store i32 4, i32* @a14, align 4
  store i32 5, i32* @a15, align 4
  store i32 6, i32* @a16, align 4
  store i32 7, i32* @a17, align 4
  store i32 8, i32* @a18, align 4
  store i32 9, i32* @a19, align 4
  store i32 0, i32* @a20, align 4
  store i32 1, i32* @a21, align 4
  store i32 2, i32* @a22, align 4
  store i32 3, i32* @a23, align 4
  store i32 4, i32* @a24, align 4
  store i32 5, i32* @a25, align 4
  store i32 6, i32* @a26, align 4
  store i32 7, i32* @a27, align 4
  store i32 8, i32* @a28, align 4
  store i32 9, i32* @a29, align 4
  store i32 0, i32* @a30, align 4
  store i32 1, i32* @a31, align 4
  store i32 4, i32* @a32, align 4
  store i32 5, i32* @a33, align 4
  store i32 6, i32* @a34, align 4
  store i32 7, i32* @a35, align 4
  store i32 8, i32* @a36, align 4
  store i32 9, i32* @a37, align 4
  store i32 0, i32* @a38, align 4
  store i32 1, i32* @a39, align 4
  %1 = load i32, i32* @a0, align 4
  %2 = load i32, i32* @a1, align 4
  %3 = load i32, i32* @a2, align 4
  %4 = load i32, i32* @a3, align 4
  %5 = load i32, i32* @a4, align 4
  %6 = load i32, i32* @a5, align 4
  %7 = load i32, i32* @a6, align 4
  %8 = load i32, i32* @a7, align 4
  %9 = call i32 @testParam8(i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8)
  store i32 %9, i32* @a0, align 4
  %10 = load i32, i32* @a0, align 4
  %11 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %10)
  %12 = load i32, i32* @a32, align 4
  %13 = load i32, i32* @a33, align 4
  %14 = load i32, i32* @a34, align 4
  %15 = load i32, i32* @a35, align 4
  %16 = load i32, i32* @a36, align 4
  %17 = load i32, i32* @a37, align 4
  %18 = load i32, i32* @a38, align 4
  %19 = load i32, i32* @a39, align 4
  %20 = load i32, i32* @a8, align 4
  %21 = load i32, i32* @a9, align 4
  %22 = load i32, i32* @a10, align 4
  %23 = load i32, i32* @a11, align 4
  %24 = load i32, i32* @a12, align 4
  %25 = load i32, i32* @a13, align 4
  %26 = load i32, i32* @a14, align 4
  %27 = load i32, i32* @a15, align 4
  %28 = call i32 @testParam16(i32 %12, i32 %13, i32 %14, i32 %15, i32 %16, i32 %17, i32 %18, i32 %19, i32 %20, i32 %21, i32 %22, i32 %23, i32 %24, i32 %25, i32 %26, i32 %27)
  store i32 %28, i32* @a0, align 4
  %29 = load i32, i32* @a0, align 4
  %30 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %29)
  %31 = load i32, i32* @a0, align 4
  %32 = load i32, i32* @a1, align 4
  %33 = load i32, i32* @a2, align 4
  %34 = load i32, i32* @a3, align 4
  %35 = load i32, i32* @a4, align 4
  %36 = load i32, i32* @a5, align 4
  %37 = load i32, i32* @a6, align 4
  %38 = load i32, i32* @a7, align 4
  %39 = load i32, i32* @a8, align 4
  %40 = load i32, i32* @a9, align 4
  %41 = load i32, i32* @a10, align 4
  %42 = load i32, i32* @a11, align 4
  %43 = load i32, i32* @a12, align 4
  %44 = load i32, i32* @a13, align 4
  %45 = load i32, i32* @a14, align 4
  %46 = load i32, i32* @a15, align 4
  %47 = load i32, i32* @a16, align 4
  %48 = load i32, i32* @a17, align 4
  %49 = load i32, i32* @a18, align 4
  %50 = load i32, i32* @a19, align 4
  %51 = load i32, i32* @a20, align 4
  %52 = load i32, i32* @a21, align 4
  %53 = load i32, i32* @a22, align 4
  %54 = load i32, i32* @a23, align 4
  %55 = load i32, i32* @a24, align 4
  %56 = load i32, i32* @a25, align 4
  %57 = load i32, i32* @a26, align 4
  %58 = load i32, i32* @a27, align 4
  %59 = load i32, i32* @a28, align 4
  %60 = load i32, i32* @a29, align 4
  %61 = load i32, i32* @a30, align 4
  %62 = load i32, i32* @a31, align 4
  %63 = call i32 @testParam32(i32 %31, i32 %32, i32 %33, i32 %34, i32 %35, i32 %36, i32 %37, i32 %38, i32 %39, i32 %40, i32 %41, i32 %42, i32 %43, i32 %44, i32 %45, i32 %46, i32 %47, i32 %48, i32 %49, i32 %50, i32 %51, i32 %52, i32 %53, i32 %54, i32 %55, i32 %56, i32 %57, i32 %58, i32 %59, i32 %60, i32 %61, i32 %62)
  store i32 %63, i32* @a0, align 4
  %64 = load i32, i32* @a0, align 4
  %65 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %64)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
