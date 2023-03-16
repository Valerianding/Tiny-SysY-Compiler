; ModuleID = '68_brainfk.bc'
source_filename = "68_brainfk.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@TAPE_LEN = dso_local constant i32 65536, align 4
@BUFFER_LEN = dso_local constant i32 32768, align 4
@ptr = dso_local global i32 0, align 4
@program = dso_local global [32768 x i32] zeroinitializer, align 16
@tape = dso_local global [65536 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind uwtable
define dso_local void @read_program() #0 {
  %1 = call i32 (...) @getint()
  br label %2

2:                                                ; preds = %4, %0
  %.0 = phi i32 [ 0, %0 ], [ %8, %4 ]
  %3 = icmp slt i32 %.0, %1
  br i1 %3, label %4, label %9

4:                                                ; preds = %2
  %5 = call i32 (...) @getch()
  %6 = sext i32 %.0 to i64
  %7 = getelementptr inbounds [32768 x i32], [32768 x i32]* @program, i64 0, i64 %6
  store i32 %5, i32* %7, align 4
  %8 = add nsw i32 %.0, 1
  br label %2

9:                                                ; preds = %2
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds [32768 x i32], [32768 x i32]* @program, i64 0, i64 %10
  store i32 0, i32* %11, align 4
  ret void
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @getch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local void @interpret(i32* %0) #0 {
  br label %2

2:                                                ; preds = %89, %1
  %.0 = phi i32 [ 0, %1 ], [ %90, %89 ]
  %3 = sext i32 %.0 to i64
  %4 = getelementptr inbounds i32, i32* %0, i64 %3
  %5 = load i32, i32* %4, align 4
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %7, label %91

7:                                                ; preds = %2
  %8 = sext i32 %.0 to i64
  %9 = getelementptr inbounds i32, i32* %0, i64 %8
  %10 = load i32, i32* %9, align 4
  %11 = icmp eq i32 %10, 62
  br i1 %11, label %12, label %15

12:                                               ; preds = %7
  %13 = load i32, i32* @ptr, align 4
  %14 = add nsw i32 %13, 1
  store i32 %14, i32* @ptr, align 4
  br label %89

15:                                               ; preds = %7
  %16 = icmp eq i32 %10, 60
  br i1 %16, label %17, label %20

17:                                               ; preds = %15
  %18 = load i32, i32* @ptr, align 4
  %19 = sub nsw i32 %18, 1
  store i32 %19, i32* @ptr, align 4
  br label %88

20:                                               ; preds = %15
  %21 = icmp eq i32 %10, 43
  br i1 %21, label %22, label %31

22:                                               ; preds = %20
  %23 = load i32, i32* @ptr, align 4
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = add nsw i32 %26, 1
  %28 = load i32, i32* @ptr, align 4
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %29
  store i32 %27, i32* %30, align 4
  br label %87

31:                                               ; preds = %20
  %32 = icmp eq i32 %10, 45
  br i1 %32, label %33, label %42

33:                                               ; preds = %31
  %34 = load i32, i32* @ptr, align 4
  %35 = sext i32 %34 to i64
  %36 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %35
  %37 = load i32, i32* %36, align 4
  %38 = sub nsw i32 %37, 1
  %39 = load i32, i32* @ptr, align 4
  %40 = sext i32 %39 to i64
  %41 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %40
  store i32 %38, i32* %41, align 4
  br label %86

42:                                               ; preds = %31
  %43 = icmp eq i32 %10, 46
  br i1 %43, label %44, label %50

44:                                               ; preds = %42
  %45 = load i32, i32* @ptr, align 4
  %46 = sext i32 %45 to i64
  %47 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %46
  %48 = load i32, i32* %47, align 4
  %49 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 %48)
  br label %85

50:                                               ; preds = %42
  %51 = icmp eq i32 %10, 44
  br i1 %51, label %52, label %57

52:                                               ; preds = %50
  %53 = call i32 (...) @getch()
  %54 = load i32, i32* @ptr, align 4
  %55 = sext i32 %54 to i64
  %56 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %55
  store i32 %53, i32* %56, align 4
  br label %84

57:                                               ; preds = %50
  %58 = icmp eq i32 %10, 93
  br i1 %58, label %59, label %83

59:                                               ; preds = %57
  %60 = load i32, i32* @ptr, align 4
  %61 = sext i32 %60 to i64
  %62 = getelementptr inbounds [65536 x i32], [65536 x i32]* @tape, i64 0, i64 %61
  %63 = load i32, i32* %62, align 4
  %64 = icmp ne i32 %63, 0
  br i1 %64, label %65, label %83

65:                                               ; preds = %59
  br label %66

66:                                               ; preds = %81, %65
  %.01 = phi i32 [ 1, %65 ], [ %.23, %81 ]
  %.1 = phi i32 [ %.0, %65 ], [ %69, %81 ]
  %67 = icmp sgt i32 %.01, 0
  br i1 %67, label %68, label %82

68:                                               ; preds = %66
  %69 = sub nsw i32 %.1, 1
  %70 = sext i32 %69 to i64
  %71 = getelementptr inbounds i32, i32* %0, i64 %70
  %72 = load i32, i32* %71, align 4
  %73 = icmp eq i32 %72, 91
  br i1 %73, label %74, label %76

74:                                               ; preds = %68
  %75 = sub nsw i32 %.01, 1
  br label %81

76:                                               ; preds = %68
  %77 = icmp eq i32 %72, 93
  br i1 %77, label %78, label %80

78:                                               ; preds = %76
  %79 = add nsw i32 %.01, 1
  br label %80

80:                                               ; preds = %78, %76
  %.12 = phi i32 [ %79, %78 ], [ %.01, %76 ]
  br label %81

81:                                               ; preds = %80, %74
  %.23 = phi i32 [ %75, %74 ], [ %.12, %80 ]
  br label %66

82:                                               ; preds = %66
  br label %83

83:                                               ; preds = %82, %59, %57
  %.2 = phi i32 [ %.1, %82 ], [ %.0, %59 ], [ %.0, %57 ]
  br label %84

84:                                               ; preds = %83, %52
  %.3 = phi i32 [ %.0, %52 ], [ %.2, %83 ]
  br label %85

85:                                               ; preds = %84, %44
  %.4 = phi i32 [ %.0, %44 ], [ %.3, %84 ]
  br label %86

86:                                               ; preds = %85, %33
  %.5 = phi i32 [ %.0, %33 ], [ %.4, %85 ]
  br label %87

87:                                               ; preds = %86, %22
  %.6 = phi i32 [ %.0, %22 ], [ %.5, %86 ]
  br label %88

88:                                               ; preds = %87, %17
  %.7 = phi i32 [ %.0, %17 ], [ %.6, %87 ]
  br label %89

89:                                               ; preds = %88, %12
  %.8 = phi i32 [ %.0, %12 ], [ %.7, %88 ]
  %90 = add nsw i32 %.8, 1
  br label %2

91:                                               ; preds = %2
  ret void
}

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  call void @read_program()
  call void @interpret(i32* getelementptr inbounds ([32768 x i32], [32768 x i32]* @program, i64 0, i64 0))
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
