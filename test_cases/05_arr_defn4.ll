; ModuleID = '05_arr_defn4.bc'
source_filename = "05_arr_defn4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@__const.main.a = private unnamed_addr constant [4 x [2 x i32]] [[2 x i32] [i32 1, i32 2], [2 x i32] [i32 3, i32 4], [2 x i32] zeroinitializer, [2 x i32] [i32 7, i32 0]], align 16
@__const.main.c = private unnamed_addr constant [4 x [2 x i32]] [[2 x i32] [i32 1, i32 2], [2 x i32] [i32 3, i32 4], [2 x i32] [i32 5, i32 6], [2 x i32] [i32 7, i32 8]], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [4 x [2 x i32]], align 16
  %2 = alloca [4 x [2 x i32]], align 16
  %3 = alloca [4 x [2 x i32]], align 16
  %4 = alloca [4 x [2 x i32]], align 16
  %5 = alloca [4 x [2 x [1 x i32]]], align 16
  %6 = bitcast [4 x [2 x i32]]* %1 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %6, i8* align 16 bitcast ([4 x [2 x i32]]* @__const.main.a to i8*), i64 32, i1 false)
  %7 = bitcast [4 x [2 x i32]]* %2 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %7, i8 0, i64 32, i1 false)
  %8 = bitcast [4 x [2 x i32]]* %3 to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* align 16 %8, i8* align 16 bitcast ([4 x [2 x i32]]* @__const.main.c to i8*), i64 32, i1 false)
  %9 = getelementptr inbounds [4 x [2 x i32]], [4 x [2 x i32]]* %4, i64 0, i64 0
  %10 = getelementptr inbounds [2 x i32], [2 x i32]* %9, i64 0, i64 0
  store i32 1, i32* %10, align 4
  %11 = getelementptr inbounds i32, i32* %10, i64 1
  store i32 2, i32* %11, align 4
  %12 = getelementptr inbounds [2 x i32], [2 x i32]* %9, i64 1
  %13 = getelementptr inbounds [2 x i32], [2 x i32]* %12, i64 0, i64 0
  store i32 3, i32* %13, align 4
  %14 = getelementptr inbounds i32, i32* %13, i64 1
  %15 = getelementptr inbounds i32, i32* %13, i64 2
  br label %16

16:                                               ; preds = %16, %0
  %17 = phi i32* [ %14, %0 ], [ %18, %16 ]
  store i32 0, i32* %17, align 4
  %18 = getelementptr inbounds i32, i32* %17, i64 1
  %19 = icmp eq i32* %18, %15
  br i1 %19, label %20, label %16

20:                                               ; preds = %16
  %21 = getelementptr inbounds [2 x i32], [2 x i32]* %12, i64 1
  %22 = getelementptr inbounds [2 x i32], [2 x i32]* %21, i64 0, i64 0
  store i32 5, i32* %22, align 4
  %23 = getelementptr inbounds i32, i32* %22, i64 1
  %24 = getelementptr inbounds i32, i32* %22, i64 2
  br label %25

25:                                               ; preds = %25, %20
  %26 = phi i32* [ %23, %20 ], [ %27, %25 ]
  store i32 0, i32* %26, align 4
  %27 = getelementptr inbounds i32, i32* %26, i64 1
  %28 = icmp eq i32* %27, %24
  br i1 %28, label %29, label %25

