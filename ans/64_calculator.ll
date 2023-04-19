; ModuleID = '64_calculator.bc'
source_filename = "64_calculator.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@i = dso_local global i32 0, align 4
@ii = dso_local global i32 1, align 4
@intt = common dso_local global i32 0, align 4
@ints = common dso_local global [10000 x i32] zeroinitializer, align 16
@chat = common dso_local global i32 0, align 4
@chas = common dso_local global [10000 x i32] zeroinitializer, align 16
@c = common dso_local global i32 0, align 4
@get2 = common dso_local global [10000 x i32] zeroinitializer, align 16
@get = common dso_local global [10000 x i32] zeroinitializer, align 16

; Function Attrs: noinline nounwind readonly uwtable
define dso_local i32 @isdigit(i32 %0) #0 {
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
define dso_local i32 @power(i32 %0, i32 %1) #1 {
  Br label %3

3:                                                ; preds = %5, %2
  %.01 = phi i32 [ %1, %2 ], [ %7, %5 ]
  %.0 = phi i32 [ 1, %2 ], [ %6, %5 ]
  %4 = icmp ne i32 %.01, 0
  Br i1 %4, label %5, label %8

5:                                                ; preds = %3
  %6 = mul nsw i32 %.0, %0
  %7 = sub nsw i32 %.01, 1
  Br label %3

8:                                                ; preds = %3
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @getstr(i32* %0) #1 {
  %2 = call i32 (...) @getch()
  Br label %3

3:                                                ; preds = %9, %1
  %.01 = phi i32 [ %2, %1 ], [ %13, %9 ]
  %.0 = phi i32 [ 0, %1 ], [ %12, %9 ]
  %4 = icmp ne i32 %.01, 13
  Br i1 %4, label %5, label %7

5:                                                ; preds = %3
  %6 = icmp ne i32 %.01, 10
  Br label %7

7:                                                ; preds = %5, %3
  %8 = phi i1 [ false, %3 ], [ %6, %5 ]
  Br i1 %8, label %9, label %14

9:                                                ; preds = %7
  %10 = sext i32 %.0 to i64
  %11 = getelementptr inbounds i32, i32* %0, i64 %10
  store i32 %.01, i32* %11, align 4
  %12 = add nsw i32 %.0, 1
  %13 = call i32 (...) @getch()
  Br label %3

14:                                               ; preds = %7
  ret i32 %.0
}

declare dso_local i32 @getch(...) #2

; Function Attrs: noinline nounwind uwtable
define dso_local void @intpush(i32 %0) #1 {
  %2 = load i32, i32* @intt, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* @intt, align 4
  %4 = load i32, i32* @intt, align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %5
  store i32 %0, i32* %6, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @chapush(i32 %0) #1 {
  %2 = load i32, i32* @chat, align 4
  %3 = add nsw i32 %2, 1
  store i32 %3, i32* @chat, align 4
  %4 = load i32, i32* @chat, align 4
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %5
  store i32 %0, i32* %6, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @intpop() #1 {
  %1 = load i32, i32* @intt, align 4
  %2 = sub nsw i32 %1, 1
  store i32 %2, i32* @intt, align 4
  %3 = load i32, i32* @intt, align 4
  %4 = add nsw i32 %3, 1
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %5
  %7 = load i32, i32* %6, align 4
  ret i32 %7
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @chapop() #1 {
  %1 = load i32, i32* @chat, align 4
  %2 = sub nsw i32 %1, 1
  store i32 %2, i32* @chat, align 4
  %3 = load i32, i32* @chat, align 4
  %4 = add nsw i32 %3, 1
  %5 = sext i32 %4 to i64
  %6 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %5
  %7 = load i32, i32* %6, align 4
  ret i32 %7
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @intadd(i32 %0) #1 {
  %2 = load i32, i32* @intt, align 4
  %3 = sext i32 %2 to i64
  %4 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %3
  %5 = load i32, i32* %4, align 4
  %6 = mul nsw i32 %5, 10
  %7 = load i32, i32* @intt, align 4
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %8
  store i32 %6, i32* %9, align 4
  %10 = load i32, i32* @intt, align 4
  %11 = sext i32 %10 to i64
  %12 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %11
  %13 = load i32, i32* %12, align 4
  %14 = add nsw i32 %13, %0
  %15 = load i32, i32* @intt, align 4
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [10000 x i32], [10000 x i32]* @ints, i64 0, i64 %16
  store i32 %14, i32* %17, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @find() #1 {
  %1 = call i32 @chapop()
  store i32 %1, i32* @c, align 4
  %2 = load i32, i32* @ii, align 4
  %3 = sext i32 %2 to i64
  %4 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %3
  store i32 32, i32* %4, align 4
  %5 = load i32, i32* @c, align 4
  %6 = load i32, i32* @ii, align 4
  %7 = add nsw i32 %6, 1
  %8 = sext i32 %7 to i64
  %9 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %8
  store i32 %5, i32* %9, align 4
  %10 = load i32, i32* @ii, align 4
  %11 = add nsw i32 %10, 2
  store i32 %11, i32* @ii, align 4
  %12 = load i32, i32* @chat, align 4
  %13 = icmp eq i32 %12, 0
  Br i1 %13, label %14, label %15

