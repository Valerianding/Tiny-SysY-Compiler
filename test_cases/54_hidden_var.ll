; ModuleID = '54_hidden_var.bc'
source_filename = "54_hidden_var.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@b = dso_local global i32 5, align 4
@c = dso_local global [4 x i32] [i32 6, i32 7, i32 8, i32 9], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [2 x [8 x i32]], align 16
  %2 = alloca [7 x [1 x [5 x i32]]], align 16
  %3 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 3)
  %4 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 3)
  %5 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 1)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %7

7:                                                ; preds = %13, %0
  %8 = icmp slt i32 1, 5
  br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = add nsw i32 0, 1
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  br label %14

13:                                               ; preds = %9
  br label %7

14:                                               ; preds = %12, %7
  %15 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 1)
  %16 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  store i32 1, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 2), align 8
  %17 = bitcast [2 x [8 x i32]]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %17, i8 0, i64 64, i1 false)
  %18 = bitcast i8* %17 to [2 x [8 x i32]]*
  %19 = getelementptr inbounds [2 x [8 x i32]], [2 x [8 x i32]]* %18, i32 0, i32 0
  %20 = getelementptr inbounds [8 x i32], [8 x i32]* %19, i32 0, i32 1
  store i32 9, i32* %20, align 4
  %21 = getelementptr inbounds [2 x [8 x i32]], [2 x [8 x i32]]* %18, i32 0, i32 1
  %22 = getelementptr inbounds [8 x i32], [8 x i32]* %21, i32 0, i32 0
  store i32 8, i32* %22, align 16
  %23 = getelementptr inbounds [8 x i32], [8 x i32]* %21, i32 0, i32 1
  store i32 3, i32* %23, align 4
  %24 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 2), align 8
  %25 = icmp ne i32 %24, 0
  br i1 %25, label %26, label %52

26:                                               ; preds = %14
  %27 = bitcast [7 x [1 x [5 x i32]]]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %27, i8 0, i64 140, i1 false)
  %28 = bitcast i8* %27 to [7 x [1 x [5 x i32]]]*
  %29 = getelementptr inbounds [7 x [1 x [5 x i32]]], [7 x [1 x [5 x i32]]]* %28, i32 0, i32 2
  %30 = getelementptr inbounds [1 x [5 x i32]], [1 x [5 x i32]]* %29, i32 0, i32 0
  %31 = getelementptr inbounds [5 x i32], [5 x i32]* %30, i32 0, i32 0
  store i32 2, i32* %31, align 8
  %32 = getelementptr inbounds [5 x i32], [5 x i32]* %30, i32 0, i32 1
  store i32 1, i32* %32, align 4
  %33 = getelementptr inbounds [5 x i32], [5 x i32]* %30, i32 0, i32 2
  store i32 8, i32* %33, align 8
  %34 = sext i32 2 to i64
  %35 = getelementptr inbounds [7 x [1 x [5 x i32]]], [7 x [1 x [5 x i32]]]* %2, i64 0, i64 %34
  %36 = getelementptr inbounds [1 x [5 x i32]], [1 x [5 x i32]]* %35, i64 0, i64 0
  %37 = getelementptr inbounds [5 x i32], [5 x i32]* %36, i64 0, i64 0
  %38 = load i32, i32* %37, align 4
  %39 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %38)
  %40 = sext i32 2 to i64
  %41 = getelementptr inbounds [7 x [1 x [5 x i32]]], [7 x [1 x [5 x i32]]]* %2, i64 0, i64 %40
  %42 = getelementptr inbounds [1 x [5 x i32]], [1 x [5 x i32]]* %41, i64 0, i64 0
  %43 = getelementptr inbounds [5 x i32], [5 x i32]* %42, i64 0, i64 1
  %44 = load i32, i32* %43, align 4
  %45 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %44)
  %46 = sext i32 2 to i64
  %47 = getelementptr inbounds [7 x [1 x [5 x i32]]], [7 x [1 x [5 x i32]]]* %2, i64 0, i64 %46
  %48 = getelementptr inbounds [1 x [5 x i32]], [1 x [5 x i32]]* %47, i64 0, i64 0
  %49 = getelementptr inbounds [5 x i32], [5 x i32]* %48, i64 0, i64 2
  %50 = load i32, i32* %49, align 4
  %51 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %50)
  br label %52

52:                                               ; preds = %26, %14
  %53 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %54 = load i32, i32* @b, align 4
  %55 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %54)
  %56 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %57 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 0), align 16
  %58 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %57)
  %59 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 1), align 4
  %60 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %59)
  %61 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 2), align 8
  %62 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %61)
  %63 = load i32, i32* getelementptr inbounds ([4 x i32], [4 x i32]* @c, i64 0, i64 3), align 4
  %64 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %63)
  %65 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind willreturn writeonly }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