29:                                               ; preds = %25
  %30 = getelementptr inbounds [2 x i32], [2 x i32]* %21, i64 1
  %31 = getelementptr inbounds [2 x i32], [2 x i32]* %30, i64 0, i64 0
  %32 = getelementptr inbounds [4 x [2 x i32]], [4 x [2 x i32]]* %1, i64 0, i64 3
  %33 = getelementptr inbounds [2 x i32], [2 x i32]* %32, i64 0, i64 0
  %34 = load i32, i32* %33, align 8
  store i32 %34, i32* %31, align 4
  %35 = getelementptr inbounds i32, i32* %31, i64 1
  store i32 8, i32* %35, align 4
  %36 = getelementptr inbounds [4 x [2 x [1 x i32]]], [4 x [2 x [1 x i32]]]* %5, i64 0, i64 0
  %37 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %36, i64 0, i64 0
  %38 = getelementptr inbounds [1 x i32], [1 x i32]* %37, i64 0, i64 0
  %39 = getelementptr inbounds [4 x [2 x i32]], [4 x [2 x i32]]* %4, i64 0, i64 2
  %40 = getelementptr inbounds [2 x i32], [2 x i32]* %39, i64 0, i64 1
  %41 = load i32, i32* %40, align 4
  store i32 %41, i32* %38, align 4
  %42 = getelementptr inbounds [1 x i32], [1 x i32]* %37, i64 1
  %43 = getelementptr inbounds [1 x i32], [1 x i32]* %42, i64 0, i64 0
  %44 = getelementptr inbounds [4 x [2 x i32]], [4 x [2 x i32]]* %3, i64 0, i64 2
  %45 = getelementptr inbounds [2 x i32], [2 x i32]* %44, i64 0, i64 1
  %46 = load i32, i32* %45, align 4
  store i32 %46, i32* %43, align 4
  %47 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %36, i64 1
  %48 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %47, i64 0, i64 0
  %49 = getelementptr inbounds [1 x i32], [1 x i32]* %48, i64 0, i64 0
  store i32 3, i32* %49, align 4
  %50 = getelementptr inbounds [1 x i32], [1 x i32]* %48, i64 1
  %51 = getelementptr inbounds [1 x i32], [1 x i32]* %50, i64 0, i64 0
  store i32 4, i32* %51, align 4
  %52 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %47, i64 1
  %53 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %52, i64 0, i64 0
  %54 = getelementptr inbounds [1 x i32], [1 x i32]* %53, i64 0, i64 0
  store i32 5, i32* %54, align 4
  %55 = getelementptr inbounds [1 x i32], [1 x i32]* %53, i64 1
  %56 = getelementptr inbounds [1 x i32], [1 x i32]* %55, i64 0, i64 0
  store i32 6, i32* %56, align 4
  %57 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %52, i64 1
  %58 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %57, i64 0, i64 0
  %59 = getelementptr inbounds [1 x i32], [1 x i32]* %58, i64 0, i64 0
  store i32 7, i32* %59, align 4
  %60 = getelementptr inbounds [1 x i32], [1 x i32]* %58, i64 1
  %61 = getelementptr inbounds [1 x i32], [1 x i32]* %60, i64 0, i64 0
  store i32 8, i32* %61, align 4
  %62 = getelementptr inbounds [4 x [2 x [1 x i32]]], [4 x [2 x [1 x i32]]]* %5, i64 0, i64 3
  %63 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %62, i64 0, i64 1
  %64 = getelementptr inbounds [1 x i32], [1 x i32]* %63, i64 0, i64 0
  %65 = load i32, i32* %64, align 4
  %66 = getelementptr inbounds [4 x [2 x [1 x i32]]], [4 x [2 x [1 x i32]]]* %5, i64 0, i64 0
  %67 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %66, i64 0, i64 0
  %68 = getelementptr inbounds [1 x i32], [1 x i32]* %67, i64 0, i64 0
  %69 = load i32, i32* %68, align 16
  %70 = add nsw i32 %65, %69
  %71 = getelementptr inbounds [4 x [2 x [1 x i32]]], [4 x [2 x [1 x i32]]]* %5, i64 0, i64 0
  %72 = getelementptr inbounds [2 x [1 x i32]], [2 x [1 x i32]]* %71, i64 0, i64 1
  %73 = getelementptr inbounds [1 x i32], [1 x i32]* %72, i64 0, i64 0
  %74 = load i32, i32* %73, align 4
  %75 = add nsw i32 %70, %74
  %76 = getelementptr inbounds [4 x [2 x i32]], [4 x [2 x i32]]* %4, i64 0, i64 3
  %77 = getelementptr inbounds [2 x i32], [2 x i32]* %76, i64 0, i64 0
  %78 = load i32, i32* %77, align 8
  %79 = add nsw i32 %75, %78
  ret i32 %79
}

; Function Attrs: argmemonly nounwind willreturn
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #1

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn }
attributes #2 = { argmemonly nounwind willreturn writeonly }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
