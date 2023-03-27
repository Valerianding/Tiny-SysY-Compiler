; ModuleID = '87_many_params.bc'
source_filename = "87_many_params.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @sort(i32* %0, i32 %1) #0 {
  br label %3

3:                                                ; preds = %31, %2
  %.01 = phi i32 [ 0, %2 ], [ %32, %31 ]
  %4 = sub nsw i32 %1, 1
  %5 = icmp slt i32 %.01, %4
  br i1 %5, label %6, label %33

6:                                                ; preds = %3
  %7 = add nsw i32 %.01, 1
  br label %8

8:                                                ; preds = %29, %6
  %.0 = phi i32 [ %7, %6 ], [ %30, %29 ]
  %9 = icmp slt i32 %.0, %1
  br i1 %9, label %10, label %31

10:                                               ; preds = %8
  %11 = sext i32 %.01 to i64
  %12 = getelementptr inbounds i32, i32* %0, i64 %11
  %13 = load i32, i32* %12, align 4
  %14 = sext i32 %.0 to i64
  %15 = getelementptr inbounds i32, i32* %0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = icmp slt i32 %13, %16
  br i1 %17, label %18, label %29

18:                                               ; preds = %10
  %19 = sext i32 %.01 to i64
  %20 = getelementptr inbounds i32, i32* %0, i64 %19
  %21 = load i32, i32* %20, align 4
  %22 = sext i32 %.0 to i64
  %23 = getelementptr inbounds i32, i32* %0, i64 %22
  %24 = load i32, i32* %23, align 4
  %25 = sext i32 %.01 to i64
  %26 = getelementptr inbounds i32, i32* %0, i64 %25
  store i32 %24, i32* %26, align 4
  %27 = sext i32 %.0 to i64
  %28 = getelementptr inbounds i32, i32* %0, i64 %27
  store i32 %21, i32* %28, align 4
  br label %29

29:                                               ; preds = %18, %10
  %30 = add nsw i32 %.0, 1
  br label %8

31:                                               ; preds = %8
  %32 = add nsw i32 %.01, 1
  br label %3

33:                                               ; preds = %3
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @param32_rec(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15, i32 %16, i32 %17, i32 %18, i32 %19, i32 %20, i32 %21, i32 %22, i32 %23, i32 %24, i32 %25, i32 %26, i32 %27, i32 %28, i32 %29, i32 %30, i32 %31) #0 {
  %33 = icmp eq i32 %0, 0
  br i1 %33, label %34, label %35

34:                                               ; preds = %32
  br label %40

35:                                               ; preds = %32
  %36 = sub nsw i32 %0, 1
  %37 = add nsw i32 %1, %2
  %38 = srem i32 %37, 998244353
  %39 = call i32 @param32_rec(i32 %36, i32 %38, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15, i32 %16, i32 %17, i32 %18, i32 %19, i32 %20, i32 %21, i32 %22, i32 %23, i32 %24, i32 %25, i32 %26, i32 %27, i32 %28, i32 %29, i32 %30, i32 %31, i32 0)
  br label %40

