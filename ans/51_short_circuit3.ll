; ModuleID = '51_short_circuit3.bc'
source_filename = "51_short_circuit3.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@a = common dso_local global i32 0, align 4
@b = common dso_local global i32 0, align 4
@d = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @set_a(i32 %0) #0 {
  store i32 %0, i32* @a, align 4
  %2 = load i32, i32* @a, align 4
  ret i32 %2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @set_b(i32 %0) #0 {
  store i32 %0, i32* @b, align 4
  %2 = load i32, i32* @b, align 4
  ret i32 %2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @set_d(i32 %0) #0 {
  store i32 %0, i32* @d, align 4
  %2 = load i32, i32* @d, align 4
  ret i32 %2
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  store i32 2, i32* @a, align 4
  store i32 3, i32* @b, align 4
  %1 = call i32 @set_a(i32 0)
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %7

3:                                                ; preds = %0
  %4 = call i32 @set_b(i32 1)
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %6, %3, %0
  %8 = load i32, i32* @a, align 4
  %9 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %8)
  %10 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %11 = load i32, i32* @b, align 4
  %12 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %11)
  %13 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  store i32 2, i32* @a, align 4
  store i32 3, i32* @b, align 4
  %14 = call i32 @set_a(i32 0)
  %15 = icmp ne i32 %14, 0
  br i1 %15, label %16, label %20

16:                                               ; preds = %7
  %17 = call i32 @set_b(i32 1)
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %19, label %20

19:                                               ; preds = %16
  br label %20

20:                                               ; preds = %19, %16, %7
  %21 = load i32, i32* @a, align 4
  %22 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %21)
  %23 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %24 = load i32, i32* @b, align 4
  %25 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %24)
  %26 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  store i32 2, i32* @d, align 4
  %27 = call i32 @set_d(i32 3)
  %28 = icmp ne i32 %27, 0
  br i1 %28, label %29, label %30

29:                                               ; preds = %20
  br label %30

30:                                               ; preds = %29, %20
  %31 = load i32, i32* @d, align 4
  %32 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %31)
  %33 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  %34 = load i32, i32* @d, align 4
  %35 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %34)
  %36 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %37 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 65)
  %38 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 68)
  %39 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 70)
  %40 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %41

41:                                               ; preds = %47, %30
  %42 = icmp ne i32 0, 0
  br i1 %42, label %43, label %45

43:                                               ; preds = %41
  %44 = icmp ne i32 1, 0
  br label %45

45:                                               ; preds = %43, %41
  %46 = phi i1 [ false, %41 ], [ %44, %43 ]
  br i1 %46, label %47, label %49

47:                                               ; preds = %45
  %48 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 32)
  br label %41

49:                                               ; preds = %45
  %50 = icmp ne i32 0, 0
  br i1 %50, label %53, label %51

51:                                               ; preds = %49
  %52 = icmp ne i32 1, 0
  br i1 %52, label %53, label %55

53:                                               ; preds = %51, %49
  %54 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 67)
  br label %55

55:                                               ; preds = %53, %51
  %56 = icmp sge i32 0, 1
  br i1 %56, label %59, label %57

57:                                               ; preds = %55
  %58 = icmp sle i32 1, 0
  br i1 %58, label %59, label %61

59:                                               ; preds = %57, %55
  %60 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 72)
  br label %61

61:                                               ; preds = %59, %57
  %62 = icmp sge i32 2, 1
  br i1 %62, label %63, label %67

63:                                               ; preds = %61
  %64 = icmp ne i32 4, 3
  br i1 %64, label %65, label %67

65:                                               ; preds = %63
  %66 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 73)
  br label %67

67:                                               ; preds = %65, %63, %61
  %68 = icmp ne i32 1, 0
  %69 = xor i1 %68, true
  %70 = zext i1 %69 to i32
  %71 = icmp eq i32 0, %70
  br i1 %71, label %72, label %74

72:                                               ; preds = %67
  %73 = icmp slt i32 3, 3
  br i1 %73, label %76, label %74

74:                                               ; preds = %72, %67
  %75 = icmp sge i32 4, 4
  br i1 %75, label %76, label %78

76:                                               ; preds = %74, %72
  %77 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 74)
  br label %78

78:                                               ; preds = %76, %74
  %79 = icmp ne i32 1, 0
  %80 = xor i1 %79, true
  %81 = zext i1 %80 to i32
  %82 = icmp eq i32 0, %81
  br i1 %82, label %87, label %83

83:                                               ; preds = %78
  %84 = icmp slt i32 3, 3
  br i1 %84, label %85, label %89

85:                                               ; preds = %83
  %86 = icmp sge i32 4, 4
  br i1 %86, label %87, label %89

87:                                               ; preds = %85, %78
  %88 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 75)
  br label %89

89:                                               ; preds = %87, %85, %83
  %90 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
