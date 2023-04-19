; ModuleID = '83_long_array.bc'
source_filename = "83_long_array.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@N = dso_local constant i32 10000, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @long_array(i32 %0) #0 {
  %2 = alloca [10000 x i32], align 16
  %3 = alloca [10000 x i32], align 16
  %4 = alloca [10000 x i32], align 16
  Br label %5

5:                                                ; preds = %7, %1
  %.01 = phi i32 [ 0, %1 ], [ %12, %7 ]
  %6 = icmp slt i32 %.01, 10000
  Br i1 %6, label %7, label %13

7:                                                ; preds = %5
  %8 = mul nsw i32 %.01, %.01
  %9 = srem i32 %8, 10
  %10 = sext i32 %.01 to i64
  %11 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %10
  store i32 %9, i32* %11, align 4
  %12 = add nsw i32 %.01, 1
  Br label %5

13:                                               ; preds = %5
  Br label %14

14:                                               ; preds = %16, %13
  %.12 = phi i32 [ 0, %13 ], [ %27, %16 ]
  %15 = icmp slt i32 %.12, 10000
  Br i1 %15, label %16, label %28

16:                                               ; preds = %14
  %17 = sext i32 %.12 to i64
  %18 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %17
  %19 = load i32, i32* %18, align 4
  %20 = sext i32 %.12 to i64
  %21 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %20
  %22 = load i32, i32* %21, align 4
  %23 = mul nsw i32 %19, %22
  %24 = srem i32 %23, 10
  %25 = sext i32 %.12 to i64
  %26 = getelementptr inbounds [10000 x i32], [10000 x i32]* %3, i64 0, i64 %25
  store i32 %24, i32* %26, align 4
  %27 = add nsw i32 %.12, 1
  Br label %14

28:                                               ; preds = %14
  Br label %29

29:                                               ; preds = %31, %28
  %.2 = phi i32 [ 0, %28 ], [ %46, %31 ]
  %30 = icmp slt i32 %.2, 10000
  Br i1 %30, label %31, label %47

31:                                               ; preds = %29
  %32 = sext i32 %.2 to i64
  %33 = getelementptr inbounds [10000 x i32], [10000 x i32]* %3, i64 0, i64 %32
  %34 = load i32, i32* %33, align 4
  %35 = sext i32 %.2 to i64
  %36 = getelementptr inbounds [10000 x i32], [10000 x i32]* %3, i64 0, i64 %35
  %37 = load i32, i32* %36, align 4
  %38 = mul nsw i32 %34, %37
  %39 = srem i32 %38, 100
  %40 = sext i32 %.2 to i64
  %41 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %40
  %42 = load i32, i32* %41, align 4
  %43 = add nsw i32 %39, %42
  %44 = sext i32 %.2 to i64
  %45 = getelementptr inbounds [10000 x i32], [10000 x i32]* %4, i64 0, i64 %44
  store i32 %43, i32* %45, align 4
  %46 = add nsw i32 %.2, 1
  Br label %29

47:                                               ; preds = %29
  Br label %48

48:                                               ; preds = %116, %47
  %.03 = phi i32 [ 0, %47 ], [ %.6, %116 ]
  %.3 = phi i32 [ 0, %47 ], [ %117, %116 ]
  %49 = icmp slt i32 %.3, 10000
  Br i1 %49, label %50, label %118

50:                                               ; preds = %48
  %51 = icmp slt i32 %.3, 10
  Br i1 %51, label %52, label %59

52:                                               ; preds = %50
  %53 = sext i32 %.3 to i64
  %54 = getelementptr inbounds [10000 x i32], [10000 x i32]* %4, i64 0, i64 %53
  %55 = load i32, i32* %54, align 4
  %56 = add nsw i32 %.03, %55
  %57 = srem i32 %56, 1333
  %58 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %57)
  Br label %116

59:                                               ; preds = %50
  %60 = icmp slt i32 %.3, 20
  Br i1 %60, label %61, label %76