14:                                               ; preds = %0
  Br label %16

15:                                               ; preds = %0
  Br label %16

16:                                               ; preds = %15, %14
  %.0 = phi i32 [ 0, %14 ], [ 1, %15 ]
  ret i32 %.0
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #1 {
  store i32 0, i32* @intt, align 4
  store i32 0, i32* @chat, align 4
  %1 = call i32 @getstr(i32* getelementptr inbounds ([10000 x i32], [10000 x i32]* @get, i64 0, i64 0))
  Br label %2

2:                                                ; preds = %285, %0
  %3 = load i32, i32* @i, align 4
  %4 = icmp slt i32 %3, %1
  Br i1 %4, label %5, label %288

5:                                                ; preds = %2
  %6 = load i32, i32* @i, align 4
  %7 = sext i32 %6 to i64
  %8 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %7
  %9 = load i32, i32* %8, align 4
  %10 = call i32 @isdigit(i32 %9) #3
  %11 = icmp eq i32 %10, 1
  Br i1 %11, label %12, label %22

12:                                               ; preds = %5
  %13 = load i32, i32* @i, align 4
  %14 = sext i32 %13 to i64
  %15 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %14
  %16 = load i32, i32* %15, align 4
  %17 = load i32, i32* @ii, align 4
  %18 = sext i32 %17 to i64
  %19 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %18
  store i32 %16, i32* %19, align 4
  %20 = load i32, i32* @ii, align 4
  %21 = add nsw i32 %20, 1
  store i32 %21, i32* @ii, align 4
  Br label %285

22:                                               ; preds = %5
  %23 = load i32, i32* @i, align 4
  %24 = sext i32 %23 to i64
  %25 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %24
  %26 = load i32, i32* %25, align 4
  %27 = icmp eq i32 %26, 40
  Br i1 %27, label %28, label %29

28:                                               ; preds = %22
  call void @chapush(i32 40)
  Br label %29

29:                                               ; preds = %28, %22
  %30 = load i32, i32* @i, align 4
  %31 = sext i32 %30 to i64
  %32 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %31
  %33 = load i32, i32* %32, align 4
  %34 = icmp eq i32 %33, 94
  Br i1 %34, label %35, label %36

35:                                               ; preds = %29
  call void @chapush(i32 94)
  Br label %36

36:                                               ; preds = %35, %29
  %37 = load i32, i32* @i, align 4
  %38 = sext i32 %37 to i64
  %39 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %38
  %40 = load i32, i32* %39, align 4
  %41 = icmp eq i32 %40, 41
  Br i1 %41, label %42, label %60

42:                                               ; preds = %36
  %43 = call i32 @chapop()
  store i32 %43, i32* @c, align 4
  Br label %44

44:                                               ; preds = %47, %42
  %45 = load i32, i32* @c, align 4
  %46 = icmp ne i32 %45, 40
  Br i1 %46, label %47, label %59

47:                                               ; preds = %44
  %48 = load i32, i32* @ii, align 4
  %49 = sext i32 %48 to i64
  %50 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %49
  store i32 32, i32* %50, align 4
  %51 = load i32, i32* @c, align 4
  %52 = load i32, i32* @ii, align 4
  %53 = add nsw i32 %52, 1
  %54 = sext i32 %53 to i64
  %55 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %54
  store i32 %51, i32* %55, align 4
  %56 = load i32, i32* @ii, align 4
  %57 = add nsw i32 %56, 2
  store i32 %57, i32* @ii, align 4
  %58 = call i32 @chapop()
  store i32 %58, i32* @c, align 4
  Br label %44