40:                                               ; preds = %35, %34
  %.0 = phi i32 [ %1, %34 ], [ %39, %35 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @param32_arr(i32* %0, i32* %1, i32* %2, i32* %3, i32* %4, i32* %5, i32* %6, i32* %7, i32* %8, i32* %9, i32* %10, i32* %11, i32* %12, i32* %13, i32* %14, i32* %15, i32* %16, i32* %17, i32* %18, i32* %19, i32* %20, i32* %21, i32* %22, i32* %23, i32* %24, i32* %25, i32* %26, i32* %27, i32* %28, i32* %29, i32* %30, i32* %31) #0 {
  %33 = getelementptr inbounds i32, i32* %0, i64 0
  %34 = load i32, i32* %33, align 4
  %35 = getelementptr inbounds i32, i32* %0, i64 1
  %36 = load i32, i32* %35, align 4
  %37 = add nsw i32 %34, %36
  %38 = getelementptr inbounds i32, i32* %1, i64 0
  %39 = load i32, i32* %38, align 4
  %40 = add nsw i32 %37, %39
  %41 = getelementptr inbounds i32, i32* %1, i64 1
  %42 = load i32, i32* %41, align 4
  %43 = add nsw i32 %40, %42
  %44 = getelementptr inbounds i32, i32* %2, i64 0
  %45 = load i32, i32* %44, align 4
  %46 = add nsw i32 %43, %45
  %47 = getelementptr inbounds i32, i32* %2, i64 1
  %48 = load i32, i32* %47, align 4
  %49 = add nsw i32 %46, %48
  %50 = getelementptr inbounds i32, i32* %3, i64 0
  %51 = load i32, i32* %50, align 4
  %52 = add nsw i32 %49, %51
  %53 = getelementptr inbounds i32, i32* %3, i64 1
  %54 = load i32, i32* %53, align 4
  %55 = add nsw i32 %52, %54
  %56 = getelementptr inbounds i32, i32* %4, i64 0
  %57 = load i32, i32* %56, align 4
  %58 = add nsw i32 %55, %57
  %59 = getelementptr inbounds i32, i32* %4, i64 1
  %60 = load i32, i32* %59, align 4
  %61 = add nsw i32 %58, %60
  %62 = getelementptr inbounds i32, i32* %5, i64 0
  %63 = load i32, i32* %62, align 4
  %64 = add nsw i32 %61, %63
  %65 = getelementptr inbounds i32, i32* %5, i64 1
  %66 = load i32, i32* %65, align 4
  %67 = add nsw i32 %64, %66
  %68 = getelementptr inbounds i32, i32* %6, i64 0
  %69 = load i32, i32* %68, align 4
  %70 = add nsw i32 %67, %69
  %71 = getelementptr inbounds i32, i32* %6, i64 1
  %72 = load i32, i32* %71, align 4
  %73 = add nsw i32 %70, %72
  %74 = getelementptr inbounds i32, i32* %7, i64 0
  %75 = load i32, i32* %74, align 4
  %76 = add nsw i32 %73, %75
  %77 = getelementptr inbounds i32, i32* %7, i64 1
  %78 = load i32, i32* %77, align 4
  %79 = add nsw i32 %76, %78
  %80 = getelementptr inbounds i32, i32* %8, i64 0
  %81 = load i32, i32* %80, align 4
  %82 = add nsw i32 %79, %81
  %83 = getelementptr inbounds i32, i32* %8, i64 1
  %84 = load i32, i32* %83, align 4
  %85 = add nsw i32 %82, %84
  %86 = getelementptr inbounds i32, i32* %9, i64 0
  %87 = load i32, i32* %86, align 4
  %88 = add nsw i32 %85, %87
  %89 = getelementptr inbounds i32, i32* %9, i64 1
  %90 = load i32, i32* %89, align 4
  %91 = add nsw i32 %88, %90
  %92 = getelementptr inbounds i32, i32* %10, i64 0
  %93 = load i32, i32* %92, align 4
  %94 = add nsw i32 %91, %93
  %95 = getelementptr inbounds i32, i32* %10, i64 1
  %96 = load i32, i32* %95, align 4
  %97 = add nsw i32 %94, %96
  %98 = getelementptr inbounds i32, i32* %11, i64 0
  %99 = load i32, i32* %98, align 4
  %100 = add nsw i32 %97, %99
  %101 = getelementptr inbounds i32, i32* %11, i64 1
  %102 = load i32, i32* %101, align 4
  %103 = add nsw i32 %100, %102
  %104 = getelementptr inbounds i32, i32* %12, i64 0
  %105 = load i32, i32* %104, align 4
  %106 = add nsw i32 %103, %105
  %107 = getelementptr inbounds i32, i32* %12, i64 1
  %108 = load i32, i32* %107, align 4
  %109 = add nsw i32 %106, %108
  %110 = getelementptr inbounds i32, i32* %13, i64 0
  %111 = load i32, i32* %110, align 4
  %112 = add nsw i32 %109, %111
  %113 = getelementptr inbounds i32, i32* %13, i64 1
  %114 = load i32, i32* %113, align 4
  %115 = add nsw i32 %112, %114
  %116 = getelementptr inbounds i32, i32* %14, i64 0
  %117 = load i32, i32* %116, align 4
  %118 = add nsw i32 %115, %117
  %119 = getelementptr inbounds i32, i32* %14, i64 1
  %120 = load i32, i32* %119, align 4
  %121 = add nsw i32 %118, %120
  %122 = getelementptr inbounds i32, i32* %15, i64 0
  %123 = load i32, i32* %122, align 4
  %124 = add nsw i32 %121, %123
  %125 = getelementptr inbounds i32, i32* %15, i64 1
  %126 = load i32, i32* %125, align 4
  %127 = add nsw i32 %124, %126
  %128 = getelementptr inbounds i32, i32* %16, i64 0
  %129 = load i32, i32* %128, align 4
  %130 = add nsw i32 %127, %129
  %131 = getelementptr inbounds i32, i32* %16, i64 1
  %132 = load i32, i32* %131, align 4
  %133 = add nsw i32 %130, %132
  %134 = getelementptr inbounds i32, i32* %17, i64 0
  %135 = load i32, i32* %134, align 4
  %136 = add nsw i32 %133, %135
  %137 = getelementptr inbounds i32, i32* %17, i64 1
  %138 = load i32, i32* %137, align 4
  %139 = add nsw i32 %136, %138
  %140 = getelementptr inbounds i32, i32* %18, i64 0
  %141 = load i32, i32* %140, align 4
  %142 = add nsw i32 %139, %141
  %143 = getelementptr inbounds i32, i32* %18, i64 1
  %144 = load i32, i32* %143, align 4
  %145 = add nsw i32 %142, %144
  %146 = getelementptr inbounds i32, i32* %19, i64 0
  %147 = load i32, i32* %146, align 4
  %148 = add nsw i32 %145, %147
  %149 = getelementptr inbounds i32, i32* %19, i64 1
  %150 = load i32, i32* %149, align 4
  %151 = add nsw i32 %148, %150
  %152 = getelementptr inbounds i32, i32* %20, i64 0
  %153 = load i32, i32* %152, align 4
  %154 = add nsw i32 %151, %153
  %155 = getelementptr inbounds i32, i32* %20, i64 1
  %156 = load i32, i32* %155, align 4
  %157 = add nsw i32 %154, %156
  %158 = getelementptr inbounds i32, i32* %21, i64 0
  %159 = load i32, i32* %158, align 4
  %160 = add nsw i32 %157, %159
  %161 = getelementptr inbounds i32, i32* %21, i64 1
  %162 = load i32, i32* %161, align 4
  %163 = add nsw i32 %160, %162
  %164 = getelementptr inbounds i32, i32* %22, i64 0
  %165 = load i32, i32* %164, align 4
  %166 = add nsw i32 %163, %165
  %167 = getelementptr inbounds i32, i32* %22, i64 1
  %168 = load i32, i32* %167, align 4
  %169 = add nsw i32 %166, %168
  %170 = getelementptr inbounds i32, i32* %23, i64 0
  %171 = load i32, i32* %170, align 4
  %172 = add nsw i32 %169, %171
  %173 = getelementptr inbounds i32, i32* %23, i64 1
  %174 = load i32, i32* %173, align 4
  %175 = add nsw i32 %172, %174
  %176 = getelementptr inbounds i32, i32* %24, i64 0
  %177 = load i32, i32* %176, align 4
  %178 = add nsw i32 %175, %177
  %179 = getelementptr inbounds i32, i32* %24, i64 1
  %180 = load i32, i32* %179, align 4
  %181 = add nsw i32 %178, %180
  %182 = getelementptr inbounds i32, i32* %25, i64 0
  %183 = load i32, i32* %182, align 4
  %184 = add nsw i32 %181, %183
  %185 = getelementptr inbounds i32, i32* %25, i64 1
  %186 = load i32, i32* %185, align 4
  %187 = add nsw i32 %184, %186
  %188 = getelementptr inbounds i32, i32* %26, i64 0
  %189 = load i32, i32* %188, align 4
  %190 = add nsw i32 %187, %189
  %191 = getelementptr inbounds i32, i32* %26, i64 1
  %192 = load i32, i32* %191, align 4
  %193 = add nsw i32 %190, %192
  %194 = getelementptr inbounds i32, i32* %27, i64 0
  %195 = load i32, i32* %194, align 4
  %196 = add nsw i32 %193, %195
  %197 = getelementptr inbounds i32, i32* %27, i64 1
  %198 = load i32, i32* %197, align 4
  %199 = add nsw i32 %196, %198
  %200 = getelementptr inbounds i32, i32* %28, i64 0
  %201 = load i32, i32* %200, align 4
  %202 = add nsw i32 %199, %201
  %203 = getelementptr inbounds i32, i32* %28, i64 1
  %204 = load i32, i32* %203, align 4
  %205 = add nsw i32 %202, %204
  %206 = getelementptr inbounds i32, i32* %29, i64 0
  %207 = load i32, i32* %206, align 4
  %208 = add nsw i32 %205, %207
  %209 = getelementptr inbounds i32, i32* %29, i64 1
  %210 = load i32, i32* %209, align 4
  %211 = add nsw i32 %208, %210
  %212 = getelementptr inbounds i32, i32* %30, i64 0
  %213 = load i32, i32* %212, align 4
  %214 = add nsw i32 %211, %213
  %215 = getelementptr inbounds i32, i32* %30, i64 1
  %216 = load i32, i32* %215, align 4
  %217 = add nsw i32 %214, %216
  %218 = getelementptr inbounds i32, i32* %31, i64 0
  %219 = load i32, i32* %218, align 4
  %220 = add nsw i32 %217, %219
  %221 = getelementptr inbounds i32, i32* %31, i64 1
  %222 = load i32, i32* %221, align 4
  %223 = add nsw i32 %220, %222
  ret i32 %223
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @param16(i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15) #0 {
  %17 = alloca [16 x i32], align 16
  %18 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 0
  store i32 %0, i32* %18, align 4
  %19 = getelementptr inbounds i32, i32* %18, i64 1
  store i32 %1, i32* %19, align 4
  %20 = getelementptr inbounds i32, i32* %19, i64 1
  store i32 %2, i32* %20, align 4
  %21 = getelementptr inbounds i32, i32* %20, i64 1
  store i32 %3, i32* %21, align 4
  %22 = getelementptr inbounds i32, i32* %21, i64 1
  store i32 %4, i32* %22, align 4
  %23 = getelementptr inbounds i32, i32* %22, i64 1
  store i32 %5, i32* %23, align 4
  %24 = getelementptr inbounds i32, i32* %23, i64 1
  store i32 %6, i32* %24, align 4
  %25 = getelementptr inbounds i32, i32* %24, i64 1
  store i32 %7, i32* %25, align 4
  %26 = getelementptr inbounds i32, i32* %25, i64 1
  store i32 %8, i32* %26, align 4
  %27 = getelementptr inbounds i32, i32* %26, i64 1
  store i32 %9, i32* %27, align 4
  %28 = getelementptr inbounds i32, i32* %27, i64 1
  store i32 %10, i32* %28, align 4
  %29 = getelementptr inbounds i32, i32* %28, i64 1
  store i32 %11, i32* %29, align 4
  %30 = getelementptr inbounds i32, i32* %29, i64 1
  store i32 %12, i32* %30, align 4
  %31 = getelementptr inbounds i32, i32* %30, i64 1
  store i32 %13, i32* %31, align 4
  %32 = getelementptr inbounds i32, i32* %31, i64 1
  store i32 %14, i32* %32, align 4
  %33 = getelementptr inbounds i32, i32* %32, i64 1
  store i32 %15, i32* %33, align 4
  %34 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 0
  call void @sort(i32* %34, i32 16)
  %35 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 0
  %36 = load i32, i32* %35, align 16
  %37 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 1
  %38 = load i32, i32* %37, align 4
  %39 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 2
  %40 = load i32, i32* %39, align 8
  %41 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 3
  %42 = load i32, i32* %41, align 4
  %43 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 4
  %44 = load i32, i32* %43, align 16
  %45 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 5
  %46 = load i32, i32* %45, align 4
  %47 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 6
  %48 = load i32, i32* %47, align 8
  %49 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 7
  %50 = load i32, i32* %49, align 4
  %51 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 8
  %52 = load i32, i32* %51, align 16
  %53 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 9
  %54 = load i32, i32* %53, align 4
  %55 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 10
  %56 = load i32, i32* %55, align 8
  %57 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 11
  %58 = load i32, i32* %57, align 4
  %59 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 12
  %60 = load i32, i32* %59, align 16
  %61 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 13
  %62 = load i32, i32* %61, align 4
  %63 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 14
  %64 = load i32, i32* %63, align 8
  %65 = getelementptr inbounds [16 x i32], [16 x i32]* %17, i64 0, i64 15
  %66 = load i32, i32* %65, align 4
  %67 = call i32 @param32_rec(i32 %36, i32 %38, i32 %40, i32 %42, i32 %44, i32 %46, i32 %48, i32 %50, i32 %52, i32 %54, i32 %56, i32 %58, i32 %60, i32 %62, i32 %64, i32 %66, i32 %0, i32 %1, i32 %2, i32 %3, i32 %4, i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15)
  ret i32 %67
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca [32 x [2 x i32]], align 16
  %2 = bitcast [32 x [2 x i32]]* %1 to i8*
  call void @llvm.memset.p0i8.i64(i8* align 16 %2, i8 0, i64 256, i1 false)
  %3 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 0
  %4 = getelementptr inbounds [2 x i32], [2 x i32]* %3, i64 0, i64 0
  %5 = call i32 (...) @getint()
  %6 = call i32 (...) @getint()
  %7 = call i32 (...) @getint()
  %8 = call i32 (...) @getint()
  %9 = call i32 (...) @getint()
  %10 = call i32 (...) @getint()
  %11 = call i32 (...) @getint()
  %12 = call i32 (...) @getint()
  %13 = call i32 (...) @getint()
  %14 = call i32 (...) @getint()
  %15 = call i32 (...) @getint()
  %16 = call i32 (...) @getint()
  %17 = call i32 (...) @getint()
  %18 = call i32 (...) @getint()
  %19 = call i32 (...) @getint()
  %20 = call i32 (...) @getint()
  %21 = call i32 @param16(i32 %5, i32 %6, i32 %7, i32 %8, i32 %9, i32 %10, i32 %11, i32 %12, i32 %13, i32 %14, i32 %15, i32 %16, i32 %17, i32 %18, i32 %19, i32 %20)
  store i32 %21, i32* %4, align 4
  %22 = getelementptr inbounds i32, i32* %4, i64 1
  store i32 8848, i32* %22, align 4
  br label %23

23:                                               ; preds = %25, %0
  %.0 = phi i32 [ 1, %0 ], [ %44, %25 ]
  %24 = icmp slt i32 %.0, 32
  br i1 %24, label %25, label %45

25:                                               ; preds = %23
  %26 = sub nsw i32 %.0, 1
  %27 = sext i32 %26 to i64
  %28 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 %27
  %29 = getelementptr inbounds [2 x i32], [2 x i32]* %28, i64 0, i64 1
  %30 = load i32, i32* %29, align 4
  %31 = sub nsw i32 %30, 1
  %32 = sext i32 %.0 to i64
  %33 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 %32
  %34 = getelementptr inbounds [2 x i32], [2 x i32]* %33, i64 0, i64 0
  store i32 %31, i32* %34, align 8
  %35 = sub nsw i32 %.0, 1
  %36 = sext i32 %35 to i64
  %37 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 %36
  %38 = getelementptr inbounds [2 x i32], [2 x i32]* %37, i64 0, i64 0
  %39 = load i32, i32* %38, align 8
  %40 = sub nsw i32 %39, 2
  %41 = sext i32 %.0 to i64
  %42 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 %41
  %43 = getelementptr inbounds [2 x i32], [2 x i32]* %42, i64 0, i64 1
  store i32 %40, i32* %43, align 4
  %44 = add nsw i32 %.0, 1
  br label %23

45:                                               ; preds = %23
  %46 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 0
  %47 = getelementptr inbounds [2 x i32], [2 x i32]* %46, i64 0, i64 0
  %48 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 1
  %49 = getelementptr inbounds [2 x i32], [2 x i32]* %48, i64 0, i64 0
  %50 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 2
  %51 = getelementptr inbounds [2 x i32], [2 x i32]* %50, i64 0, i64 0
  %52 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 3
  %53 = getelementptr inbounds [2 x i32], [2 x i32]* %52, i64 0, i64 0
  %54 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 4
  %55 = getelementptr inbounds [2 x i32], [2 x i32]* %54, i64 0, i64 0
  %56 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 5
  %57 = getelementptr inbounds [2 x i32], [2 x i32]* %56, i64 0, i64 0
  %58 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 6
  %59 = getelementptr inbounds [2 x i32], [2 x i32]* %58, i64 0, i64 0
  %60 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 7
  %61 = getelementptr inbounds [2 x i32], [2 x i32]* %60, i64 0, i64 0
  %62 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 8
  %63 = getelementptr inbounds [2 x i32], [2 x i32]* %62, i64 0, i64 0
  %64 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 9
  %65 = getelementptr inbounds [2 x i32], [2 x i32]* %64, i64 0, i64 0
  %66 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 10
  %67 = getelementptr inbounds [2 x i32], [2 x i32]* %66, i64 0, i64 0
  %68 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 11
  %69 = getelementptr inbounds [2 x i32], [2 x i32]* %68, i64 0, i64 0
  %70 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 12
  %71 = getelementptr inbounds [2 x i32], [2 x i32]* %70, i64 0, i64 0
  %72 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 13
  %73 = getelementptr inbounds [2 x i32], [2 x i32]* %72, i64 0, i64 0
  %74 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 14
  %75 = getelementptr inbounds [2 x i32], [2 x i32]* %74, i64 0, i64 0
  %76 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 15
  %77 = getelementptr inbounds [2 x i32], [2 x i32]* %76, i64 0, i64 0
  %78 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 16
  %79 = getelementptr inbounds [2 x i32], [2 x i32]* %78, i64 0, i64 0
  %80 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 17
  %81 = getelementptr inbounds [2 x i32], [2 x i32]* %80, i64 0, i64 0
  %82 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 18
  %83 = getelementptr inbounds [2 x i32], [2 x i32]* %82, i64 0, i64 0
  %84 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 19
  %85 = getelementptr inbounds [2 x i32], [2 x i32]* %84, i64 0, i64 0
  %86 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 20
  %87 = getelementptr inbounds [2 x i32], [2 x i32]* %86, i64 0, i64 0
  %88 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 21
  %89 = getelementptr inbounds [2 x i32], [2 x i32]* %88, i64 0, i64 0
  %90 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 22
  %91 = getelementptr inbounds [2 x i32], [2 x i32]* %90, i64 0, i64 0
  %92 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 23
  %93 = getelementptr inbounds [2 x i32], [2 x i32]* %92, i64 0, i64 0
  %94 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 24
  %95 = getelementptr inbounds [2 x i32], [2 x i32]* %94, i64 0, i64 0
  %96 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 25
  %97 = getelementptr inbounds [2 x i32], [2 x i32]* %96, i64 0, i64 0
  %98 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 26
  %99 = getelementptr inbounds [2 x i32], [2 x i32]* %98, i64 0, i64 0
  %100 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 27
  %101 = getelementptr inbounds [2 x i32], [2 x i32]* %100, i64 0, i64 0
  %102 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 28
  %103 = getelementptr inbounds [2 x i32], [2 x i32]* %102, i64 0, i64 0
  %104 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 29
  %105 = getelementptr inbounds [2 x i32], [2 x i32]* %104, i64 0, i64 0
  %106 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 30
  %107 = getelementptr inbounds [2 x i32], [2 x i32]* %106, i64 0, i64 0
  %108 = getelementptr inbounds [32 x [2 x i32]], [32 x [2 x i32]]* %1, i64 0, i64 31
  %109 = getelementptr inbounds [2 x i32], [2 x i32]* %108, i64 0, i64 0
  %110 = call i32 @param32_arr(i32* %47, i32* %49, i32* %51, i32* %53, i32* %55, i32* %57, i32* %59, i32* %61, i32* %63, i32* %65, i32* %67, i32* %69, i32* %71, i32* %73, i32* %75, i32* %77, i32* %79, i32* %81, i32* %83, i32* %85, i32* %87, i32* %89, i32* %91, i32* %93, i32* %95, i32* %97, i32* %99, i32* %101, i32* %103, i32* %105, i32* %107, i32* %109)
  %111 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %110)
  %112 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  ret i32 0
}

; Function Attrs: argmemonly nounwind willreturn writeonly
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1

declare dso_local i32 @getint(...) #2

declare dso_local i32 @putint(...) #2

declare dso_local i32 @putch(...) #2

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind willreturn writeonly }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