61:                                               ; preds = %59
  Br label %62

62:                                               ; preds = %64, %61
  %.07 = phi i32 [ 5000, %61 ], [ %73, %64 ]
  %.14 = phi i32 [ %.03, %61 ], [ %72, %64 ]
  %63 = icmp slt i32 %.07, 10000
  Br i1 %63, label %64, label %74

64:                                               ; preds = %62
  %65 = sext i32 %.3 to i64
  %66 = getelementptr inbounds [10000 x i32], [10000 x i32]* %4, i64 0, i64 %65
  %67 = load i32, i32* %66, align 4
  %68 = add nsw i32 %.14, %67
  %69 = sext i32 %.07 to i64
  %70 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %69
  %71 = load i32, i32* %70, align 4
  %72 = sub nsw i32 %68, %71
  %73 = add nsw i32 %.07, 1
  Br label %62

74:                                               ; preds = %62
  %75 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.14)
  Br label %115

76:                                               ; preds = %59
  %77 = icmp slt i32 %.3, 30
  Br i1 %77, label %78, label %107

78:                                               ; preds = %76
  Br label %79

79:                                               ; preds = %104, %78
  %.25 = phi i32 [ %.03, %78 ], [ %.36, %104 ]
  %.0 = phi i32 [ 5000, %78 ], [ %.1, %104 ]
  %80 = icmp slt i32 %.0, 10000
  Br i1 %80, label %81, label %105

81:                                               ; preds = %79
  %82 = icmp sgt i32 %.0, 2233
  Br i1 %82, label %83, label %93

83:                                               ; preds = %81
  %84 = sext i32 %.3 to i64
  %85 = getelementptr inbounds [10000 x i32], [10000 x i32]* %3, i64 0, i64 %84
  %86 = load i32, i32* %85, align 4
  %87 = add nsw i32 %.25, %86
  %88 = sext i32 %.0 to i64
  %89 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %88
  %90 = load i32, i32* %89, align 4
  %91 = sub nsw i32 %87, %90
  %92 = add nsw i32 %.0, 1
  Br label %104

93:                                               ; preds = %81
  %94 = sext i32 %.3 to i64
  %95 = getelementptr inbounds [10000 x i32], [10000 x i32]* %2, i64 0, i64 %94
  %96 = load i32, i32* %95, align 4
  %97 = add nsw i32 %.25, %96
  %98 = sext i32 %.0 to i64
  %99 = getelementptr inbounds [10000 x i32], [10000 x i32]* %4, i64 0, i64 %98
  %100 = load i32, i32* %99, align 4
  %101 = add nsw i32 %97, %100
  %102 = srem i32 %101, 13333
  %103 = add nsw i32 %.0, 2
  Br label %104

104:                                              ; preds = %93, %83
  %.36 = phi i32 [ %91, %83 ], [ %102, %93 ]
  %.1 = phi i32 [ %92, %83 ], [ %103, %93 ]
  Br label %79

105:                                              ; preds = %79
  %106 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.25)
  Br label %114

107:                                              ; preds = %76
  %108 = sext i32 %.3 to i64
  %109 = getelementptr inbounds [10000 x i32], [10000 x i32]* %4, i64 0, i64 %108
  %110 = load i32, i32* %109, align 4
  %111 = mul nsw i32 %110, %0
  %112 = add nsw i32 %.03, %111
  %113 = srem i32 %112, 99988
  Br label %114

114:                                              ; preds = %107, %105
  %.4 = phi i32 [ %.25, %105 ], [ %113, %107 ]
  Br label %115

115:                                              ; preds = %114, %74
  %.5 = phi i32 [ %.14, %74 ], [ %.4, %114 ]
  Br label %116

116:                                              ; preds = %115, %52
  %.6 = phi i32 [ %57, %52 ], [ %.5, %115 ]
  %117 = add nsw i32 %.3, 1
  Br label %48

118:                                              ; preds = %48
  ret i32 %.03
}

declare dso_local i32 @putint(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @long_array(i32 9)
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