59:                                               ; preds = %44
  Br label %60

60:                                               ; preds = %59, %36
  %61 = load i32, i32* @i, align 4
  %62 = sext i32 %61 to i64
  %63 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %62
  %64 = load i32, i32* %63, align 4
  %65 = icmp eq i32 %64, 43
  Br i1 %65, label %66, label %111

66:                                               ; preds = %60
  Br label %67

67:                                               ; preds = %109, %66
  %68 = load i32, i32* @chat, align 4
  %69 = sext i32 %68 to i64
  %70 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %69
  %71 = load i32, i32* %70, align 4
  %72 = icmp eq i32 %71, 43
  Br i1 %72, label %103, label %73

73:                                               ; preds = %67
  %74 = load i32, i32* @chat, align 4
  %75 = sext i32 %74 to i64
  %76 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %75
  %77 = load i32, i32* %76, align 4
  %78 = icmp eq i32 %77, 45
  Br i1 %78, label %103, label %79

79:                                               ; preds = %73
  %80 = load i32, i32* @chat, align 4
  %81 = sext i32 %80 to i64
  %82 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %81
  %83 = load i32, i32* %82, align 4
  %84 = icmp eq i32 %83, 42
  Br i1 %84, label %103, label %85

85:                                               ; preds = %79
  %86 = load i32, i32* @chat, align 4
  %87 = sext i32 %86 to i64
  %88 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %87
  %89 = load i32, i32* %88, align 4
  %90 = icmp eq i32 %89, 47
  Br i1 %90, label %103, label %91

91:                                               ; preds = %85
  %92 = load i32, i32* @chat, align 4
  %93 = sext i32 %92 to i64
  %94 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %93
  %95 = load i32, i32* %94, align 4
  %96 = icmp eq i32 %95, 37
  Br i1 %96, label %103, label %97

97:                                               ; preds = %91
  %98 = load i32, i32* @chat, align 4
  %99 = sext i32 %98 to i64
  %100 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %99
  %101 = load i32, i32* %100, align 4
  %102 = icmp eq i32 %101, 94
  Br label %103

103:                                              ; preds = %97, %91, %85, %79, %73, %67
  %104 = phi i1 [ true, %91 ], [ true, %85 ], [ true, %79 ], [ true, %73 ], [ true, %67 ], [ %102, %97 ]
  Br i1 %104, label %105, label %110

105:                                              ; preds = %103
  %106 = call i32 @find()
  %107 = icmp eq i32 %106, 0
  Br i1 %107, label %108, label %109

108:                                              ; preds = %105
  Br label %110

109:                                              ; preds = %105
  Br label %67

110:                                              ; preds = %108, %103
  call void @chapush(i32 43)
  Br label %111

111:                                              ; preds = %110, %60
  %112 = load i32, i32* @i, align 4
  %113 = sext i32 %112 to i64
  %114 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %113
  %115 = load i32, i32* %114, align 4
  %116 = icmp eq i32 %115, 45
  Br i1 %116, label %117, label %162

117:                                              ; preds = %111
  Br label %118

118:                                              ; preds = %160, %117
  %119 = load i32, i32* @chat, align 4
  %120 = sext i32 %119 to i64
  %121 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %120
  %122 = load i32, i32* %121, align 4
  %123 = icmp eq i32 %122, 43
  Br i1 %123, label %154, label %124

124:                                              ; preds = %118
  %125 = load i32, i32* @chat, align 4
  %126 = sext i32 %125 to i64
  %127 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %126
  %128 = load i32, i32* %127, align 4
  %129 = icmp eq i32 %128, 45
  Br i1 %129, label %154, label %130

130:                                              ; preds = %124
  %131 = load i32, i32* @chat, align 4
  %132 = sext i32 %131 to i64
  %133 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %132
  %134 = load i32, i32* %133, align 4
  %135 = icmp eq i32 %134, 42
  Br i1 %135, label %154, label %136

