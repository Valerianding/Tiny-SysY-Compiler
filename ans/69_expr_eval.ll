; ModuleID = '69_expr_eval.bc'
source_filename = "69_expr_eval.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@TOKEN_NUM = dso_local constant i32 0, align 4
@TOKEN_OTHER = dso_local constant i32 1, align 4
@last_char = dso_local global i32 32, align 4
@num = common dso_local global i32 0, align 4
@cur_token = common dso_local global i32 0, align 4
@other = common dso_local global i32 0, align 4

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @next_char() #0 {
  %1 = call i32 (...) @getch()
  store i32 %1, i32* @last_char, align 4
  %2 = load i32, i32* @last_char, align 4
  ret i32 %2
}

declare dso_local i32 @getch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @is_space(i32 %0) #0 {
  %2 = icmp eq i32 %0, 32
  Br i1 %2, label %5, label %3

3:                                                ; preds = %1
  %4 = icmp eq i32 %0, 10
  Br i1 %4, label %5, label %6

5:                                                ; preds = %3, %1
  Br label %7

6:                                                ; preds = %3
  Br label %7

7:                                                ; preds = %6, %5
  %.0 = phi i32 [ 1, %5 ], [ 0, %6 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @is_num(i32 %0) #0 {
  %2 = icmp sge i32 %0, 48
  Br i1 %2, label %3, label %6

3:                                                ; preds = %1
  %4 = icmp sle i32 %0, 57
  Br i1 %4, label %5, label %6

5:                                                ; preds = %3
  Br label %7

6:                                                ; preds = %3, %1
  Br label %7

7:                                                ; preds = %6, %5
  %.0 = phi i32 [ 1, %5 ], [ 0, %6 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @next_token() #0 {
  Br label %1

1:                                                ; preds = %5, %0
  %2 = load i32, i32* @last_char, align 4
  %3 = call i32 @is_space(i32 %2)
  %4 = icmp ne i32 %3, 0
  Br i1 %4, label %5, label %7

5:                                                ; preds = %1
  %6 = call i32 @next_char()
  Br label %1

7:                                                ; preds = %1
  %8 = load i32, i32* @last_char, align 4
  %9 = call i32 @is_num(i32 %8)
  %10 = icmp ne i32 %9, 0
  Br i1 %10, label %11, label %25

11:                                               ; preds = %7
  %12 = load i32, i32* @last_char, align 4
  %13 = sub nsw i32 %12, 48
  store i32 %13, i32* @num, align 4
  Br label %14

14:                                               ; preds = %18, %11
  %15 = call i32 @next_char()
  %16 = call i32 @is_num(i32 %15)
  %17 = icmp ne i32 %16, 0
  Br i1 %17, label %18, label %24

18:                                               ; preds = %14
  %19 = load i32, i32* @num, align 4
  %20 = mul nsw i32 %19, 10
  %21 = load i32, i32* @last_char, align 4
  %22 = add nsw i32 %20, %21
  %23 = sub nsw i32 %22, 48
  store i32 %23, i32* @num, align 4
  Br label %14

24:                                               ; preds = %14
  store i32 0, i32* @cur_token, align 4
  Br label %28

25:                                               ; preds = %7
  %26 = load i32, i32* @last_char, align 4
  store i32 %26, i32* @other, align 4
  %27 = call i32 @next_char()
  store i32 1, i32* @cur_token, align 4
  Br label %28

28:                                               ; preds = %25, %24
  %29 = load i32, i32* @cur_token, align 4
  ret i32 %29
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @panic() #0 {
  %1 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 112)
  %2 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 97)
  %3 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 110)
  %4 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 105)
  %5 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 99)
  %6 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 33)
  %7 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 -1
}

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @get_op_prec(i32 %0) #0 {
  %2 = icmp eq i32 %0, 43
  Br i1 %2, label %5, label %3

3:                                                ; preds = %1
  %4 = icmp eq i32 %0, 45
  Br i1 %4, label %5, label %6

5:                                                ; preds = %3, %1
  Br label %14

6:                                                ; preds = %3
  %7 = icmp eq i32 %0, 42
  Br i1 %7, label %12, label %8