136:                                              ; preds = %130
  %137 = load i32, i32* @chat, align 4
  %138 = sext i32 %137 to i64
  %139 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %138
  %140 = load i32, i32* %139, align 4
  %141 = icmp eq i32 %140, 47
  Br i1 %141, label %154, label %142

142:                                              ; preds = %136
  %143 = load i32, i32* @chat, align 4
  %144 = sext i32 %143 to i64
  %145 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %144
  %146 = load i32, i32* %145, align 4
  %147 = icmp eq i32 %146, 37
  Br i1 %147, label %154, label %148

148:                                              ; preds = %142
  %149 = load i32, i32* @chat, align 4
  %150 = sext i32 %149 to i64
  %151 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %150
  %152 = load i32, i32* %151, align 4
  %153 = icmp eq i32 %152, 94
  Br label %154

154:                                              ; preds = %148, %142, %136, %130, %124, %118
  %155 = phi i1 [ true, %142 ], [ true, %136 ], [ true, %130 ], [ true, %124 ], [ true, %118 ], [ %153, %148 ]
  Br i1 %155, label %156, label %161

156:                                              ; preds = %154
  %157 = call i32 @find()
  %158 = icmp eq i32 %157, 0
  Br i1 %158, label %159, label %160

159:                                              ; preds = %156
  Br label %161

160:                                              ; preds = %156
  Br label %118

161:                                              ; preds = %159, %154
  call void @chapush(i32 45)
  Br label %162

162:                                              ; preds = %161, %111
  %163 = load i32, i32* @i, align 4
  %164 = sext i32 %163 to i64
  %165 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %164
  %166 = load i32, i32* %165, align 4
  %167 = icmp eq i32 %166, 42
  Br i1 %167, label %168, label %201

168:                                              ; preds = %162
  Br label %169

169:                                              ; preds = %199, %168
  %170 = load i32, i32* @chat, align 4
  %171 = sext i32 %170 to i64
  %172 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %171
  %173 = load i32, i32* %172, align 4
  %174 = icmp eq i32 %173, 42
  Br i1 %174, label %193, label %175

175:                                              ; preds = %169
  %176 = load i32, i32* @chat, align 4
  %177 = sext i32 %176 to i64
  %178 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %177
  %179 = load i32, i32* %178, align 4
  %180 = icmp eq i32 %179, 47
  Br i1 %180, label %193, label %181

181:                                              ; preds = %175
  %182 = load i32, i32* @chat, align 4
  %183 = sext i32 %182 to i64
  %184 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %183
  %185 = load i32, i32* %184, align 4
  %186 = icmp eq i32 %185, 37
  Br i1 %186, label %193, label %187

187:                                              ; preds = %181
  %188 = load i32, i32* @chat, align 4
  %189 = sext i32 %188 to i64
  %190 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %189
  %191 = load i32, i32* %190, align 4
  %192 = icmp eq i32 %191, 94
  Br label %193

193:                                              ; preds = %187, %181, %175, %169
  %194 = phi i1 [ true, %181 ], [ true, %175 ], [ true, %169 ], [ %192, %187 ]
  Br i1 %194, label %195, label %200

195:                                              ; preds = %193
  %196 = call i32 @find()
  %197 = icmp eq i32 %196, 0
  Br i1 %197, label %198, label %199

198:                                              ; preds = %195
  Br label %200

199:                                              ; preds = %195
  Br label %169

200:                                              ; preds = %198, %193
  call void @chapush(i32 42)
  Br label %201

201:                                              ; preds = %200, %162
  %202 = load i32, i32* @i, align 4
  %203 = sext i32 %202 to i64
  %204 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %203
  %205 = load i32, i32* %204, align 4
  %206 = icmp eq i32 %205, 47
  Br i1 %206, label %207, label %240

207:                                              ; preds = %201
  Br label %208

208:                                              ; preds = %238, %207
  %209 = load i32, i32* @chat, align 4
  %210 = sext i32 %209 to i64
  %211 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %210
  %212 = load i32, i32* %211, align 4
  %213 = icmp eq i32 %212, 42
  Br i1 %213, label %232, label %214

214:                                              ; preds = %208
  %215 = load i32, i32* @chat, align 4
  %216 = sext i32 %215 to i64
  %217 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %216
  %218 = load i32, i32* %217, align 4
  %219 = icmp eq i32 %218, 47
  Br i1 %219, label %232, label %220

220:                                              ; preds = %214
  %221 = load i32, i32* @chat, align 4
  %222 = sext i32 %221 to i64
  %223 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %222
  %224 = load i32, i32* %223, align 4
  %225 = icmp eq i32 %224, 37
  Br i1 %225, label %232, label %226

226:                                              ; preds = %220
  %227 = load i32, i32* @chat, align 4
  %228 = sext i32 %227 to i64
  %229 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %228
  %230 = load i32, i32* %229, align 4
  %231 = icmp eq i32 %230, 94
  Br label %232

232:                                              ; preds = %226, %220, %214, %208
  %233 = phi i1 [ true, %220 ], [ true, %214 ], [ true, %208 ], [ %231, %226 ]
  Br i1 %233, label %234, label %239

234:                                              ; preds = %232
  %235 = call i32 @find()
  %236 = icmp eq i32 %235, 0
  Br i1 %236, label %237, label %238

237:                                              ; preds = %234
  Br label %239

238:                                              ; preds = %234
  Br label %208

239:                                              ; preds = %237, %232
  call void @chapush(i32 47)
  Br label %240

240:                                              ; preds = %239, %201
  %241 = load i32, i32* @i, align 4
  %242 = sext i32 %241 to i64
  %243 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get, i64 0, i64 %242
  %244 = load i32, i32* %243, align 4
  %245 = icmp eq i32 %244, 37
  Br i1 %245, label %246, label %279

246:                                              ; preds = %240
  Br label %247

247:                                              ; preds = %277, %246
  %248 = load i32, i32* @chat, align 4
  %249 = sext i32 %248 to i64
  %250 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %249
  %251 = load i32, i32* %250, align 4
  %252 = icmp eq i32 %251, 42
  Br i1 %252, label %271, label %253

253:                                              ; preds = %247
  %254 = load i32, i32* @chat, align 4
  %255 = sext i32 %254 to i64
  %256 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %255
  %257 = load i32, i32* %256, align 4
  %258 = icmp eq i32 %257, 47
  Br i1 %258, label %271, label %259

259:                                              ; preds = %253
  %260 = load i32, i32* @chat, align 4
  %261 = sext i32 %260 to i64
  %262 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %261
  %263 = load i32, i32* %262, align 4
  %264 = icmp eq i32 %263, 37
  Br i1 %264, label %271, label %265

265:                                              ; preds = %259
  %266 = load i32, i32* @chat, align 4
  %267 = sext i32 %266 to i64
  %268 = getelementptr inbounds [10000 x i32], [10000 x i32]* @chas, i64 0, i64 %267
  %269 = load i32, i32* %268, align 4
  %270 = icmp eq i32 %269, 94
  Br label %271

271:                                              ; preds = %265, %259, %253, %247
  %272 = phi i1 [ true, %259 ], [ true, %253 ], [ true, %247 ], [ %270, %265 ]
  Br i1 %272, label %273, label %278

273:                                              ; preds = %271
  %274 = call i32 @find()
  %275 = icmp eq i32 %274, 0
  Br i1 %275, label %276, label %277

276:                                              ; preds = %273
  Br label %278

277:                                              ; preds = %273
  Br label %247

278:                                              ; preds = %276, %271
  call void @chapush(i32 37)
  Br label %279

279:                                              ; preds = %278, %240
  %280 = load i32, i32* @ii, align 4
  %281 = sext i32 %280 to i64
  %282 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %281
  store i32 32, i32* %282, align 4
  %283 = load i32, i32* @ii, align 4
  %284 = add nsw i32 %283, 1
  store i32 %284, i32* @ii, align 4
  Br label %285

285:                                              ; preds = %279, %12
  %286 = load i32, i32* @i, align 4
  %287 = add nsw i32 %286, 1
  store i32 %287, i32* @i, align 4
  Br label %2

288:                                              ; preds = %2
  Br label %289

289:                                              ; preds = %292, %288
  %290 = load i32, i32* @chat, align 4
  %291 = icmp sgt i32 %290, 0
  Br i1 %291, label %292, label %303