8:                                                ; preds = %6
  %9 = icmp eq i32 %0, 47
  Br i1 %9, label %12, label %10

10:                                               ; preds = %8
  %11 = icmp eq i32 %0, 37
  Br i1 %11, label %12, label %13

12:                                               ; preds = %10, %8, %6
  Br label %14

13:                                               ; preds = %10
  Br label %14

14:                                               ; preds = %13, %12, %5
  %.0 = phi i32 [ 10, %5 ], [ 20, %12 ], [ 0, %13 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @stack_push(i32* %0, i32 %1) #0 {
  %3 = getelementptr inbounds i32, i32* %0, i64 0
  %4 = load i32, i32* %3, align 4
  %5 = add nsw i32 %4, 1
  %6 = getelementptr inbounds i32, i32* %0, i64 0
  store i32 %5, i32* %6, align 4
  %7 = getelementptr inbounds i32, i32* %0, i64 0
  %8 = load i32, i32* %7, align 4
  %9 = sext i32 %8 to i64
  %10 = getelementptr inbounds i32, i32* %0, i64 %9
  store i32 %1, i32* %10, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @stack_pop(i32* %0) #0 {
  %2 = getelementptr inbounds i32, i32* %0, i64 0
  %3 = load i32, i32* %2, align 4
  %4 = sext i32 %3 to i64
  %5 = getelementptr inbounds i32, i32* %0, i64 %4
  %6 = load i32, i32* %5, align 4
  %7 = getelementptr inbounds i32, i32* %0, i64 0
  %8 = load i32, i32* %7, align 4
  %9 = sub nsw i32 %8, 1
  %10 = getelementptr inbounds i32, i32* %0, i64 0
  store i32 %9, i32* %10, align 4
  ret i32 %6
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @stack_peek(i32* %0) #0 {
  %2 = getelementptr inbounds i32, i32* %0, i64 0
  %3 = load i32, i32* %2, align 4
  %4 = sext i32 %3 to i64
  %5 = getelementptr inbounds i32, i32* %0, i64 %4
  %6 = load i32, i32* %5, align 4
  ret i32 %6
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @stack_size(i32* %0) #0 {
  %2 = getelementptr inbounds i32, i32* %0, i64 0
  %3 = load i32, i32* %2, align 4
  ret i32 %3
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @eval_op(i32 %0, i32 %1, i32 %2) #0 {
  %4 = icmp eq i32 %0, 43
  Br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = add nsw i32 %1, %2
  Br label %24

7:                                                ; preds = %3
  %8 = icmp eq i32 %0, 45
  Br i1 %8, label %9, label %11

9:                                                ; preds = %7
  %10 = sub nsw i32 %1, %2
  Br label %24

11:                                               ; preds = %7
  %12 = icmp eq i32 %0, 42
  Br i1 %12, label %13, label %15

13:                                               ; preds = %11
  %14 = mul nsw i32 %1, %2
  Br label %24

15:                                               ; preds = %11
  %16 = icmp eq i32 %0, 47
  Br i1 %16, label %17, label %19

17:                                               ; preds = %15
  %18 = sdiv i32 %1, %2
  Br label %24

19:                                               ; preds = %15
  %20 = icmp eq i32 %0, 37
  Br i1 %20, label %21, label %23

21:                                               ; preds = %19
  %22 = srem i32 %1, %2
  Br label %24

23:                                               ; preds = %19
  Br label %24

24:                                               ; preds = %23, %21, %17, %13, %9, %5
  %.0 = phi i32 [ %6, %5 ], [ %10, %9 ], [ %14, %13 ], [ %18, %17 ], [ %22, %21 ], [ 0, %23 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @eval() #0 {
  %1 = alloca [256 x i32], align 16
  %2 = alloca [256 x i32], align 16
  %3 = bitcast [256 x i32]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %3, i8 0, i64 1024, i1 false)
  %4 = bitcast [256 x i32]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %4, i8 0, i64 1024, i1 false)
  %5 = load i32, i32* @cur_token, align 4
  %6 = icmp ne i32 %5, 0
  Br i1 %6, label %7, label %9

7:                                                ; preds = %0
  %8 = call i32 @panic()
  Br label %72