292:                                              ; preds = %289
  %293 = call i32 @chapop()
  %294 = load i32, i32* @ii, align 4
  %295 = sext i32 %294 to i64
  %296 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %295
  store i32 32, i32* %296, align 4
  %297 = load i32, i32* @ii, align 4
  %298 = add nsw i32 %297, 1
  %299 = sext i32 %298 to i64
  %300 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %299
  store i32 %293, i32* %300, align 4
  %301 = load i32, i32* @ii, align 4
  %302 = add nsw i32 %301, 2
  store i32 %302, i32* @ii, align 4
  Br label %289

303:                                              ; preds = %289
  %304 = load i32, i32* @ii, align 4
  %305 = sext i32 %304 to i64
  %306 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %305
  store i32 64, i32* %306, align 4
  store i32 1, i32* @i, align 4
  Br label %307

307:                                              ; preds = %436, %303
  %.0 = phi i32 [ undef, %303 ], [ %.7, %436 ]
  %308 = load i32, i32* @i, align 4
  %309 = sext i32 %308 to i64
  %310 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %309
  %311 = load i32, i32* %310, align 4
  %312 = icmp ne i32 %311, 64
  Br i1 %312, label %313, label %439

313:                                              ; preds = %307
  %314 = load i32, i32* @i, align 4
  %315 = sext i32 %314 to i64
  %316 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %315
  %317 = load i32, i32* %316, align 4
  %318 = icmp eq i32 %317, 43
  Br i1 %318, label %349, label %319

319:                                              ; preds = %313
  %320 = load i32, i32* @i, align 4
  %321 = sext i32 %320 to i64
  %322 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %321
  %323 = load i32, i32* %322, align 4
  %324 = icmp eq i32 %323, 45
  Br i1 %324, label %349, label %325

325:                                              ; preds = %319
  %326 = load i32, i32* @i, align 4
  %327 = sext i32 %326 to i64
  %328 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %327
  %329 = load i32, i32* %328, align 4
  %330 = icmp eq i32 %329, 42
  Br i1 %330, label %349, label %331

331:                                              ; preds = %325
  %332 = load i32, i32* @i, align 4
  %333 = sext i32 %332 to i64
  %334 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %333
  %335 = load i32, i32* %334, align 4
  %336 = icmp eq i32 %335, 47
  Br i1 %336, label %349, label %337

337:                                              ; preds = %331
  %338 = load i32, i32* @i, align 4
  %339 = sext i32 %338 to i64
  %340 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %339
  %341 = load i32, i32* %340, align 4
  %342 = icmp eq i32 %341, 37
  Br i1 %342, label %349, label %343

343:                                              ; preds = %337
  %344 = load i32, i32* @i, align 4
  %345 = sext i32 %344 to i64
  %346 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %345
  %347 = load i32, i32* %346, align 4
  %348 = icmp eq i32 %347, 94
  Br i1 %348, label %349, label %400

349:                                              ; preds = %343, %337, %331, %325, %319, %313
  %350 = call i32 @intpop()
  %351 = call i32 @intpop()
  %352 = load i32, i32* @i, align 4
  %353 = sext i32 %352 to i64
  %354 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %353
  %355 = load i32, i32* %354, align 4
  %356 = icmp eq i32 %355, 43
  Br i1 %356, label %357, label %359

357:                                              ; preds = %349
  %358 = add nsw i32 %350, %351
  Br label %359

359:                                              ; preds = %357, %349
  %.1 = phi i32 [ %358, %357 ], [ %.0, %349 ]
  %360 = load i32, i32* @i, align 4
  %361 = sext i32 %360 to i64
  %362 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %361
  %363 = load i32, i32* %362, align 4
  %364 = icmp eq i32 %363, 45
  Br i1 %364, label %365, label %367

365:                                              ; preds = %359
  %366 = sub nsw i32 %351, %350
  Br label %367

367:                                              ; preds = %365, %359
  %.2 = phi i32 [ %366, %365 ], [ %.1, %359 ]
  %368 = load i32, i32* @i, align 4
  %369 = sext i32 %368 to i64
  %370 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %369
  %371 = load i32, i32* %370, align 4
  %372 = icmp eq i32 %371, 42
  Br i1 %372, label %373, label %375

373:                                              ; preds = %367
  %374 = mul nsw i32 %350, %351
  Br label %375