9:                                                ; preds = %0
  %10 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %11 = load i32, i32* @num, align 4
  call void @stack_push(i32* %10, i32 %11)
  %12 = call i32 @next_token()
  Br label %13

13:                                               ; preds = %50, %9
  %14 = load i32, i32* @cur_token, align 4
  %15 = icmp eq i32 %14, 1
  Br i1 %15, label %16, label %54

16:                                               ; preds = %13
  %17 = load i32, i32* @other, align 4
  %18 = call i32 @get_op_prec(i32 %17)
  %19 = icmp ne i32 %18, 0
  Br i1 %19, label %21, label %20

20:                                               ; preds = %16
  Br label %54

21:                                               ; preds = %16
  %22 = call i32 @next_token()
  Br label %23

23:                                               ; preds = %35, %21
  %24 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  %25 = call i32 @stack_size(i32* %24)
  %26 = icmp ne i32 %25, 0
  Br i1 %26, label %27, label %33

27:                                               ; preds = %23
  %28 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  %29 = call i32 @stack_peek(i32* %28)
  %30 = call i32 @get_op_prec(i32 %29)
  %31 = call i32 @get_op_prec(i32 %17)
  %32 = icmp sge i32 %30, %31
  Br label %33

33:                                               ; preds = %27, %23
  %34 = phi i1 [ false, %23 ], [ %32, %27 ]
  Br i1 %34, label %35, label %44

35:                                               ; preds = %33
  %36 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  %37 = call i32 @stack_pop(i32* %36)
  %38 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %39 = call i32 @stack_pop(i32* %38)
  %40 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %41 = call i32 @stack_pop(i32* %40)
  %42 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %43 = call i32 @eval_op(i32 %37, i32 %41, i32 %39)
  call void @stack_push(i32* %42, i32 %43)
  Br label %23

44:                                               ; preds = %33
  %45 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  call void @stack_push(i32* %45, i32 %17)
  %46 = load i32, i32* @cur_token, align 4
  %47 = icmp ne i32 %46, 0
  Br i1 %47, label %48, label %50

48:                                               ; preds = %44
  %49 = call i32 @panic()
  Br label %72

50:                                               ; preds = %44
  %51 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %52 = load i32, i32* @num, align 4
  call void @stack_push(i32* %51, i32 %52)
  %53 = call i32 @next_token()
  Br label %13

54:                                               ; preds = %20, %13
  %55 = call i32 @next_token()
  Br label %56

56:                                               ; preds = %60, %54
  %57 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  %58 = call i32 @stack_size(i32* %57)
  %59 = icmp ne i32 %58, 0
  Br i1 %59, label %60, label %69

60:                                               ; preds = %56
  %61 = getelementptr inbounds [256 x i32], [256 x i32]* %2, i64 0, i64 0
  %62 = call i32 @stack_pop(i32* %61)
  %63 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %64 = call i32 @stack_pop(i32* %63)
  %65 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %66 = call i32 @stack_pop(i32* %65)
  %67 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %68 = call i32 @eval_op(i32 %62, i32 %66, i32 %64)
  call void @stack_push(i32* %67, i32 %68)
  Br label %56

69:                                               ; preds = %56
  %70 = getelementptr inbounds [256 x i32], [256 x i32]* %1, i64 0, i64 0
  %71 = call i32 @stack_peek(i32* %70)
  Br label %72

72:                                               ; preds = %69, %48, %7
  %.0 = phi i32 [ %8, %7 ], [ %49, %48 ], [ %71, %69 ]
  ret i32 %.0
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 (...) @getint()
  %2 = call i32 (...) @getch()
  %3 = call i32 @next_token()
  Br label %4

4:                                                ; preds = %6, %0
  %.0 = phi i32 [ %1, %0 ], [ %10, %6 ]
  %5 = icmp ne i32 %.0, 0
  Br i1 %5, label %6, label %11

6:                                                ; preds = %4
  %7 = call i32 @eval()
  %8 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %7)
  %9 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %10 = sub nsw i32 %.0, 1
  Br label %4

11:                                               ; preds = %4
  ret i32 0
}

declare dso_local i32 @getint(...) #1

declare dso_local i32 @putint(...) #1

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { argmemonly nounwind willreturn }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