375:                                              ; preds = %373, %367
  %.3 = phi i32 [ %374, %373 ], [ %.2, %367 ]
  %376 = load i32, i32* @i, align 4
  %377 = sext i32 %376 to i64
  %378 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %377
  %379 = load i32, i32* %378, align 4
  %380 = icmp eq i32 %379, 47
  Br i1 %380, label %381, label %383

381:                                              ; preds = %375
  %382 = sdiv i32 %351, %350
  Br label %383

383:                                              ; preds = %381, %375
  %.4 = phi i32 [ %382, %381 ], [ %.3, %375 ]
  %384 = load i32, i32* @i, align 4
  %385 = sext i32 %384 to i64
  %386 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %385
  %387 = load i32, i32* %386, align 4
  %388 = icmp eq i32 %387, 37
  Br i1 %388, label %389, label %391

389:                                              ; preds = %383
  %390 = srem i32 %351, %350
  Br label %391

391:                                              ; preds = %389, %383
  %.5 = phi i32 [ %390, %389 ], [ %.4, %383 ]
  %392 = load i32, i32* @i, align 4
  %393 = sext i32 %392 to i64
  %394 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %393
  %395 = load i32, i32* %394, align 4
  %396 = icmp eq i32 %395, 94
  Br i1 %396, label %397, label %399

397:                                              ; preds = %391
  %398 = call i32 @power(i32 %351, i32 %350)
  Br label %399

399:                                              ; preds = %397, %391
  %.6 = phi i32 [ %398, %397 ], [ %.5, %391 ]
  call void @intpush(i32 %.6)
  Br label %436

400:                                              ; preds = %343
  %401 = load i32, i32* @i, align 4
  %402 = sext i32 %401 to i64
  %403 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %402
  %404 = load i32, i32* %403, align 4
  %405 = icmp ne i32 %404, 32
  Br i1 %405, label %406, label %435

406:                                              ; preds = %400
  %407 = load i32, i32* @i, align 4
  %408 = sext i32 %407 to i64
  %409 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %408
  %410 = load i32, i32* %409, align 4
  %411 = sub nsw i32 %410, 48
  call void @intpush(i32 %411)
  store i32 1, i32* @ii, align 4
  Br label %412

412:                                              ; preds = %420, %406
  %413 = load i32, i32* @i, align 4
  %414 = load i32, i32* @ii, align 4
  %415 = add nsw i32 %413, %414
  %416 = sext i32 %415 to i64
  %417 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %416
  %418 = load i32, i32* %417, align 4
  %419 = icmp ne i32 %418, 32
  Br i1 %419, label %420, label %430

420:                                              ; preds = %412
  %421 = load i32, i32* @i, align 4
  %422 = load i32, i32* @ii, align 4
  %423 = add nsw i32 %421, %422
  %424 = sext i32 %423 to i64
  %425 = getelementptr inbounds [10000 x i32], [10000 x i32]* @get2, i64 0, i64 %424
  %426 = load i32, i32* %425, align 4
  %427 = sub nsw i32 %426, 48
  call void @intadd(i32 %427)
  %428 = load i32, i32* @ii, align 4
  %429 = add nsw i32 %428, 1
  store i32 %429, i32* @ii, align 4
  Br label %412

430:                                              ; preds = %412
  %431 = load i32, i32* @i, align 4
  %432 = load i32, i32* @ii, align 4
  %433 = add nsw i32 %431, %432
  %434 = sub nsw i32 %433, 1
  store i32 %434, i32* @i, align 4
  Br label %435

435:                                              ; preds = %430, %400
  Br label %436

436:                                              ; preds = %435, %399
  %.7 = phi i32 [ %.6, %399 ], [ %.0, %435 ]
  %437 = load i32, i32* @i, align 4
  %438 = add nsw i32 %437, 1
  store i32 %438, i32* @i, align 4
  Br label %307

439:                                              ; preds = %307
  %440 = load i32, i32* getelementptr inbounds ([10000 x i32], [10000 x i32]* @ints, i64 0, i64 1), align 4
  %441 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %440)
  ret i32 0
}

declare dso_local i32 @putint(...) #2

attributes #0 = { noinline nounwind readonly uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind readonly }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}
