; ModuleID = '82_long_func.bc'
source_filename = "82_long_func.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@SHIFT_TABLE = dso_local constant [16 x i32] [i32 1, i32 2, i32 4, i32 8, i32 16, i32 32, i32 64, i32 128, i32 256, i32 512, i32 1024, i32 2048, i32 4096, i32 8192, i32 16384, i32 32768], align 16

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @long_func() #0 {
  br label %1

1:                                                ; preds = %466, %0
  %.0283 = phi i32 [ 1, %0 ], [ %.1284, %466 ]
  %.0282 = phi i32 [ 0, %0 ], [ %.49, %466 ]
  %.0281 = phi i32 [ 2, %0 ], [ %.0297, %466 ]
  %2 = icmp sgt i32 %.0282, 0
  br i1 %2, label %3, label %467

3:                                                ; preds = %1
  br label %4

4:                                                ; preds = %18, %3
  %.0219 = phi i32 [ 1, %3 ], [ %20, %18 ]
  %.0159 = phi i32 [ %.0282, %3 ], [ %19, %18 ]
  %.02 = phi i32 [ 0, %3 ], [ %21, %18 ]
  %.01 = phi i32 [ 0, %3 ], [ %.1, %18 ]
  %5 = icmp slt i32 %.02, 16
  br i1 %5, label %6, label %22

6:                                                ; preds = %4
  %7 = srem i32 %.0159, 2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %18

9:                                                ; preds = %6
  %10 = srem i32 %.0219, 2
  %11 = icmp ne i32 %10, 0
  br i1 %11, label %12, label %18

12:                                               ; preds = %9
  %13 = sext i32 %.02 to i64
  %14 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %13
  %15 = load i32, i32* %14, align 4
  %16 = mul nsw i32 1, %15
  %17 = add nsw i32 %.01, %16
  br label %18

18:                                               ; preds = %12, %9, %6
  %.1 = phi i32 [ %17, %12 ], [ %.01, %9 ], [ %.01, %6 ]
  %19 = sdiv i32 %.0159, 2
  %20 = sdiv i32 %.0219, 2
  %21 = add nsw i32 %.02, 1
  br label %4

22:                                               ; preds = %4
  %23 = icmp ne i32 %.01, 0
  br i1 %23, label %24, label %230

24:                                               ; preds = %22
  br label %25

25:                                               ; preds = %228, %24
  %.0287 = phi i32 [ 0, %24 ], [ %.1288, %228 ]
  %.0286 = phi i32 [ %.0281, %24 ], [ %.23, %228 ]
  %.0285 = phi i32 [ %.0283, %24 ], [ %.0292, %228 ]
  %26 = icmp ne i32 %.0286, 0
  br i1 %26, label %27, label %229

27:                                               ; preds = %25
  br label %28

28:                                               ; preds = %42, %27
  %.1220 = phi i32 [ 1, %27 ], [ %44, %42 ]
  %.1160 = phi i32 [ %.0286, %27 ], [ %43, %42 ]
  %.1100 = phi i32 [ 0, %27 ], [ %45, %42 ]
  %.2 = phi i32 [ 0, %27 ], [ %.3, %42 ]
  %29 = icmp slt i32 %.1100, 16
  br i1 %29, label %30, label %46

30:                                               ; preds = %28
  %31 = srem i32 %.1160, 2
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %33, label %42

33:                                               ; preds = %30
  %34 = srem i32 %.1220, 2
  %35 = icmp ne i32 %34, 0
  br i1 %35, label %36, label %42

36:                                               ; preds = %33
  %37 = sext i32 %.1100 to i64
  %38 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %37
  %39 = load i32, i32* %38, align 4
  %40 = mul nsw i32 1, %39
  %41 = add nsw i32 %.2, %40
  br label %42

42:                                               ; preds = %36, %33, %30
  %.3 = phi i32 [ %41, %36 ], [ %.2, %33 ], [ %.2, %30 ]
  %43 = sdiv i32 %.1160, 2
  %44 = sdiv i32 %.1220, 2
  %45 = add nsw i32 %.1100, 1
  br label %28

46:                                               ; preds = %28
  %47 = icmp ne i32 %.2, 0
  br i1 %47, label %48, label %123

48:                                               ; preds = %46
  br label %49

49:                                               ; preds = %121, %48
  %.0290 = phi i32 [ %.0285, %48 ], [ %.10, %121 ]
  %.0289 = phi i32 [ %.0287, %48 ], [ %.4, %121 ]
  %50 = icmp ne i32 %.0290, 0
  br i1 %50, label %51, label %122

51:                                               ; preds = %49
  br label %52

52:                                               ; preds = %77, %51
  %.2221 = phi i32 [ %.0290, %51 ], [ %79, %77 ]
  %.2161 = phi i32 [ %.0289, %51 ], [ %78, %77 ]
  %.2101 = phi i32 [ 0, %51 ], [ %80, %77 ]
  %.4 = phi i32 [ 0, %51 ], [ %.7, %77 ]
  %53 = icmp slt i32 %.2101, 16
  br i1 %53, label %54, label %81

54:                                               ; preds = %52
  %55 = srem i32 %.2161, 2
  %56 = icmp ne i32 %55, 0
  br i1 %56, label %57, label %67

57:                                               ; preds = %54
  %58 = srem i32 %.2221, 2
  %59 = icmp eq i32 %58, 0
  br i1 %59, label %60, label %66

60:                                               ; preds = %57
  %61 = sext i32 %.2101 to i64
  %62 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %61
  %63 = load i32, i32* %62, align 4
  %64 = mul nsw i32 1, %63
  %65 = add nsw i32 %.4, %64
  br label %66

66:                                               ; preds = %60, %57
  %.5 = phi i32 [ %65, %60 ], [ %.4, %57 ]
  br label %77

67:                                               ; preds = %54
  %68 = srem i32 %.2221, 2
  %69 = icmp ne i32 %68, 0
  br i1 %69, label %70, label %76

70:                                               ; preds = %67
  %71 = sext i32 %.2101 to i64
  %72 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %71
  %73 = load i32, i32* %72, align 4
  %74 = mul nsw i32 1, %73
  %75 = add nsw i32 %.4, %74
  br label %76

76:                                               ; preds = %70, %67
  %.6 = phi i32 [ %75, %70 ], [ %.4, %67 ]
  br label %77

77:                                               ; preds = %76, %66
  %.7 = phi i32 [ %.5, %66 ], [ %.6, %76 ]
  %78 = sdiv i32 %.2161, 2
  %79 = sdiv i32 %.2221, 2
  %80 = add nsw i32 %.2101, 1
  br label %52

81:                                               ; preds = %52
  br label %82

82:                                               ; preds = %96, %81
  %.3222 = phi i32 [ %.0290, %81 ], [ %98, %96 ]
  %.3162 = phi i32 [ %.0289, %81 ], [ %97, %96 ]
  %.3102 = phi i32 [ 0, %81 ], [ %99, %96 ]
  %.8 = phi i32 [ 0, %81 ], [ %.9, %96 ]
  %83 = icmp slt i32 %.3102, 16
  br i1 %83, label %84, label %100

84:                                               ; preds = %82
  %85 = srem i32 %.3162, 2
  %86 = icmp ne i32 %85, 0
  br i1 %86, label %87, label %96

87:                                               ; preds = %84
  %88 = srem i32 %.3222, 2
  %89 = icmp ne i32 %88, 0
  br i1 %89, label %90, label %96

90:                                               ; preds = %87
  %91 = sext i32 %.3102 to i64
  %92 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %91
  %93 = load i32, i32* %92, align 4
  %94 = mul nsw i32 1, %93
  %95 = add nsw i32 %.8, %94
  br label %96

96:                                               ; preds = %90, %87, %84
  %.9 = phi i32 [ %95, %90 ], [ %.8, %87 ], [ %.8, %84 ]
  %97 = sdiv i32 %.3162, 2
  %98 = sdiv i32 %.3222, 2
  %99 = add nsw i32 %.3102, 1
  br label %82

100:                                              ; preds = %82
  %101 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %102 = mul nsw i32 %.8, %101
  br label %103

103:                                              ; preds = %117, %100
  %.4223 = phi i32 [ 65535, %100 ], [ %119, %117 ]
  %.4163 = phi i32 [ %102, %100 ], [ %118, %117 ]
  %.4103 = phi i32 [ 0, %100 ], [ %120, %117 ]
  %.10 = phi i32 [ 0, %100 ], [ %.11, %117 ]
  %104 = icmp slt i32 %.4103, 16
  br i1 %104, label %105, label %121

105:                                              ; preds = %103
  %106 = srem i32 %.4163, 2
  %107 = icmp ne i32 %106, 0
  br i1 %107, label %108, label %117

108:                                              ; preds = %105
  %109 = srem i32 %.4223, 2
  %110 = icmp ne i32 %109, 0
  br i1 %110, label %111, label %117

111:                                              ; preds = %108
  %112 = sext i32 %.4103 to i64
  %113 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %112
  %114 = load i32, i32* %113, align 4
  %115 = mul nsw i32 1, %114
  %116 = add nsw i32 %.10, %115
  br label %117

117:                                              ; preds = %111, %108, %105
  %.11 = phi i32 [ %116, %111 ], [ %.10, %108 ], [ %.10, %105 ]
  %118 = sdiv i32 %.4163, 2
  %119 = sdiv i32 %.4223, 2
  %120 = add nsw i32 %.4103, 1
  br label %103

121:                                              ; preds = %103
  br label %49

122:                                              ; preds = %49
  br label %123

123:                                              ; preds = %122, %46
  %.1288 = phi i32 [ %.0289, %122 ], [ %.0287, %46 ]
  br label %124

124:                                              ; preds = %196, %123
  %.0293 = phi i32 [ %.0285, %123 ], [ %.18, %196 ]
  %.0292 = phi i32 [ %.0285, %123 ], [ %.12, %196 ]
  %125 = icmp ne i32 %.0293, 0
  br i1 %125, label %126, label %197

126:                                              ; preds = %124
  br label %127

127:                                              ; preds = %152, %126
  %.5224 = phi i32 [ %.0293, %126 ], [ %154, %152 ]
  %.5164 = phi i32 [ %.0292, %126 ], [ %153, %152 ]
  %.5104 = phi i32 [ 0, %126 ], [ %155, %152 ]
  %.12 = phi i32 [ 0, %126 ], [ %.15, %152 ]
  %128 = icmp slt i32 %.5104, 16
  br i1 %128, label %129, label %156

129:                                              ; preds = %127
  %130 = srem i32 %.5164, 2
  %131 = icmp ne i32 %130, 0
  br i1 %131, label %132, label %142

132:                                              ; preds = %129
  %133 = srem i32 %.5224, 2
  %134 = icmp eq i32 %133, 0
  br i1 %134, label %135, label %141

135:                                              ; preds = %132
  %136 = sext i32 %.5104 to i64
  %137 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %136
  %138 = load i32, i32* %137, align 4
  %139 = mul nsw i32 1, %138
  %140 = add nsw i32 %.12, %139
  br label %141

141:                                              ; preds = %135, %132
  %.13 = phi i32 [ %140, %135 ], [ %.12, %132 ]
  br label %152

142:                                              ; preds = %129
  %143 = srem i32 %.5224, 2
  %144 = icmp ne i32 %143, 0
  br i1 %144, label %145, label %151

145:                                              ; preds = %142
  %146 = sext i32 %.5104 to i64
  %147 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %146
  %148 = load i32, i32* %147, align 4
  %149 = mul nsw i32 1, %148
  %150 = add nsw i32 %.12, %149
  br label %151

151:                                              ; preds = %145, %142
  %.14 = phi i32 [ %150, %145 ], [ %.12, %142 ]
  br label %152

152:                                              ; preds = %151, %141
  %.15 = phi i32 [ %.13, %141 ], [ %.14, %151 ]
  %153 = sdiv i32 %.5164, 2
  %154 = sdiv i32 %.5224, 2
  %155 = add nsw i32 %.5104, 1
  br label %127

156:                                              ; preds = %127
  br label %157

157:                                              ; preds = %171, %156
  %.6225 = phi i32 [ %.0293, %156 ], [ %173, %171 ]
  %.6165 = phi i32 [ %.0292, %156 ], [ %172, %171 ]
  %.6105 = phi i32 [ 0, %156 ], [ %174, %171 ]
  %.16 = phi i32 [ 0, %156 ], [ %.17, %171 ]
  %158 = icmp slt i32 %.6105, 16
  br i1 %158, label %159, label %175

159:                                              ; preds = %157
  %160 = srem i32 %.6165, 2
  %161 = icmp ne i32 %160, 0
  br i1 %161, label %162, label %171

162:                                              ; preds = %159
  %163 = srem i32 %.6225, 2
  %164 = icmp ne i32 %163, 0
  br i1 %164, label %165, label %171

165:                                              ; preds = %162
  %166 = sext i32 %.6105 to i64
  %167 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %166
  %168 = load i32, i32* %167, align 4
  %169 = mul nsw i32 1, %168
  %170 = add nsw i32 %.16, %169
  br label %171

171:                                              ; preds = %165, %162, %159
  %.17 = phi i32 [ %170, %165 ], [ %.16, %162 ], [ %.16, %159 ]
  %172 = sdiv i32 %.6165, 2
  %173 = sdiv i32 %.6225, 2
  %174 = add nsw i32 %.6105, 1
  br label %157

175:                                              ; preds = %157
  %176 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %177 = mul nsw i32 %.16, %176
  br label %178

178:                                              ; preds = %192, %175
  %.7226 = phi i32 [ 65535, %175 ], [ %194, %192 ]
  %.7166 = phi i32 [ %177, %175 ], [ %193, %192 ]
  %.7106 = phi i32 [ 0, %175 ], [ %195, %192 ]
  %.18 = phi i32 [ 0, %175 ], [ %.19, %192 ]
  %179 = icmp slt i32 %.7106, 16
  br i1 %179, label %180, label %196

180:                                              ; preds = %178
  %181 = srem i32 %.7166, 2
  %182 = icmp ne i32 %181, 0
  br i1 %182, label %183, label %192

183:                                              ; preds = %180
  %184 = srem i32 %.7226, 2
  %185 = icmp ne i32 %184, 0
  br i1 %185, label %186, label %192

186:                                              ; preds = %183
  %187 = sext i32 %.7106 to i64
  %188 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %187
  %189 = load i32, i32* %188, align 4
  %190 = mul nsw i32 1, %189
  %191 = add nsw i32 %.18, %190
  br label %192

192:                                              ; preds = %186, %183, %180
  %.19 = phi i32 [ %191, %186 ], [ %.18, %183 ], [ %.18, %180 ]
  %193 = sdiv i32 %.7166, 2
  %194 = sdiv i32 %.7226, 2
  %195 = add nsw i32 %.7106, 1
  br label %178

196:                                              ; preds = %178
  br label %124

197:                                              ; preds = %124
  %198 = icmp sge i32 1, 15
  br i1 %198, label %199, label %204

199:                                              ; preds = %197
  %200 = icmp slt i32 %.0286, 0
  br i1 %200, label %201, label %202

201:                                              ; preds = %199
  br label %203

202:                                              ; preds = %199
  br label %203

203:                                              ; preds = %202, %201
  %.20 = phi i32 [ 65535, %201 ], [ 0, %202 ]
  br label %228

204:                                              ; preds = %197
  %205 = icmp sgt i32 1, 0
  br i1 %205, label %206, label %226

206:                                              ; preds = %204
  %207 = icmp sgt i32 %.0286, 32767
  br i1 %207, label %208, label %220

208:                                              ; preds = %206
  %209 = sext i32 1 to i64
  %210 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %209
  %211 = load i32, i32* %210, align 4
  %212 = sdiv i32 %.0286, %211
  %213 = add nsw i32 %212, 65536
  %214 = sub nsw i32 15, 1
  %215 = add nsw i32 %214, 1
  %216 = sext i32 %215 to i64
  %217 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %216
  %218 = load i32, i32* %217, align 4
  %219 = sub nsw i32 %213, %218
  br label %225

220:                                              ; preds = %206
  %221 = sext i32 1 to i64
  %222 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %221
  %223 = load i32, i32* %222, align 4
  %224 = sdiv i32 %.0286, %223
  br label %225

225:                                              ; preds = %220, %208
  %.21 = phi i32 [ %219, %208 ], [ %224, %220 ]
  br label %227

226:                                              ; preds = %204
  br label %227

227:                                              ; preds = %226, %225
  %.22 = phi i32 [ %.21, %225 ], [ %.0286, %226 ]
  br label %228

228:                                              ; preds = %227, %203
  %.23 = phi i32 [ %.20, %203 ], [ %.22, %227 ]
  br label %25

229:                                              ; preds = %25
  br label %230

230:                                              ; preds = %229, %22
  %.1284 = phi i32 [ %.0287, %229 ], [ %.0283, %22 ]
  br label %231

231:                                              ; preds = %434, %230
  %.0297 = phi i32 [ 0, %230 ], [ %.1298, %434 ]
  %.0296 = phi i32 [ %.0281, %230 ], [ %.45, %434 ]
  %.0295 = phi i32 [ %.0281, %230 ], [ %.0302, %434 ]
  %232 = icmp ne i32 %.0296, 0
  br i1 %232, label %233, label %435

233:                                              ; preds = %231
  br label %234

234:                                              ; preds = %248, %233
  %.8227 = phi i32 [ 1, %233 ], [ %250, %248 ]
  %.8167 = phi i32 [ %.0296, %233 ], [ %249, %248 ]
  %.8107 = phi i32 [ 0, %233 ], [ %251, %248 ]
  %.24 = phi i32 [ 0, %233 ], [ %.25, %248 ]
  %235 = icmp slt i32 %.8107, 16
  br i1 %235, label %236, label %252

236:                                              ; preds = %234
  %237 = srem i32 %.8167, 2
  %238 = icmp ne i32 %237, 0
  br i1 %238, label %239, label %248

239:                                              ; preds = %236
  %240 = srem i32 %.8227, 2
  %241 = icmp ne i32 %240, 0
  br i1 %241, label %242, label %248

242:                                              ; preds = %239
  %243 = sext i32 %.8107 to i64
  %244 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %243
  %245 = load i32, i32* %244, align 4
  %246 = mul nsw i32 1, %245
  %247 = add nsw i32 %.24, %246
  br label %248

248:                                              ; preds = %242, %239, %236
  %.25 = phi i32 [ %247, %242 ], [ %.24, %239 ], [ %.24, %236 ]
  %249 = sdiv i32 %.8167, 2
  %250 = sdiv i32 %.8227, 2
  %251 = add nsw i32 %.8107, 1
  br label %234

252:                                              ; preds = %234
  %253 = icmp ne i32 %.24, 0
  br i1 %253, label %254, label %329

254:                                              ; preds = %252
  br label %255

255:                                              ; preds = %327, %254
  %.0300 = phi i32 [ %.0295, %254 ], [ %.32, %327 ]
  %.0299 = phi i32 [ %.0297, %254 ], [ %.26, %327 ]
  %256 = icmp ne i32 %.0300, 0
  br i1 %256, label %257, label %328

257:                                              ; preds = %255
  br label %258

258:                                              ; preds = %283, %257
  %.9228 = phi i32 [ %.0300, %257 ], [ %285, %283 ]
  %.9168 = phi i32 [ %.0299, %257 ], [ %284, %283 ]
  %.9108 = phi i32 [ 0, %257 ], [ %286, %283 ]
  %.26 = phi i32 [ 0, %257 ], [ %.29, %283 ]
  %259 = icmp slt i32 %.9108, 16
  br i1 %259, label %260, label %287

260:                                              ; preds = %258
  %261 = srem i32 %.9168, 2
  %262 = icmp ne i32 %261, 0
  br i1 %262, label %263, label %273

263:                                              ; preds = %260
  %264 = srem i32 %.9228, 2
  %265 = icmp eq i32 %264, 0
  br i1 %265, label %266, label %272

266:                                              ; preds = %263
  %267 = sext i32 %.9108 to i64
  %268 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %267
  %269 = load i32, i32* %268, align 4
  %270 = mul nsw i32 1, %269
  %271 = add nsw i32 %.26, %270
  br label %272

272:                                              ; preds = %266, %263
  %.27 = phi i32 [ %271, %266 ], [ %.26, %263 ]
  br label %283

273:                                              ; preds = %260
  %274 = srem i32 %.9228, 2
  %275 = icmp ne i32 %274, 0
  br i1 %275, label %276, label %282

276:                                              ; preds = %273
  %277 = sext i32 %.9108 to i64
  %278 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %277
  %279 = load i32, i32* %278, align 4
  %280 = mul nsw i32 1, %279
  %281 = add nsw i32 %.26, %280
  br label %282

282:                                              ; preds = %276, %273
  %.28 = phi i32 [ %281, %276 ], [ %.26, %273 ]
  br label %283

283:                                              ; preds = %282, %272
  %.29 = phi i32 [ %.27, %272 ], [ %.28, %282 ]
  %284 = sdiv i32 %.9168, 2
  %285 = sdiv i32 %.9228, 2
  %286 = add nsw i32 %.9108, 1
  br label %258

287:                                              ; preds = %258
  br label %288

288:                                              ; preds = %302, %287
  %.10229 = phi i32 [ %.0300, %287 ], [ %304, %302 ]
  %.10169 = phi i32 [ %.0299, %287 ], [ %303, %302 ]
  %.10109 = phi i32 [ 0, %287 ], [ %305, %302 ]
  %.30 = phi i32 [ 0, %287 ], [ %.31, %302 ]
  %289 = icmp slt i32 %.10109, 16
  br i1 %289, label %290, label %306

290:                                              ; preds = %288
  %291 = srem i32 %.10169, 2
  %292 = icmp ne i32 %291, 0
  br i1 %292, label %293, label %302

293:                                              ; preds = %290
  %294 = srem i32 %.10229, 2
  %295 = icmp ne i32 %294, 0
  br i1 %295, label %296, label %302

296:                                              ; preds = %293
  %297 = sext i32 %.10109 to i64
  %298 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %297
  %299 = load i32, i32* %298, align 4
  %300 = mul nsw i32 1, %299
  %301 = add nsw i32 %.30, %300
  br label %302

302:                                              ; preds = %296, %293, %290
  %.31 = phi i32 [ %301, %296 ], [ %.30, %293 ], [ %.30, %290 ]
  %303 = sdiv i32 %.10169, 2
  %304 = sdiv i32 %.10229, 2
  %305 = add nsw i32 %.10109, 1
  br label %288

306:                                              ; preds = %288
  %307 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %308 = mul nsw i32 %.30, %307
  br label %309

309:                                              ; preds = %323, %306
  %.11230 = phi i32 [ 65535, %306 ], [ %325, %323 ]
  %.11170 = phi i32 [ %308, %306 ], [ %324, %323 ]
  %.11110 = phi i32 [ 0, %306 ], [ %326, %323 ]
  %.32 = phi i32 [ 0, %306 ], [ %.33, %323 ]
  %310 = icmp slt i32 %.11110, 16
  br i1 %310, label %311, label %327

311:                                              ; preds = %309
  %312 = srem i32 %.11170, 2
  %313 = icmp ne i32 %312, 0
  br i1 %313, label %314, label %323

314:                                              ; preds = %311
  %315 = srem i32 %.11230, 2
  %316 = icmp ne i32 %315, 0
  br i1 %316, label %317, label %323

317:                                              ; preds = %314
  %318 = sext i32 %.11110 to i64
  %319 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %318
  %320 = load i32, i32* %319, align 4
  %321 = mul nsw i32 1, %320
  %322 = add nsw i32 %.32, %321
  br label %323

323:                                              ; preds = %317, %314, %311
  %.33 = phi i32 [ %322, %317 ], [ %.32, %314 ], [ %.32, %311 ]
  %324 = sdiv i32 %.11170, 2
  %325 = sdiv i32 %.11230, 2
  %326 = add nsw i32 %.11110, 1
  br label %309

327:                                              ; preds = %309
  br label %255

328:                                              ; preds = %255
  br label %329

329:                                              ; preds = %328, %252
  %.1298 = phi i32 [ %.0299, %328 ], [ %.0297, %252 ]
  br label %330

330:                                              ; preds = %402, %329
  %.0303 = phi i32 [ %.0295, %329 ], [ %.40, %402 ]
  %.0302 = phi i32 [ %.0295, %329 ], [ %.34, %402 ]
  %331 = icmp ne i32 %.0303, 0
  br i1 %331, label %332, label %403

332:                                              ; preds = %330
  br label %333

333:                                              ; preds = %358, %332
  %.12231 = phi i32 [ %.0303, %332 ], [ %360, %358 ]
  %.12171 = phi i32 [ %.0302, %332 ], [ %359, %358 ]
  %.12111 = phi i32 [ 0, %332 ], [ %361, %358 ]
  %.34 = phi i32 [ 0, %332 ], [ %.37, %358 ]
  %334 = icmp slt i32 %.12111, 16
  br i1 %334, label %335, label %362

335:                                              ; preds = %333
  %336 = srem i32 %.12171, 2
  %337 = icmp ne i32 %336, 0
  br i1 %337, label %338, label %348

338:                                              ; preds = %335
  %339 = srem i32 %.12231, 2
  %340 = icmp eq i32 %339, 0
  br i1 %340, label %341, label %347

341:                                              ; preds = %338
  %342 = sext i32 %.12111 to i64
  %343 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %342
  %344 = load i32, i32* %343, align 4
  %345 = mul nsw i32 1, %344
  %346 = add nsw i32 %.34, %345
  br label %347

347:                                              ; preds = %341, %338
  %.35 = phi i32 [ %346, %341 ], [ %.34, %338 ]
  br label %358

348:                                              ; preds = %335
  %349 = srem i32 %.12231, 2
  %350 = icmp ne i32 %349, 0
  br i1 %350, label %351, label %357

351:                                              ; preds = %348
  %352 = sext i32 %.12111 to i64
  %353 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %352
  %354 = load i32, i32* %353, align 4
  %355 = mul nsw i32 1, %354
  %356 = add nsw i32 %.34, %355
  br label %357

357:                                              ; preds = %351, %348
  %.36 = phi i32 [ %356, %351 ], [ %.34, %348 ]
  br label %358

358:                                              ; preds = %357, %347
  %.37 = phi i32 [ %.35, %347 ], [ %.36, %357 ]
  %359 = sdiv i32 %.12171, 2
  %360 = sdiv i32 %.12231, 2
  %361 = add nsw i32 %.12111, 1
  br label %333

362:                                              ; preds = %333
  br label %363

363:                                              ; preds = %377, %362
  %.13232 = phi i32 [ %.0303, %362 ], [ %379, %377 ]
  %.13172 = phi i32 [ %.0302, %362 ], [ %378, %377 ]
  %.13112 = phi i32 [ 0, %362 ], [ %380, %377 ]
  %.38 = phi i32 [ 0, %362 ], [ %.39, %377 ]
  %364 = icmp slt i32 %.13112, 16
  br i1 %364, label %365, label %381

365:                                              ; preds = %363
  %366 = srem i32 %.13172, 2
  %367 = icmp ne i32 %366, 0
  br i1 %367, label %368, label %377

368:                                              ; preds = %365
  %369 = srem i32 %.13232, 2
  %370 = icmp ne i32 %369, 0
  br i1 %370, label %371, label %377

371:                                              ; preds = %368
  %372 = sext i32 %.13112 to i64
  %373 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %372
  %374 = load i32, i32* %373, align 4
  %375 = mul nsw i32 1, %374
  %376 = add nsw i32 %.38, %375
  br label %377

377:                                              ; preds = %371, %368, %365
  %.39 = phi i32 [ %376, %371 ], [ %.38, %368 ], [ %.38, %365 ]
  %378 = sdiv i32 %.13172, 2
  %379 = sdiv i32 %.13232, 2
  %380 = add nsw i32 %.13112, 1
  br label %363

381:                                              ; preds = %363
  %382 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %383 = mul nsw i32 %.38, %382
  br label %384

384:                                              ; preds = %398, %381
  %.14233 = phi i32 [ 65535, %381 ], [ %400, %398 ]
  %.14173 = phi i32 [ %383, %381 ], [ %399, %398 ]
  %.14113 = phi i32 [ 0, %381 ], [ %401, %398 ]
  %.40 = phi i32 [ 0, %381 ], [ %.41, %398 ]
  %385 = icmp slt i32 %.14113, 16
  br i1 %385, label %386, label %402

386:                                              ; preds = %384
  %387 = srem i32 %.14173, 2
  %388 = icmp ne i32 %387, 0
  br i1 %388, label %389, label %398

389:                                              ; preds = %386
  %390 = srem i32 %.14233, 2
  %391 = icmp ne i32 %390, 0
  br i1 %391, label %392, label %398

392:                                              ; preds = %389
  %393 = sext i32 %.14113 to i64
  %394 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %393
  %395 = load i32, i32* %394, align 4
  %396 = mul nsw i32 1, %395
  %397 = add nsw i32 %.40, %396
  br label %398

398:                                              ; preds = %392, %389, %386
  %.41 = phi i32 [ %397, %392 ], [ %.40, %389 ], [ %.40, %386 ]
  %399 = sdiv i32 %.14173, 2
  %400 = sdiv i32 %.14233, 2
  %401 = add nsw i32 %.14113, 1
  br label %384

402:                                              ; preds = %384
  br label %330

403:                                              ; preds = %330
  %404 = icmp sge i32 1, 15
  br i1 %404, label %405, label %410

405:                                              ; preds = %403
  %406 = icmp slt i32 %.0296, 0
  br i1 %406, label %407, label %408

407:                                              ; preds = %405
  br label %409

408:                                              ; preds = %405
  br label %409

409:                                              ; preds = %408, %407
  %.42 = phi i32 [ 65535, %407 ], [ 0, %408 ]
  br label %434

410:                                              ; preds = %403
  %411 = icmp sgt i32 1, 0
  br i1 %411, label %412, label %432

412:                                              ; preds = %410
  %413 = icmp sgt i32 %.0296, 32767
  br i1 %413, label %414, label %426

414:                                              ; preds = %412
  %415 = sext i32 1 to i64
  %416 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %415
  %417 = load i32, i32* %416, align 4
  %418 = sdiv i32 %.0296, %417
  %419 = add nsw i32 %418, 65536
  %420 = sub nsw i32 15, 1
  %421 = add nsw i32 %420, 1
  %422 = sext i32 %421 to i64
  %423 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %422
  %424 = load i32, i32* %423, align 4
  %425 = sub nsw i32 %419, %424
  br label %431

426:                                              ; preds = %412
  %427 = sext i32 1 to i64
  %428 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %427
  %429 = load i32, i32* %428, align 4
  %430 = sdiv i32 %.0296, %429
  br label %431

431:                                              ; preds = %426, %414
  %.43 = phi i32 [ %425, %414 ], [ %430, %426 ]
  br label %433

432:                                              ; preds = %410
  br label %433

433:                                              ; preds = %432, %431
  %.44 = phi i32 [ %.43, %431 ], [ %.0296, %432 ]
  br label %434

434:                                              ; preds = %433, %409
  %.45 = phi i32 [ %.42, %409 ], [ %.44, %433 ]
  br label %231

435:                                              ; preds = %231
  %436 = icmp sge i32 1, 15
  br i1 %436, label %437, label %442

437:                                              ; preds = %435
  %438 = icmp slt i32 %.0282, 0
  br i1 %438, label %439, label %440

439:                                              ; preds = %437
  br label %441

440:                                              ; preds = %437
  br label %441

441:                                              ; preds = %440, %439
  %.46 = phi i32 [ 65535, %439 ], [ 0, %440 ]
  br label %466

442:                                              ; preds = %435
  %443 = icmp sgt i32 1, 0
  br i1 %443, label %444, label %464

444:                                              ; preds = %442
  %445 = icmp sgt i32 %.0282, 32767
  br i1 %445, label %446, label %458

446:                                              ; preds = %444
  %447 = sext i32 1 to i64
  %448 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %447
  %449 = load i32, i32* %448, align 4
  %450 = sdiv i32 %.0282, %449
  %451 = add nsw i32 %450, 65536
  %452 = sub nsw i32 15, 1
  %453 = add nsw i32 %452, 1
  %454 = sext i32 %453 to i64
  %455 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %454
  %456 = load i32, i32* %455, align 4
  %457 = sub nsw i32 %451, %456
  br label %463

458:                                              ; preds = %444
  %459 = sext i32 1 to i64
  %460 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %459
  %461 = load i32, i32* %460, align 4
  %462 = sdiv i32 %.0282, %461
  br label %463

463:                                              ; preds = %458, %446
  %.47 = phi i32 [ %457, %446 ], [ %462, %458 ]
  br label %465

464:                                              ; preds = %442
  br label %465

465:                                              ; preds = %464, %463
  %.48 = phi i32 [ %.47, %463 ], [ %.0282, %464 ]
  br label %466

466:                                              ; preds = %465, %441
  %.49 = phi i32 [ %.46, %441 ], [ %.48, %465 ]
  br label %1

467:                                              ; preds = %1
  %468 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.0283)
  %469 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %470

470:                                              ; preds = %935, %467
  %.0307 = phi i32 [ 1, %467 ], [ %.1308, %935 ]
  %.0306 = phi i32 [ 1, %467 ], [ %.99, %935 ]
  %.0305 = phi i32 [ 2, %467 ], [ %.0322, %935 ]
  %471 = icmp sgt i32 %.0306, 0
  br i1 %471, label %472, label %936

472:                                              ; preds = %470
  br label %473

473:                                              ; preds = %487, %472
  %.15234 = phi i32 [ 1, %472 ], [ %489, %487 ]
  %.15174 = phi i32 [ %.0306, %472 ], [ %488, %487 ]
  %.15114 = phi i32 [ 0, %472 ], [ %490, %487 ]
  %.50 = phi i32 [ 0, %472 ], [ %.51, %487 ]
  %474 = icmp slt i32 %.15114, 16
  br i1 %474, label %475, label %491

475:                                              ; preds = %473
  %476 = srem i32 %.15174, 2
  %477 = icmp ne i32 %476, 0
  br i1 %477, label %478, label %487

478:                                              ; preds = %475
  %479 = srem i32 %.15234, 2
  %480 = icmp ne i32 %479, 0
  br i1 %480, label %481, label %487

481:                                              ; preds = %478
  %482 = sext i32 %.15114 to i64
  %483 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %482
  %484 = load i32, i32* %483, align 4
  %485 = mul nsw i32 1, %484
  %486 = add nsw i32 %.50, %485
  br label %487

487:                                              ; preds = %481, %478, %475
  %.51 = phi i32 [ %486, %481 ], [ %.50, %478 ], [ %.50, %475 ]
  %488 = sdiv i32 %.15174, 2
  %489 = sdiv i32 %.15234, 2
  %490 = add nsw i32 %.15114, 1
  br label %473

491:                                              ; preds = %473
  %492 = icmp ne i32 %.50, 0
  br i1 %492, label %493, label %699

493:                                              ; preds = %491
  br label %494

494:                                              ; preds = %697, %493
  %.0311 = phi i32 [ 0, %493 ], [ %.1312, %697 ]
  %.0310 = phi i32 [ %.0305, %493 ], [ %.73, %697 ]
  %.0309 = phi i32 [ %.0307, %493 ], [ %.0317, %697 ]
  %495 = icmp ne i32 %.0310, 0
  br i1 %495, label %496, label %698

496:                                              ; preds = %494
  br label %497

497:                                              ; preds = %511, %496
  %.16235 = phi i32 [ 1, %496 ], [ %513, %511 ]
  %.16175 = phi i32 [ %.0310, %496 ], [ %512, %511 ]
  %.16115 = phi i32 [ 0, %496 ], [ %514, %511 ]
  %.52 = phi i32 [ 0, %496 ], [ %.53, %511 ]
  %498 = icmp slt i32 %.16115, 16
  br i1 %498, label %499, label %515

499:                                              ; preds = %497
  %500 = srem i32 %.16175, 2
  %501 = icmp ne i32 %500, 0
  br i1 %501, label %502, label %511

502:                                              ; preds = %499
  %503 = srem i32 %.16235, 2
  %504 = icmp ne i32 %503, 0
  br i1 %504, label %505, label %511

505:                                              ; preds = %502
  %506 = sext i32 %.16115 to i64
  %507 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %506
  %508 = load i32, i32* %507, align 4
  %509 = mul nsw i32 1, %508
  %510 = add nsw i32 %.52, %509
  br label %511

511:                                              ; preds = %505, %502, %499
  %.53 = phi i32 [ %510, %505 ], [ %.52, %502 ], [ %.52, %499 ]
  %512 = sdiv i32 %.16175, 2
  %513 = sdiv i32 %.16235, 2
  %514 = add nsw i32 %.16115, 1
  br label %497

515:                                              ; preds = %497
  %516 = icmp ne i32 %.52, 0
  br i1 %516, label %517, label %592

517:                                              ; preds = %515
  br label %518

518:                                              ; preds = %590, %517
  %.0314 = phi i32 [ %.0309, %517 ], [ %.60, %590 ]
  %.0313 = phi i32 [ %.0311, %517 ], [ %.54, %590 ]
  %519 = icmp ne i32 %.0314, 0
  br i1 %519, label %520, label %591

520:                                              ; preds = %518
  br label %521

521:                                              ; preds = %546, %520
  %.17236 = phi i32 [ %.0314, %520 ], [ %548, %546 ]
  %.17176 = phi i32 [ %.0313, %520 ], [ %547, %546 ]
  %.17116 = phi i32 [ 0, %520 ], [ %549, %546 ]
  %.54 = phi i32 [ 0, %520 ], [ %.57, %546 ]
  %522 = icmp slt i32 %.17116, 16
  br i1 %522, label %523, label %550

523:                                              ; preds = %521
  %524 = srem i32 %.17176, 2
  %525 = icmp ne i32 %524, 0
  br i1 %525, label %526, label %536

526:                                              ; preds = %523
  %527 = srem i32 %.17236, 2
  %528 = icmp eq i32 %527, 0
  br i1 %528, label %529, label %535

529:                                              ; preds = %526
  %530 = sext i32 %.17116 to i64
  %531 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %530
  %532 = load i32, i32* %531, align 4
  %533 = mul nsw i32 1, %532
  %534 = add nsw i32 %.54, %533
  br label %535

535:                                              ; preds = %529, %526
  %.55 = phi i32 [ %534, %529 ], [ %.54, %526 ]
  br label %546

536:                                              ; preds = %523
  %537 = srem i32 %.17236, 2
  %538 = icmp ne i32 %537, 0
  br i1 %538, label %539, label %545

539:                                              ; preds = %536
  %540 = sext i32 %.17116 to i64
  %541 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %540
  %542 = load i32, i32* %541, align 4
  %543 = mul nsw i32 1, %542
  %544 = add nsw i32 %.54, %543
  br label %545

545:                                              ; preds = %539, %536
  %.56 = phi i32 [ %544, %539 ], [ %.54, %536 ]
  br label %546

546:                                              ; preds = %545, %535
  %.57 = phi i32 [ %.55, %535 ], [ %.56, %545 ]
  %547 = sdiv i32 %.17176, 2
  %548 = sdiv i32 %.17236, 2
  %549 = add nsw i32 %.17116, 1
  br label %521

550:                                              ; preds = %521
  br label %551

551:                                              ; preds = %565, %550
  %.18237 = phi i32 [ %.0314, %550 ], [ %567, %565 ]
  %.18177 = phi i32 [ %.0313, %550 ], [ %566, %565 ]
  %.18117 = phi i32 [ 0, %550 ], [ %568, %565 ]
  %.58 = phi i32 [ 0, %550 ], [ %.59, %565 ]
  %552 = icmp slt i32 %.18117, 16
  br i1 %552, label %553, label %569

553:                                              ; preds = %551
  %554 = srem i32 %.18177, 2
  %555 = icmp ne i32 %554, 0
  br i1 %555, label %556, label %565

556:                                              ; preds = %553
  %557 = srem i32 %.18237, 2
  %558 = icmp ne i32 %557, 0
  br i1 %558, label %559, label %565

559:                                              ; preds = %556
  %560 = sext i32 %.18117 to i64
  %561 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %560
  %562 = load i32, i32* %561, align 4
  %563 = mul nsw i32 1, %562
  %564 = add nsw i32 %.58, %563
  br label %565

565:                                              ; preds = %559, %556, %553
  %.59 = phi i32 [ %564, %559 ], [ %.58, %556 ], [ %.58, %553 ]
  %566 = sdiv i32 %.18177, 2
  %567 = sdiv i32 %.18237, 2
  %568 = add nsw i32 %.18117, 1
  br label %551

569:                                              ; preds = %551
  %570 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %571 = mul nsw i32 %.58, %570
  br label %572

572:                                              ; preds = %586, %569
  %.19238 = phi i32 [ 65535, %569 ], [ %588, %586 ]
  %.19178 = phi i32 [ %571, %569 ], [ %587, %586 ]
  %.19118 = phi i32 [ 0, %569 ], [ %589, %586 ]
  %.60 = phi i32 [ 0, %569 ], [ %.61, %586 ]
  %573 = icmp slt i32 %.19118, 16
  br i1 %573, label %574, label %590

574:                                              ; preds = %572
  %575 = srem i32 %.19178, 2
  %576 = icmp ne i32 %575, 0
  br i1 %576, label %577, label %586

577:                                              ; preds = %574
  %578 = srem i32 %.19238, 2
  %579 = icmp ne i32 %578, 0
  br i1 %579, label %580, label %586

580:                                              ; preds = %577
  %581 = sext i32 %.19118 to i64
  %582 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %581
  %583 = load i32, i32* %582, align 4
  %584 = mul nsw i32 1, %583
  %585 = add nsw i32 %.60, %584
  br label %586

586:                                              ; preds = %580, %577, %574
  %.61 = phi i32 [ %585, %580 ], [ %.60, %577 ], [ %.60, %574 ]
  %587 = sdiv i32 %.19178, 2
  %588 = sdiv i32 %.19238, 2
  %589 = add nsw i32 %.19118, 1
  br label %572

590:                                              ; preds = %572
  br label %518

591:                                              ; preds = %518
  br label %592

592:                                              ; preds = %591, %515
  %.1312 = phi i32 [ %.0313, %591 ], [ %.0311, %515 ]
  br label %593

593:                                              ; preds = %665, %592
  %.0318 = phi i32 [ %.0309, %592 ], [ %.68, %665 ]
  %.0317 = phi i32 [ %.0309, %592 ], [ %.62, %665 ]
  %594 = icmp ne i32 %.0318, 0
  br i1 %594, label %595, label %666

595:                                              ; preds = %593
  br label %596

596:                                              ; preds = %621, %595
  %.20239 = phi i32 [ %.0318, %595 ], [ %623, %621 ]
  %.20179 = phi i32 [ %.0317, %595 ], [ %622, %621 ]
  %.20119 = phi i32 [ 0, %595 ], [ %624, %621 ]
  %.62 = phi i32 [ 0, %595 ], [ %.65, %621 ]
  %597 = icmp slt i32 %.20119, 16
  br i1 %597, label %598, label %625

598:                                              ; preds = %596
  %599 = srem i32 %.20179, 2
  %600 = icmp ne i32 %599, 0
  br i1 %600, label %601, label %611

601:                                              ; preds = %598
  %602 = srem i32 %.20239, 2
  %603 = icmp eq i32 %602, 0
  br i1 %603, label %604, label %610

604:                                              ; preds = %601
  %605 = sext i32 %.20119 to i64
  %606 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %605
  %607 = load i32, i32* %606, align 4
  %608 = mul nsw i32 1, %607
  %609 = add nsw i32 %.62, %608
  br label %610

610:                                              ; preds = %604, %601
  %.63 = phi i32 [ %609, %604 ], [ %.62, %601 ]
  br label %621

611:                                              ; preds = %598
  %612 = srem i32 %.20239, 2
  %613 = icmp ne i32 %612, 0
  br i1 %613, label %614, label %620

614:                                              ; preds = %611
  %615 = sext i32 %.20119 to i64
  %616 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %615
  %617 = load i32, i32* %616, align 4
  %618 = mul nsw i32 1, %617
  %619 = add nsw i32 %.62, %618
  br label %620

620:                                              ; preds = %614, %611
  %.64 = phi i32 [ %619, %614 ], [ %.62, %611 ]
  br label %621

621:                                              ; preds = %620, %610
  %.65 = phi i32 [ %.63, %610 ], [ %.64, %620 ]
  %622 = sdiv i32 %.20179, 2
  %623 = sdiv i32 %.20239, 2
  %624 = add nsw i32 %.20119, 1
  br label %596

625:                                              ; preds = %596
  br label %626

626:                                              ; preds = %640, %625
  %.21240 = phi i32 [ %.0318, %625 ], [ %642, %640 ]
  %.21180 = phi i32 [ %.0317, %625 ], [ %641, %640 ]
  %.21120 = phi i32 [ 0, %625 ], [ %643, %640 ]
  %.66 = phi i32 [ 0, %625 ], [ %.67, %640 ]
  %627 = icmp slt i32 %.21120, 16
  br i1 %627, label %628, label %644

628:                                              ; preds = %626
  %629 = srem i32 %.21180, 2
  %630 = icmp ne i32 %629, 0
  br i1 %630, label %631, label %640

631:                                              ; preds = %628
  %632 = srem i32 %.21240, 2
  %633 = icmp ne i32 %632, 0
  br i1 %633, label %634, label %640

634:                                              ; preds = %631
  %635 = sext i32 %.21120 to i64
  %636 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %635
  %637 = load i32, i32* %636, align 4
  %638 = mul nsw i32 1, %637
  %639 = add nsw i32 %.66, %638
  br label %640

640:                                              ; preds = %634, %631, %628
  %.67 = phi i32 [ %639, %634 ], [ %.66, %631 ], [ %.66, %628 ]
  %641 = sdiv i32 %.21180, 2
  %642 = sdiv i32 %.21240, 2
  %643 = add nsw i32 %.21120, 1
  br label %626

644:                                              ; preds = %626
  %645 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %646 = mul nsw i32 %.66, %645
  br label %647

647:                                              ; preds = %661, %644
  %.22241 = phi i32 [ 65535, %644 ], [ %663, %661 ]
  %.22181 = phi i32 [ %646, %644 ], [ %662, %661 ]
  %.22121 = phi i32 [ 0, %644 ], [ %664, %661 ]
  %.68 = phi i32 [ 0, %644 ], [ %.69, %661 ]
  %648 = icmp slt i32 %.22121, 16
  br i1 %648, label %649, label %665

649:                                              ; preds = %647
  %650 = srem i32 %.22181, 2
  %651 = icmp ne i32 %650, 0
  br i1 %651, label %652, label %661

652:                                              ; preds = %649
  %653 = srem i32 %.22241, 2
  %654 = icmp ne i32 %653, 0
  br i1 %654, label %655, label %661

655:                                              ; preds = %652
  %656 = sext i32 %.22121 to i64
  %657 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %656
  %658 = load i32, i32* %657, align 4
  %659 = mul nsw i32 1, %658
  %660 = add nsw i32 %.68, %659
  br label %661

661:                                              ; preds = %655, %652, %649
  %.69 = phi i32 [ %660, %655 ], [ %.68, %652 ], [ %.68, %649 ]
  %662 = sdiv i32 %.22181, 2
  %663 = sdiv i32 %.22241, 2
  %664 = add nsw i32 %.22121, 1
  br label %647

665:                                              ; preds = %647
  br label %593

666:                                              ; preds = %593
  %667 = icmp sge i32 1, 15
  br i1 %667, label %668, label %673

668:                                              ; preds = %666
  %669 = icmp slt i32 %.0310, 0
  br i1 %669, label %670, label %671

670:                                              ; preds = %668
  br label %672

671:                                              ; preds = %668
  br label %672

672:                                              ; preds = %671, %670
  %.70 = phi i32 [ 65535, %670 ], [ 0, %671 ]
  br label %697

673:                                              ; preds = %666
  %674 = icmp sgt i32 1, 0
  br i1 %674, label %675, label %695

675:                                              ; preds = %673
  %676 = icmp sgt i32 %.0310, 32767
  br i1 %676, label %677, label %689

677:                                              ; preds = %675
  %678 = sext i32 1 to i64
  %679 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %678
  %680 = load i32, i32* %679, align 4
  %681 = sdiv i32 %.0310, %680
  %682 = add nsw i32 %681, 65536
  %683 = sub nsw i32 15, 1
  %684 = add nsw i32 %683, 1
  %685 = sext i32 %684 to i64
  %686 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %685
  %687 = load i32, i32* %686, align 4
  %688 = sub nsw i32 %682, %687
  br label %694

689:                                              ; preds = %675
  %690 = sext i32 1 to i64
  %691 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %690
  %692 = load i32, i32* %691, align 4
  %693 = sdiv i32 %.0310, %692
  br label %694

694:                                              ; preds = %689, %677
  %.71 = phi i32 [ %688, %677 ], [ %693, %689 ]
  br label %696

695:                                              ; preds = %673
  br label %696

696:                                              ; preds = %695, %694
  %.72 = phi i32 [ %.71, %694 ], [ %.0310, %695 ]
  br label %697

697:                                              ; preds = %696, %672
  %.73 = phi i32 [ %.70, %672 ], [ %.72, %696 ]
  br label %494

698:                                              ; preds = %494
  br label %699

699:                                              ; preds = %698, %491
  %.1308 = phi i32 [ %.0311, %698 ], [ %.0307, %491 ]
  br label %700

700:                                              ; preds = %903, %699
  %.0322 = phi i32 [ 0, %699 ], [ %.1323, %903 ]
  %.0321 = phi i32 [ %.0305, %699 ], [ %.95, %903 ]
  %.0320 = phi i32 [ %.0305, %699 ], [ %.0327, %903 ]
  %701 = icmp ne i32 %.0321, 0
  br i1 %701, label %702, label %904

702:                                              ; preds = %700
  br label %703

703:                                              ; preds = %717, %702
  %.23242 = phi i32 [ 1, %702 ], [ %719, %717 ]
  %.23182 = phi i32 [ %.0321, %702 ], [ %718, %717 ]
  %.23122 = phi i32 [ 0, %702 ], [ %720, %717 ]
  %.74 = phi i32 [ 0, %702 ], [ %.75, %717 ]
  %704 = icmp slt i32 %.23122, 16
  br i1 %704, label %705, label %721

705:                                              ; preds = %703
  %706 = srem i32 %.23182, 2
  %707 = icmp ne i32 %706, 0
  br i1 %707, label %708, label %717

708:                                              ; preds = %705
  %709 = srem i32 %.23242, 2
  %710 = icmp ne i32 %709, 0
  br i1 %710, label %711, label %717

711:                                              ; preds = %708
  %712 = sext i32 %.23122 to i64
  %713 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %712
  %714 = load i32, i32* %713, align 4
  %715 = mul nsw i32 1, %714
  %716 = add nsw i32 %.74, %715
  br label %717

717:                                              ; preds = %711, %708, %705
  %.75 = phi i32 [ %716, %711 ], [ %.74, %708 ], [ %.74, %705 ]
  %718 = sdiv i32 %.23182, 2
  %719 = sdiv i32 %.23242, 2
  %720 = add nsw i32 %.23122, 1
  br label %703

721:                                              ; preds = %703
  %722 = icmp ne i32 %.74, 0
  br i1 %722, label %723, label %798

723:                                              ; preds = %721
  br label %724

724:                                              ; preds = %796, %723
  %.0325 = phi i32 [ %.0320, %723 ], [ %.82, %796 ]
  %.0324 = phi i32 [ %.0322, %723 ], [ %.76, %796 ]
  %725 = icmp ne i32 %.0325, 0
  br i1 %725, label %726, label %797

726:                                              ; preds = %724
  br label %727

727:                                              ; preds = %752, %726
  %.24243 = phi i32 [ %.0325, %726 ], [ %754, %752 ]
  %.24183 = phi i32 [ %.0324, %726 ], [ %753, %752 ]
  %.24123 = phi i32 [ 0, %726 ], [ %755, %752 ]
  %.76 = phi i32 [ 0, %726 ], [ %.79, %752 ]
  %728 = icmp slt i32 %.24123, 16
  br i1 %728, label %729, label %756

729:                                              ; preds = %727
  %730 = srem i32 %.24183, 2
  %731 = icmp ne i32 %730, 0
  br i1 %731, label %732, label %742

732:                                              ; preds = %729
  %733 = srem i32 %.24243, 2
  %734 = icmp eq i32 %733, 0
  br i1 %734, label %735, label %741

735:                                              ; preds = %732
  %736 = sext i32 %.24123 to i64
  %737 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %736
  %738 = load i32, i32* %737, align 4
  %739 = mul nsw i32 1, %738
  %740 = add nsw i32 %.76, %739
  br label %741

741:                                              ; preds = %735, %732
  %.77 = phi i32 [ %740, %735 ], [ %.76, %732 ]
  br label %752

742:                                              ; preds = %729
  %743 = srem i32 %.24243, 2
  %744 = icmp ne i32 %743, 0
  br i1 %744, label %745, label %751

745:                                              ; preds = %742
  %746 = sext i32 %.24123 to i64
  %747 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %746
  %748 = load i32, i32* %747, align 4
  %749 = mul nsw i32 1, %748
  %750 = add nsw i32 %.76, %749
  br label %751

751:                                              ; preds = %745, %742
  %.78 = phi i32 [ %750, %745 ], [ %.76, %742 ]
  br label %752

752:                                              ; preds = %751, %741
  %.79 = phi i32 [ %.77, %741 ], [ %.78, %751 ]
  %753 = sdiv i32 %.24183, 2
  %754 = sdiv i32 %.24243, 2
  %755 = add nsw i32 %.24123, 1
  br label %727

756:                                              ; preds = %727
  br label %757

757:                                              ; preds = %771, %756
  %.25244 = phi i32 [ %.0325, %756 ], [ %773, %771 ]
  %.25184 = phi i32 [ %.0324, %756 ], [ %772, %771 ]
  %.25124 = phi i32 [ 0, %756 ], [ %774, %771 ]
  %.80 = phi i32 [ 0, %756 ], [ %.81, %771 ]
  %758 = icmp slt i32 %.25124, 16
  br i1 %758, label %759, label %775

759:                                              ; preds = %757
  %760 = srem i32 %.25184, 2
  %761 = icmp ne i32 %760, 0
  br i1 %761, label %762, label %771

762:                                              ; preds = %759
  %763 = srem i32 %.25244, 2
  %764 = icmp ne i32 %763, 0
  br i1 %764, label %765, label %771

765:                                              ; preds = %762
  %766 = sext i32 %.25124 to i64
  %767 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %766
  %768 = load i32, i32* %767, align 4
  %769 = mul nsw i32 1, %768
  %770 = add nsw i32 %.80, %769
  br label %771

771:                                              ; preds = %765, %762, %759
  %.81 = phi i32 [ %770, %765 ], [ %.80, %762 ], [ %.80, %759 ]
  %772 = sdiv i32 %.25184, 2
  %773 = sdiv i32 %.25244, 2
  %774 = add nsw i32 %.25124, 1
  br label %757

775:                                              ; preds = %757
  %776 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %777 = mul nsw i32 %.80, %776
  br label %778

778:                                              ; preds = %792, %775
  %.26245 = phi i32 [ 65535, %775 ], [ %794, %792 ]
  %.26185 = phi i32 [ %777, %775 ], [ %793, %792 ]
  %.26125 = phi i32 [ 0, %775 ], [ %795, %792 ]
  %.82 = phi i32 [ 0, %775 ], [ %.83, %792 ]
  %779 = icmp slt i32 %.26125, 16
  br i1 %779, label %780, label %796

780:                                              ; preds = %778
  %781 = srem i32 %.26185, 2
  %782 = icmp ne i32 %781, 0
  br i1 %782, label %783, label %792

783:                                              ; preds = %780
  %784 = srem i32 %.26245, 2
  %785 = icmp ne i32 %784, 0
  br i1 %785, label %786, label %792

786:                                              ; preds = %783
  %787 = sext i32 %.26125 to i64
  %788 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %787
  %789 = load i32, i32* %788, align 4
  %790 = mul nsw i32 1, %789
  %791 = add nsw i32 %.82, %790
  br label %792

792:                                              ; preds = %786, %783, %780
  %.83 = phi i32 [ %791, %786 ], [ %.82, %783 ], [ %.82, %780 ]
  %793 = sdiv i32 %.26185, 2
  %794 = sdiv i32 %.26245, 2
  %795 = add nsw i32 %.26125, 1
  br label %778

796:                                              ; preds = %778
  br label %724

797:                                              ; preds = %724
  br label %798

798:                                              ; preds = %797, %721
  %.1323 = phi i32 [ %.0324, %797 ], [ %.0322, %721 ]
  br label %799

799:                                              ; preds = %871, %798
  %.0328 = phi i32 [ %.0320, %798 ], [ %.90, %871 ]
  %.0327 = phi i32 [ %.0320, %798 ], [ %.84, %871 ]
  %800 = icmp ne i32 %.0328, 0
  br i1 %800, label %801, label %872

801:                                              ; preds = %799
  br label %802

802:                                              ; preds = %827, %801
  %.27246 = phi i32 [ %.0328, %801 ], [ %829, %827 ]
  %.27186 = phi i32 [ %.0327, %801 ], [ %828, %827 ]
  %.27126 = phi i32 [ 0, %801 ], [ %830, %827 ]
  %.84 = phi i32 [ 0, %801 ], [ %.87, %827 ]
  %803 = icmp slt i32 %.27126, 16
  br i1 %803, label %804, label %831

804:                                              ; preds = %802
  %805 = srem i32 %.27186, 2
  %806 = icmp ne i32 %805, 0
  br i1 %806, label %807, label %817

807:                                              ; preds = %804
  %808 = srem i32 %.27246, 2
  %809 = icmp eq i32 %808, 0
  br i1 %809, label %810, label %816

810:                                              ; preds = %807
  %811 = sext i32 %.27126 to i64
  %812 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %811
  %813 = load i32, i32* %812, align 4
  %814 = mul nsw i32 1, %813
  %815 = add nsw i32 %.84, %814
  br label %816

816:                                              ; preds = %810, %807
  %.85 = phi i32 [ %815, %810 ], [ %.84, %807 ]
  br label %827

817:                                              ; preds = %804
  %818 = srem i32 %.27246, 2
  %819 = icmp ne i32 %818, 0
  br i1 %819, label %820, label %826

820:                                              ; preds = %817
  %821 = sext i32 %.27126 to i64
  %822 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %821
  %823 = load i32, i32* %822, align 4
  %824 = mul nsw i32 1, %823
  %825 = add nsw i32 %.84, %824
  br label %826

826:                                              ; preds = %820, %817
  %.86 = phi i32 [ %825, %820 ], [ %.84, %817 ]
  br label %827

827:                                              ; preds = %826, %816
  %.87 = phi i32 [ %.85, %816 ], [ %.86, %826 ]
  %828 = sdiv i32 %.27186, 2
  %829 = sdiv i32 %.27246, 2
  %830 = add nsw i32 %.27126, 1
  br label %802

831:                                              ; preds = %802
  br label %832

832:                                              ; preds = %846, %831
  %.28247 = phi i32 [ %.0328, %831 ], [ %848, %846 ]
  %.28187 = phi i32 [ %.0327, %831 ], [ %847, %846 ]
  %.28127 = phi i32 [ 0, %831 ], [ %849, %846 ]
  %.88 = phi i32 [ 0, %831 ], [ %.89, %846 ]
  %833 = icmp slt i32 %.28127, 16
  br i1 %833, label %834, label %850

834:                                              ; preds = %832
  %835 = srem i32 %.28187, 2
  %836 = icmp ne i32 %835, 0
  br i1 %836, label %837, label %846

837:                                              ; preds = %834
  %838 = srem i32 %.28247, 2
  %839 = icmp ne i32 %838, 0
  br i1 %839, label %840, label %846

840:                                              ; preds = %837
  %841 = sext i32 %.28127 to i64
  %842 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %841
  %843 = load i32, i32* %842, align 4
  %844 = mul nsw i32 1, %843
  %845 = add nsw i32 %.88, %844
  br label %846

846:                                              ; preds = %840, %837, %834
  %.89 = phi i32 [ %845, %840 ], [ %.88, %837 ], [ %.88, %834 ]
  %847 = sdiv i32 %.28187, 2
  %848 = sdiv i32 %.28247, 2
  %849 = add nsw i32 %.28127, 1
  br label %832

850:                                              ; preds = %832
  %851 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %852 = mul nsw i32 %.88, %851
  br label %853

853:                                              ; preds = %867, %850
  %.29248 = phi i32 [ 65535, %850 ], [ %869, %867 ]
  %.29188 = phi i32 [ %852, %850 ], [ %868, %867 ]
  %.29128 = phi i32 [ 0, %850 ], [ %870, %867 ]
  %.90 = phi i32 [ 0, %850 ], [ %.91, %867 ]
  %854 = icmp slt i32 %.29128, 16
  br i1 %854, label %855, label %871

855:                                              ; preds = %853
  %856 = srem i32 %.29188, 2
  %857 = icmp ne i32 %856, 0
  br i1 %857, label %858, label %867

858:                                              ; preds = %855
  %859 = srem i32 %.29248, 2
  %860 = icmp ne i32 %859, 0
  br i1 %860, label %861, label %867

861:                                              ; preds = %858
  %862 = sext i32 %.29128 to i64
  %863 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %862
  %864 = load i32, i32* %863, align 4
  %865 = mul nsw i32 1, %864
  %866 = add nsw i32 %.90, %865
  br label %867

867:                                              ; preds = %861, %858, %855
  %.91 = phi i32 [ %866, %861 ], [ %.90, %858 ], [ %.90, %855 ]
  %868 = sdiv i32 %.29188, 2
  %869 = sdiv i32 %.29248, 2
  %870 = add nsw i32 %.29128, 1
  br label %853

871:                                              ; preds = %853
  br label %799

872:                                              ; preds = %799
  %873 = icmp sge i32 1, 15
  br i1 %873, label %874, label %879

874:                                              ; preds = %872
  %875 = icmp slt i32 %.0321, 0
  br i1 %875, label %876, label %877

876:                                              ; preds = %874
  br label %878

877:                                              ; preds = %874
  br label %878

878:                                              ; preds = %877, %876
  %.92 = phi i32 [ 65535, %876 ], [ 0, %877 ]
  br label %903

879:                                              ; preds = %872
  %880 = icmp sgt i32 1, 0
  br i1 %880, label %881, label %901

881:                                              ; preds = %879
  %882 = icmp sgt i32 %.0321, 32767
  br i1 %882, label %883, label %895

883:                                              ; preds = %881
  %884 = sext i32 1 to i64
  %885 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %884
  %886 = load i32, i32* %885, align 4
  %887 = sdiv i32 %.0321, %886
  %888 = add nsw i32 %887, 65536
  %889 = sub nsw i32 15, 1
  %890 = add nsw i32 %889, 1
  %891 = sext i32 %890 to i64
  %892 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %891
  %893 = load i32, i32* %892, align 4
  %894 = sub nsw i32 %888, %893
  br label %900

895:                                              ; preds = %881
  %896 = sext i32 1 to i64
  %897 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %896
  %898 = load i32, i32* %897, align 4
  %899 = sdiv i32 %.0321, %898
  br label %900

900:                                              ; preds = %895, %883
  %.93 = phi i32 [ %894, %883 ], [ %899, %895 ]
  br label %902

901:                                              ; preds = %879
  br label %902

902:                                              ; preds = %901, %900
  %.94 = phi i32 [ %.93, %900 ], [ %.0321, %901 ]
  br label %903

903:                                              ; preds = %902, %878
  %.95 = phi i32 [ %.92, %878 ], [ %.94, %902 ]
  br label %700

904:                                              ; preds = %700
  %905 = icmp sge i32 1, 15
  br i1 %905, label %906, label %911

906:                                              ; preds = %904
  %907 = icmp slt i32 %.0306, 0
  br i1 %907, label %908, label %909

908:                                              ; preds = %906
  br label %910

909:                                              ; preds = %906
  br label %910

910:                                              ; preds = %909, %908
  %.96 = phi i32 [ 65535, %908 ], [ 0, %909 ]
  br label %935

911:                                              ; preds = %904
  %912 = icmp sgt i32 1, 0
  br i1 %912, label %913, label %933

913:                                              ; preds = %911
  %914 = icmp sgt i32 %.0306, 32767
  br i1 %914, label %915, label %927

915:                                              ; preds = %913
  %916 = sext i32 1 to i64
  %917 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %916
  %918 = load i32, i32* %917, align 4
  %919 = sdiv i32 %.0306, %918
  %920 = add nsw i32 %919, 65536
  %921 = sub nsw i32 15, 1
  %922 = add nsw i32 %921, 1
  %923 = sext i32 %922 to i64
  %924 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %923
  %925 = load i32, i32* %924, align 4
  %926 = sub nsw i32 %920, %925
  br label %932

927:                                              ; preds = %913
  %928 = sext i32 1 to i64
  %929 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %928
  %930 = load i32, i32* %929, align 4
  %931 = sdiv i32 %.0306, %930
  br label %932

932:                                              ; preds = %927, %915
  %.97 = phi i32 [ %926, %915 ], [ %931, %927 ]
  br label %934

933:                                              ; preds = %911
  br label %934

934:                                              ; preds = %933, %932
  %.98 = phi i32 [ %.97, %932 ], [ %.0306, %933 ]
  br label %935

935:                                              ; preds = %934, %910
  %.99 = phi i32 [ %.96, %910 ], [ %.98, %934 ]
  br label %470

936:                                              ; preds = %470
  %937 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.0307)
  %938 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  br label %939

939:                                              ; preds = %1408, %936
  %.0279 = phi i32 [ 2, %936 ], [ %1411, %1408 ]
  %940 = icmp slt i32 %.0279, 16
  br i1 %940, label %941, label %1412

941:                                              ; preds = %939
  br label %942

942:                                              ; preds = %1407, %941
  %.0332 = phi i32 [ 1, %941 ], [ %.1333, %1407 ]
  %.0331 = phi i32 [ %.0279, %941 ], [ %.149, %1407 ]
  %.0330 = phi i32 [ 2, %941 ], [ %.0346, %1407 ]
  %943 = icmp sgt i32 %.0331, 0
  br i1 %943, label %944, label %1408

944:                                              ; preds = %942
  br label %945

945:                                              ; preds = %959, %944
  %.30249 = phi i32 [ 1, %944 ], [ %961, %959 ]
  %.30189 = phi i32 [ %.0331, %944 ], [ %960, %959 ]
  %.30129 = phi i32 [ 0, %944 ], [ %962, %959 ]
  %.100 = phi i32 [ 0, %944 ], [ %.101, %959 ]
  %946 = icmp slt i32 %.30129, 16
  br i1 %946, label %947, label %963

947:                                              ; preds = %945
  %948 = srem i32 %.30189, 2
  %949 = icmp ne i32 %948, 0
  br i1 %949, label %950, label %959

950:                                              ; preds = %947
  %951 = srem i32 %.30249, 2
  %952 = icmp ne i32 %951, 0
  br i1 %952, label %953, label %959

953:                                              ; preds = %950
  %954 = sext i32 %.30129 to i64
  %955 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %954
  %956 = load i32, i32* %955, align 4
  %957 = mul nsw i32 1, %956
  %958 = add nsw i32 %.100, %957
  br label %959

959:                                              ; preds = %953, %950, %947
  %.101 = phi i32 [ %958, %953 ], [ %.100, %950 ], [ %.100, %947 ]
  %960 = sdiv i32 %.30189, 2
  %961 = sdiv i32 %.30249, 2
  %962 = add nsw i32 %.30129, 1
  br label %945

963:                                              ; preds = %945
  %964 = icmp ne i32 %.100, 0
  br i1 %964, label %965, label %1171

965:                                              ; preds = %963
  br label %966

966:                                              ; preds = %1169, %965
  %.0336 = phi i32 [ 0, %965 ], [ %.1337, %1169 ]
  %.0335 = phi i32 [ %.0330, %965 ], [ %.123, %1169 ]
  %.0334 = phi i32 [ %.0332, %965 ], [ %.0341, %1169 ]
  %967 = icmp ne i32 %.0335, 0
  br i1 %967, label %968, label %1170

968:                                              ; preds = %966
  br label %969

969:                                              ; preds = %983, %968
  %.31250 = phi i32 [ 1, %968 ], [ %985, %983 ]
  %.31190 = phi i32 [ %.0335, %968 ], [ %984, %983 ]
  %.31130 = phi i32 [ 0, %968 ], [ %986, %983 ]
  %.102 = phi i32 [ 0, %968 ], [ %.103, %983 ]
  %970 = icmp slt i32 %.31130, 16
  br i1 %970, label %971, label %987

971:                                              ; preds = %969
  %972 = srem i32 %.31190, 2
  %973 = icmp ne i32 %972, 0
  br i1 %973, label %974, label %983

974:                                              ; preds = %971
  %975 = srem i32 %.31250, 2
  %976 = icmp ne i32 %975, 0
  br i1 %976, label %977, label %983

977:                                              ; preds = %974
  %978 = sext i32 %.31130 to i64
  %979 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %978
  %980 = load i32, i32* %979, align 4
  %981 = mul nsw i32 1, %980
  %982 = add nsw i32 %.102, %981
  br label %983

983:                                              ; preds = %977, %974, %971
  %.103 = phi i32 [ %982, %977 ], [ %.102, %974 ], [ %.102, %971 ]
  %984 = sdiv i32 %.31190, 2
  %985 = sdiv i32 %.31250, 2
  %986 = add nsw i32 %.31130, 1
  br label %969

987:                                              ; preds = %969
  %988 = icmp ne i32 %.102, 0
  br i1 %988, label %989, label %1064

989:                                              ; preds = %987
  br label %990

990:                                              ; preds = %1062, %989
  %.0339 = phi i32 [ %.0334, %989 ], [ %.110, %1062 ]
  %.0338 = phi i32 [ %.0336, %989 ], [ %.104, %1062 ]
  %991 = icmp ne i32 %.0339, 0
  br i1 %991, label %992, label %1063

992:                                              ; preds = %990
  br label %993

993:                                              ; preds = %1018, %992
  %.32251 = phi i32 [ %.0339, %992 ], [ %1020, %1018 ]
  %.32191 = phi i32 [ %.0338, %992 ], [ %1019, %1018 ]
  %.32131 = phi i32 [ 0, %992 ], [ %1021, %1018 ]
  %.104 = phi i32 [ 0, %992 ], [ %.107, %1018 ]
  %994 = icmp slt i32 %.32131, 16
  br i1 %994, label %995, label %1022

995:                                              ; preds = %993
  %996 = srem i32 %.32191, 2
  %997 = icmp ne i32 %996, 0
  br i1 %997, label %998, label %1008

998:                                              ; preds = %995
  %999 = srem i32 %.32251, 2
  %1000 = icmp eq i32 %999, 0
  br i1 %1000, label %1001, label %1007

1001:                                             ; preds = %998
  %1002 = sext i32 %.32131 to i64
  %1003 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1002
  %1004 = load i32, i32* %1003, align 4
  %1005 = mul nsw i32 1, %1004
  %1006 = add nsw i32 %.104, %1005
  br label %1007

1007:                                             ; preds = %1001, %998
  %.105 = phi i32 [ %1006, %1001 ], [ %.104, %998 ]
  br label %1018

1008:                                             ; preds = %995
  %1009 = srem i32 %.32251, 2
  %1010 = icmp ne i32 %1009, 0
  br i1 %1010, label %1011, label %1017

1011:                                             ; preds = %1008
  %1012 = sext i32 %.32131 to i64
  %1013 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1012
  %1014 = load i32, i32* %1013, align 4
  %1015 = mul nsw i32 1, %1014
  %1016 = add nsw i32 %.104, %1015
  br label %1017

1017:                                             ; preds = %1011, %1008
  %.106 = phi i32 [ %1016, %1011 ], [ %.104, %1008 ]
  br label %1018

1018:                                             ; preds = %1017, %1007
  %.107 = phi i32 [ %.105, %1007 ], [ %.106, %1017 ]
  %1019 = sdiv i32 %.32191, 2
  %1020 = sdiv i32 %.32251, 2
  %1021 = add nsw i32 %.32131, 1
  br label %993

1022:                                             ; preds = %993
  br label %1023

1023:                                             ; preds = %1037, %1022
  %.33252 = phi i32 [ %.0339, %1022 ], [ %1039, %1037 ]
  %.33192 = phi i32 [ %.0338, %1022 ], [ %1038, %1037 ]
  %.33132 = phi i32 [ 0, %1022 ], [ %1040, %1037 ]
  %.108 = phi i32 [ 0, %1022 ], [ %.109, %1037 ]
  %1024 = icmp slt i32 %.33132, 16
  br i1 %1024, label %1025, label %1041

1025:                                             ; preds = %1023
  %1026 = srem i32 %.33192, 2
  %1027 = icmp ne i32 %1026, 0
  br i1 %1027, label %1028, label %1037

1028:                                             ; preds = %1025
  %1029 = srem i32 %.33252, 2
  %1030 = icmp ne i32 %1029, 0
  br i1 %1030, label %1031, label %1037

1031:                                             ; preds = %1028
  %1032 = sext i32 %.33132 to i64
  %1033 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1032
  %1034 = load i32, i32* %1033, align 4
  %1035 = mul nsw i32 1, %1034
  %1036 = add nsw i32 %.108, %1035
  br label %1037

1037:                                             ; preds = %1031, %1028, %1025
  %.109 = phi i32 [ %1036, %1031 ], [ %.108, %1028 ], [ %.108, %1025 ]
  %1038 = sdiv i32 %.33192, 2
  %1039 = sdiv i32 %.33252, 2
  %1040 = add nsw i32 %.33132, 1
  br label %1023

1041:                                             ; preds = %1023
  %1042 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1043 = mul nsw i32 %.108, %1042
  br label %1044

1044:                                             ; preds = %1058, %1041
  %.34253 = phi i32 [ 65535, %1041 ], [ %1060, %1058 ]
  %.34193 = phi i32 [ %1043, %1041 ], [ %1059, %1058 ]
  %.34133 = phi i32 [ 0, %1041 ], [ %1061, %1058 ]
  %.110 = phi i32 [ 0, %1041 ], [ %.111, %1058 ]
  %1045 = icmp slt i32 %.34133, 16
  br i1 %1045, label %1046, label %1062

1046:                                             ; preds = %1044
  %1047 = srem i32 %.34193, 2
  %1048 = icmp ne i32 %1047, 0
  br i1 %1048, label %1049, label %1058

1049:                                             ; preds = %1046
  %1050 = srem i32 %.34253, 2
  %1051 = icmp ne i32 %1050, 0
  br i1 %1051, label %1052, label %1058

1052:                                             ; preds = %1049
  %1053 = sext i32 %.34133 to i64
  %1054 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1053
  %1055 = load i32, i32* %1054, align 4
  %1056 = mul nsw i32 1, %1055
  %1057 = add nsw i32 %.110, %1056
  br label %1058

1058:                                             ; preds = %1052, %1049, %1046
  %.111 = phi i32 [ %1057, %1052 ], [ %.110, %1049 ], [ %.110, %1046 ]
  %1059 = sdiv i32 %.34193, 2
  %1060 = sdiv i32 %.34253, 2
  %1061 = add nsw i32 %.34133, 1
  br label %1044

1062:                                             ; preds = %1044
  br label %990

1063:                                             ; preds = %990
  br label %1064

1064:                                             ; preds = %1063, %987
  %.1337 = phi i32 [ %.0338, %1063 ], [ %.0336, %987 ]
  br label %1065

1065:                                             ; preds = %1137, %1064
  %.0342 = phi i32 [ %.0334, %1064 ], [ %.118, %1137 ]
  %.0341 = phi i32 [ %.0334, %1064 ], [ %.112, %1137 ]
  %1066 = icmp ne i32 %.0342, 0
  br i1 %1066, label %1067, label %1138

1067:                                             ; preds = %1065
  br label %1068

1068:                                             ; preds = %1093, %1067
  %.35254 = phi i32 [ %.0342, %1067 ], [ %1095, %1093 ]
  %.35194 = phi i32 [ %.0341, %1067 ], [ %1094, %1093 ]
  %.35134 = phi i32 [ 0, %1067 ], [ %1096, %1093 ]
  %.112 = phi i32 [ 0, %1067 ], [ %.115, %1093 ]
  %1069 = icmp slt i32 %.35134, 16
  br i1 %1069, label %1070, label %1097

1070:                                             ; preds = %1068
  %1071 = srem i32 %.35194, 2
  %1072 = icmp ne i32 %1071, 0
  br i1 %1072, label %1073, label %1083

1073:                                             ; preds = %1070
  %1074 = srem i32 %.35254, 2
  %1075 = icmp eq i32 %1074, 0
  br i1 %1075, label %1076, label %1082

1076:                                             ; preds = %1073
  %1077 = sext i32 %.35134 to i64
  %1078 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1077
  %1079 = load i32, i32* %1078, align 4
  %1080 = mul nsw i32 1, %1079
  %1081 = add nsw i32 %.112, %1080
  br label %1082

1082:                                             ; preds = %1076, %1073
  %.113 = phi i32 [ %1081, %1076 ], [ %.112, %1073 ]
  br label %1093

1083:                                             ; preds = %1070
  %1084 = srem i32 %.35254, 2
  %1085 = icmp ne i32 %1084, 0
  br i1 %1085, label %1086, label %1092

1086:                                             ; preds = %1083
  %1087 = sext i32 %.35134 to i64
  %1088 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1087
  %1089 = load i32, i32* %1088, align 4
  %1090 = mul nsw i32 1, %1089
  %1091 = add nsw i32 %.112, %1090
  br label %1092

1092:                                             ; preds = %1086, %1083
  %.114 = phi i32 [ %1091, %1086 ], [ %.112, %1083 ]
  br label %1093

1093:                                             ; preds = %1092, %1082
  %.115 = phi i32 [ %.113, %1082 ], [ %.114, %1092 ]
  %1094 = sdiv i32 %.35194, 2
  %1095 = sdiv i32 %.35254, 2
  %1096 = add nsw i32 %.35134, 1
  br label %1068

1097:                                             ; preds = %1068
  br label %1098

1098:                                             ; preds = %1112, %1097
  %.36255 = phi i32 [ %.0342, %1097 ], [ %1114, %1112 ]
  %.36195 = phi i32 [ %.0341, %1097 ], [ %1113, %1112 ]
  %.36135 = phi i32 [ 0, %1097 ], [ %1115, %1112 ]
  %.116 = phi i32 [ 0, %1097 ], [ %.117, %1112 ]
  %1099 = icmp slt i32 %.36135, 16
  br i1 %1099, label %1100, label %1116

1100:                                             ; preds = %1098
  %1101 = srem i32 %.36195, 2
  %1102 = icmp ne i32 %1101, 0
  br i1 %1102, label %1103, label %1112

1103:                                             ; preds = %1100
  %1104 = srem i32 %.36255, 2
  %1105 = icmp ne i32 %1104, 0
  br i1 %1105, label %1106, label %1112

1106:                                             ; preds = %1103
  %1107 = sext i32 %.36135 to i64
  %1108 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1107
  %1109 = load i32, i32* %1108, align 4
  %1110 = mul nsw i32 1, %1109
  %1111 = add nsw i32 %.116, %1110
  br label %1112

1112:                                             ; preds = %1106, %1103, %1100
  %.117 = phi i32 [ %1111, %1106 ], [ %.116, %1103 ], [ %.116, %1100 ]
  %1113 = sdiv i32 %.36195, 2
  %1114 = sdiv i32 %.36255, 2
  %1115 = add nsw i32 %.36135, 1
  br label %1098

1116:                                             ; preds = %1098
  %1117 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1118 = mul nsw i32 %.116, %1117
  br label %1119

1119:                                             ; preds = %1133, %1116
  %.37256 = phi i32 [ 65535, %1116 ], [ %1135, %1133 ]
  %.37196 = phi i32 [ %1118, %1116 ], [ %1134, %1133 ]
  %.37136 = phi i32 [ 0, %1116 ], [ %1136, %1133 ]
  %.118 = phi i32 [ 0, %1116 ], [ %.119, %1133 ]
  %1120 = icmp slt i32 %.37136, 16
  br i1 %1120, label %1121, label %1137

1121:                                             ; preds = %1119
  %1122 = srem i32 %.37196, 2
  %1123 = icmp ne i32 %1122, 0
  br i1 %1123, label %1124, label %1133

1124:                                             ; preds = %1121
  %1125 = srem i32 %.37256, 2
  %1126 = icmp ne i32 %1125, 0
  br i1 %1126, label %1127, label %1133

1127:                                             ; preds = %1124
  %1128 = sext i32 %.37136 to i64
  %1129 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1128
  %1130 = load i32, i32* %1129, align 4
  %1131 = mul nsw i32 1, %1130
  %1132 = add nsw i32 %.118, %1131
  br label %1133

1133:                                             ; preds = %1127, %1124, %1121
  %.119 = phi i32 [ %1132, %1127 ], [ %.118, %1124 ], [ %.118, %1121 ]
  %1134 = sdiv i32 %.37196, 2
  %1135 = sdiv i32 %.37256, 2
  %1136 = add nsw i32 %.37136, 1
  br label %1119

1137:                                             ; preds = %1119
  br label %1065

1138:                                             ; preds = %1065
  %1139 = icmp sge i32 1, 15
  br i1 %1139, label %1140, label %1145

1140:                                             ; preds = %1138
  %1141 = icmp slt i32 %.0335, 0
  br i1 %1141, label %1142, label %1143

1142:                                             ; preds = %1140
  br label %1144

1143:                                             ; preds = %1140
  br label %1144

1144:                                             ; preds = %1143, %1142
  %.120 = phi i32 [ 65535, %1142 ], [ 0, %1143 ]
  br label %1169

1145:                                             ; preds = %1138
  %1146 = icmp sgt i32 1, 0
  br i1 %1146, label %1147, label %1167

1147:                                             ; preds = %1145
  %1148 = icmp sgt i32 %.0335, 32767
  br i1 %1148, label %1149, label %1161

1149:                                             ; preds = %1147
  %1150 = sext i32 1 to i64
  %1151 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1150
  %1152 = load i32, i32* %1151, align 4
  %1153 = sdiv i32 %.0335, %1152
  %1154 = add nsw i32 %1153, 65536
  %1155 = sub nsw i32 15, 1
  %1156 = add nsw i32 %1155, 1
  %1157 = sext i32 %1156 to i64
  %1158 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1157
  %1159 = load i32, i32* %1158, align 4
  %1160 = sub nsw i32 %1154, %1159
  br label %1166

1161:                                             ; preds = %1147
  %1162 = sext i32 1 to i64
  %1163 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1162
  %1164 = load i32, i32* %1163, align 4
  %1165 = sdiv i32 %.0335, %1164
  br label %1166

1166:                                             ; preds = %1161, %1149
  %.121 = phi i32 [ %1160, %1149 ], [ %1165, %1161 ]
  br label %1168

1167:                                             ; preds = %1145
  br label %1168

1168:                                             ; preds = %1167, %1166
  %.122 = phi i32 [ %.121, %1166 ], [ %.0335, %1167 ]
  br label %1169

1169:                                             ; preds = %1168, %1144
  %.123 = phi i32 [ %.120, %1144 ], [ %.122, %1168 ]
  br label %966

1170:                                             ; preds = %966
  br label %1171

1171:                                             ; preds = %1170, %963
  %.1333 = phi i32 [ %.0336, %1170 ], [ %.0332, %963 ]
  br label %1172

1172:                                             ; preds = %1375, %1171
  %.0346 = phi i32 [ 0, %1171 ], [ %.1347, %1375 ]
  %.0345 = phi i32 [ %.0330, %1171 ], [ %.145, %1375 ]
  %.0344 = phi i32 [ %.0330, %1171 ], [ %.0351, %1375 ]
  %1173 = icmp ne i32 %.0345, 0
  br i1 %1173, label %1174, label %1376

1174:                                             ; preds = %1172
  br label %1175

1175:                                             ; preds = %1189, %1174
  %.38257 = phi i32 [ 1, %1174 ], [ %1191, %1189 ]
  %.38197 = phi i32 [ %.0345, %1174 ], [ %1190, %1189 ]
  %.38137 = phi i32 [ 0, %1174 ], [ %1192, %1189 ]
  %.124 = phi i32 [ 0, %1174 ], [ %.125, %1189 ]
  %1176 = icmp slt i32 %.38137, 16
  br i1 %1176, label %1177, label %1193

1177:                                             ; preds = %1175
  %1178 = srem i32 %.38197, 2
  %1179 = icmp ne i32 %1178, 0
  br i1 %1179, label %1180, label %1189

1180:                                             ; preds = %1177
  %1181 = srem i32 %.38257, 2
  %1182 = icmp ne i32 %1181, 0
  br i1 %1182, label %1183, label %1189

1183:                                             ; preds = %1180
  %1184 = sext i32 %.38137 to i64
  %1185 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1184
  %1186 = load i32, i32* %1185, align 4
  %1187 = mul nsw i32 1, %1186
  %1188 = add nsw i32 %.124, %1187
  br label %1189

1189:                                             ; preds = %1183, %1180, %1177
  %.125 = phi i32 [ %1188, %1183 ], [ %.124, %1180 ], [ %.124, %1177 ]
  %1190 = sdiv i32 %.38197, 2
  %1191 = sdiv i32 %.38257, 2
  %1192 = add nsw i32 %.38137, 1
  br label %1175

1193:                                             ; preds = %1175
  %1194 = icmp ne i32 %.124, 0
  br i1 %1194, label %1195, label %1270

1195:                                             ; preds = %1193
  br label %1196

1196:                                             ; preds = %1268, %1195
  %.0349 = phi i32 [ %.0344, %1195 ], [ %.132, %1268 ]
  %.0348 = phi i32 [ %.0346, %1195 ], [ %.126, %1268 ]
  %1197 = icmp ne i32 %.0349, 0
  br i1 %1197, label %1198, label %1269

1198:                                             ; preds = %1196
  br label %1199

1199:                                             ; preds = %1224, %1198
  %.39258 = phi i32 [ %.0349, %1198 ], [ %1226, %1224 ]
  %.39198 = phi i32 [ %.0348, %1198 ], [ %1225, %1224 ]
  %.39138 = phi i32 [ 0, %1198 ], [ %1227, %1224 ]
  %.126 = phi i32 [ 0, %1198 ], [ %.129, %1224 ]
  %1200 = icmp slt i32 %.39138, 16
  br i1 %1200, label %1201, label %1228

1201:                                             ; preds = %1199
  %1202 = srem i32 %.39198, 2
  %1203 = icmp ne i32 %1202, 0
  br i1 %1203, label %1204, label %1214

1204:                                             ; preds = %1201
  %1205 = srem i32 %.39258, 2
  %1206 = icmp eq i32 %1205, 0
  br i1 %1206, label %1207, label %1213

1207:                                             ; preds = %1204
  %1208 = sext i32 %.39138 to i64
  %1209 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1208
  %1210 = load i32, i32* %1209, align 4
  %1211 = mul nsw i32 1, %1210
  %1212 = add nsw i32 %.126, %1211
  br label %1213

1213:                                             ; preds = %1207, %1204
  %.127 = phi i32 [ %1212, %1207 ], [ %.126, %1204 ]
  br label %1224

1214:                                             ; preds = %1201
  %1215 = srem i32 %.39258, 2
  %1216 = icmp ne i32 %1215, 0
  br i1 %1216, label %1217, label %1223

1217:                                             ; preds = %1214
  %1218 = sext i32 %.39138 to i64
  %1219 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1218
  %1220 = load i32, i32* %1219, align 4
  %1221 = mul nsw i32 1, %1220
  %1222 = add nsw i32 %.126, %1221
  br label %1223

1223:                                             ; preds = %1217, %1214
  %.128 = phi i32 [ %1222, %1217 ], [ %.126, %1214 ]
  br label %1224

1224:                                             ; preds = %1223, %1213
  %.129 = phi i32 [ %.127, %1213 ], [ %.128, %1223 ]
  %1225 = sdiv i32 %.39198, 2
  %1226 = sdiv i32 %.39258, 2
  %1227 = add nsw i32 %.39138, 1
  br label %1199

1228:                                             ; preds = %1199
  br label %1229

1229:                                             ; preds = %1243, %1228
  %.40259 = phi i32 [ %.0349, %1228 ], [ %1245, %1243 ]
  %.40199 = phi i32 [ %.0348, %1228 ], [ %1244, %1243 ]
  %.40139 = phi i32 [ 0, %1228 ], [ %1246, %1243 ]
  %.130 = phi i32 [ 0, %1228 ], [ %.131, %1243 ]
  %1230 = icmp slt i32 %.40139, 16
  br i1 %1230, label %1231, label %1247

1231:                                             ; preds = %1229
  %1232 = srem i32 %.40199, 2
  %1233 = icmp ne i32 %1232, 0
  br i1 %1233, label %1234, label %1243

1234:                                             ; preds = %1231
  %1235 = srem i32 %.40259, 2
  %1236 = icmp ne i32 %1235, 0
  br i1 %1236, label %1237, label %1243

1237:                                             ; preds = %1234
  %1238 = sext i32 %.40139 to i64
  %1239 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1238
  %1240 = load i32, i32* %1239, align 4
  %1241 = mul nsw i32 1, %1240
  %1242 = add nsw i32 %.130, %1241
  br label %1243

1243:                                             ; preds = %1237, %1234, %1231
  %.131 = phi i32 [ %1242, %1237 ], [ %.130, %1234 ], [ %.130, %1231 ]
  %1244 = sdiv i32 %.40199, 2
  %1245 = sdiv i32 %.40259, 2
  %1246 = add nsw i32 %.40139, 1
  br label %1229

1247:                                             ; preds = %1229
  %1248 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1249 = mul nsw i32 %.130, %1248
  br label %1250

1250:                                             ; preds = %1264, %1247
  %.41260 = phi i32 [ 65535, %1247 ], [ %1266, %1264 ]
  %.41200 = phi i32 [ %1249, %1247 ], [ %1265, %1264 ]
  %.41140 = phi i32 [ 0, %1247 ], [ %1267, %1264 ]
  %.132 = phi i32 [ 0, %1247 ], [ %.133, %1264 ]
  %1251 = icmp slt i32 %.41140, 16
  br i1 %1251, label %1252, label %1268

1252:                                             ; preds = %1250
  %1253 = srem i32 %.41200, 2
  %1254 = icmp ne i32 %1253, 0
  br i1 %1254, label %1255, label %1264

1255:                                             ; preds = %1252
  %1256 = srem i32 %.41260, 2
  %1257 = icmp ne i32 %1256, 0
  br i1 %1257, label %1258, label %1264

1258:                                             ; preds = %1255
  %1259 = sext i32 %.41140 to i64
  %1260 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1259
  %1261 = load i32, i32* %1260, align 4
  %1262 = mul nsw i32 1, %1261
  %1263 = add nsw i32 %.132, %1262
  br label %1264

1264:                                             ; preds = %1258, %1255, %1252
  %.133 = phi i32 [ %1263, %1258 ], [ %.132, %1255 ], [ %.132, %1252 ]
  %1265 = sdiv i32 %.41200, 2
  %1266 = sdiv i32 %.41260, 2
  %1267 = add nsw i32 %.41140, 1
  br label %1250

1268:                                             ; preds = %1250
  br label %1196

1269:                                             ; preds = %1196
  br label %1270

1270:                                             ; preds = %1269, %1193
  %.1347 = phi i32 [ %.0348, %1269 ], [ %.0346, %1193 ]
  br label %1271

1271:                                             ; preds = %1343, %1270
  %.0352 = phi i32 [ %.0344, %1270 ], [ %.140, %1343 ]
  %.0351 = phi i32 [ %.0344, %1270 ], [ %.134, %1343 ]
  %1272 = icmp ne i32 %.0352, 0
  br i1 %1272, label %1273, label %1344

1273:                                             ; preds = %1271
  br label %1274

1274:                                             ; preds = %1299, %1273
  %.42261 = phi i32 [ %.0352, %1273 ], [ %1301, %1299 ]
  %.42201 = phi i32 [ %.0351, %1273 ], [ %1300, %1299 ]
  %.42141 = phi i32 [ 0, %1273 ], [ %1302, %1299 ]
  %.134 = phi i32 [ 0, %1273 ], [ %.137, %1299 ]
  %1275 = icmp slt i32 %.42141, 16
  br i1 %1275, label %1276, label %1303

1276:                                             ; preds = %1274
  %1277 = srem i32 %.42201, 2
  %1278 = icmp ne i32 %1277, 0
  br i1 %1278, label %1279, label %1289

1279:                                             ; preds = %1276
  %1280 = srem i32 %.42261, 2
  %1281 = icmp eq i32 %1280, 0
  br i1 %1281, label %1282, label %1288

1282:                                             ; preds = %1279
  %1283 = sext i32 %.42141 to i64
  %1284 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1283
  %1285 = load i32, i32* %1284, align 4
  %1286 = mul nsw i32 1, %1285
  %1287 = add nsw i32 %.134, %1286
  br label %1288

1288:                                             ; preds = %1282, %1279
  %.135 = phi i32 [ %1287, %1282 ], [ %.134, %1279 ]
  br label %1299

1289:                                             ; preds = %1276
  %1290 = srem i32 %.42261, 2
  %1291 = icmp ne i32 %1290, 0
  br i1 %1291, label %1292, label %1298

1292:                                             ; preds = %1289
  %1293 = sext i32 %.42141 to i64
  %1294 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1293
  %1295 = load i32, i32* %1294, align 4
  %1296 = mul nsw i32 1, %1295
  %1297 = add nsw i32 %.134, %1296
  br label %1298

1298:                                             ; preds = %1292, %1289
  %.136 = phi i32 [ %1297, %1292 ], [ %.134, %1289 ]
  br label %1299

1299:                                             ; preds = %1298, %1288
  %.137 = phi i32 [ %.135, %1288 ], [ %.136, %1298 ]
  %1300 = sdiv i32 %.42201, 2
  %1301 = sdiv i32 %.42261, 2
  %1302 = add nsw i32 %.42141, 1
  br label %1274

1303:                                             ; preds = %1274
  br label %1304

1304:                                             ; preds = %1318, %1303
  %.43262 = phi i32 [ %.0352, %1303 ], [ %1320, %1318 ]
  %.43202 = phi i32 [ %.0351, %1303 ], [ %1319, %1318 ]
  %.43142 = phi i32 [ 0, %1303 ], [ %1321, %1318 ]
  %.138 = phi i32 [ 0, %1303 ], [ %.139, %1318 ]
  %1305 = icmp slt i32 %.43142, 16
  br i1 %1305, label %1306, label %1322

1306:                                             ; preds = %1304
  %1307 = srem i32 %.43202, 2
  %1308 = icmp ne i32 %1307, 0
  br i1 %1308, label %1309, label %1318

1309:                                             ; preds = %1306
  %1310 = srem i32 %.43262, 2
  %1311 = icmp ne i32 %1310, 0
  br i1 %1311, label %1312, label %1318

1312:                                             ; preds = %1309
  %1313 = sext i32 %.43142 to i64
  %1314 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1313
  %1315 = load i32, i32* %1314, align 4
  %1316 = mul nsw i32 1, %1315
  %1317 = add nsw i32 %.138, %1316
  br label %1318

1318:                                             ; preds = %1312, %1309, %1306
  %.139 = phi i32 [ %1317, %1312 ], [ %.138, %1309 ], [ %.138, %1306 ]
  %1319 = sdiv i32 %.43202, 2
  %1320 = sdiv i32 %.43262, 2
  %1321 = add nsw i32 %.43142, 1
  br label %1304

1322:                                             ; preds = %1304
  %1323 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1324 = mul nsw i32 %.138, %1323
  br label %1325

1325:                                             ; preds = %1339, %1322
  %.44263 = phi i32 [ 65535, %1322 ], [ %1341, %1339 ]
  %.44203 = phi i32 [ %1324, %1322 ], [ %1340, %1339 ]
  %.44143 = phi i32 [ 0, %1322 ], [ %1342, %1339 ]
  %.140 = phi i32 [ 0, %1322 ], [ %.141, %1339 ]
  %1326 = icmp slt i32 %.44143, 16
  br i1 %1326, label %1327, label %1343

1327:                                             ; preds = %1325
  %1328 = srem i32 %.44203, 2
  %1329 = icmp ne i32 %1328, 0
  br i1 %1329, label %1330, label %1339

1330:                                             ; preds = %1327
  %1331 = srem i32 %.44263, 2
  %1332 = icmp ne i32 %1331, 0
  br i1 %1332, label %1333, label %1339

1333:                                             ; preds = %1330
  %1334 = sext i32 %.44143 to i64
  %1335 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1334
  %1336 = load i32, i32* %1335, align 4
  %1337 = mul nsw i32 1, %1336
  %1338 = add nsw i32 %.140, %1337
  br label %1339

1339:                                             ; preds = %1333, %1330, %1327
  %.141 = phi i32 [ %1338, %1333 ], [ %.140, %1330 ], [ %.140, %1327 ]
  %1340 = sdiv i32 %.44203, 2
  %1341 = sdiv i32 %.44263, 2
  %1342 = add nsw i32 %.44143, 1
  br label %1325

1343:                                             ; preds = %1325
  br label %1271

1344:                                             ; preds = %1271
  %1345 = icmp sge i32 1, 15
  br i1 %1345, label %1346, label %1351

1346:                                             ; preds = %1344
  %1347 = icmp slt i32 %.0345, 0
  br i1 %1347, label %1348, label %1349

1348:                                             ; preds = %1346
  br label %1350

1349:                                             ; preds = %1346
  br label %1350

1350:                                             ; preds = %1349, %1348
  %.142 = phi i32 [ 65535, %1348 ], [ 0, %1349 ]
  br label %1375

1351:                                             ; preds = %1344
  %1352 = icmp sgt i32 1, 0
  br i1 %1352, label %1353, label %1373

1353:                                             ; preds = %1351
  %1354 = icmp sgt i32 %.0345, 32767
  br i1 %1354, label %1355, label %1367

1355:                                             ; preds = %1353
  %1356 = sext i32 1 to i64
  %1357 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1356
  %1358 = load i32, i32* %1357, align 4
  %1359 = sdiv i32 %.0345, %1358
  %1360 = add nsw i32 %1359, 65536
  %1361 = sub nsw i32 15, 1
  %1362 = add nsw i32 %1361, 1
  %1363 = sext i32 %1362 to i64
  %1364 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1363
  %1365 = load i32, i32* %1364, align 4
  %1366 = sub nsw i32 %1360, %1365
  br label %1372

1367:                                             ; preds = %1353
  %1368 = sext i32 1 to i64
  %1369 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1368
  %1370 = load i32, i32* %1369, align 4
  %1371 = sdiv i32 %.0345, %1370
  br label %1372

1372:                                             ; preds = %1367, %1355
  %.143 = phi i32 [ %1366, %1355 ], [ %1371, %1367 ]
  br label %1374

1373:                                             ; preds = %1351
  br label %1374

1374:                                             ; preds = %1373, %1372
  %.144 = phi i32 [ %.143, %1372 ], [ %.0345, %1373 ]
  br label %1375

1375:                                             ; preds = %1374, %1350
  %.145 = phi i32 [ %.142, %1350 ], [ %.144, %1374 ]
  br label %1172

1376:                                             ; preds = %1172
  %1377 = icmp sge i32 1, 15
  br i1 %1377, label %1378, label %1383

1378:                                             ; preds = %1376
  %1379 = icmp slt i32 %.0331, 0
  br i1 %1379, label %1380, label %1381

1380:                                             ; preds = %1378
  br label %1382

1381:                                             ; preds = %1378
  br label %1382

1382:                                             ; preds = %1381, %1380
  %.146 = phi i32 [ 65535, %1380 ], [ 0, %1381 ]
  br label %1407

1383:                                             ; preds = %1376
  %1384 = icmp sgt i32 1, 0
  br i1 %1384, label %1385, label %1405

1385:                                             ; preds = %1383
  %1386 = icmp sgt i32 %.0331, 32767
  br i1 %1386, label %1387, label %1399

1387:                                             ; preds = %1385
  %1388 = sext i32 1 to i64
  %1389 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1388
  %1390 = load i32, i32* %1389, align 4
  %1391 = sdiv i32 %.0331, %1390
  %1392 = add nsw i32 %1391, 65536
  %1393 = sub nsw i32 15, 1
  %1394 = add nsw i32 %1393, 1
  %1395 = sext i32 %1394 to i64
  %1396 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1395
  %1397 = load i32, i32* %1396, align 4
  %1398 = sub nsw i32 %1392, %1397
  br label %1404

1399:                                             ; preds = %1385
  %1400 = sext i32 1 to i64
  %1401 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1400
  %1402 = load i32, i32* %1401, align 4
  %1403 = sdiv i32 %.0331, %1402
  br label %1404

1404:                                             ; preds = %1399, %1387
  %.147 = phi i32 [ %1398, %1387 ], [ %1403, %1399 ]
  br label %1406

1405:                                             ; preds = %1383
  br label %1406

1406:                                             ; preds = %1405, %1404
  %.148 = phi i32 [ %.147, %1404 ], [ %.0331, %1405 ]
  br label %1407

1407:                                             ; preds = %1406, %1382
  %.149 = phi i32 [ %.146, %1382 ], [ %.148, %1406 ]
  br label %942

1408:                                             ; preds = %942
  %1409 = call i32 (i32, ...) bitcast (i32 (...)* @putint to i32 (i32, ...)*)(i32 %.0332)
  %1410 = call i32 (i32, ...) bitcast (i32 (...)* @putch to i32 (i32, ...)*)(i32 10)
  %1411 = add nsw i32 %.0279, 1
  br label %939

1412:                                             ; preds = %939
  br label %1413

1413:                                             ; preds = %1888, %1412
  %.1280 = phi i32 [ 0, %1412 ], [ %1889, %1888 ]
  %1414 = icmp slt i32 %.1280, 16
  br i1 %1414, label %1415, label %1890

1415:                                             ; preds = %1413
  br label %1416

1416:                                             ; preds = %1881, %1415
  %.0357 = phi i32 [ 1, %1415 ], [ %.1358, %1881 ]
  %.0356 = phi i32 [ %.1280, %1415 ], [ %.199, %1881 ]
  %.0355 = phi i32 [ 2, %1415 ], [ %.0315, %1881 ]
  %1417 = icmp sgt i32 %.0356, 0
  br i1 %1417, label %1418, label %1882

1418:                                             ; preds = %1416
  br label %1419

1419:                                             ; preds = %1433, %1418
  %.45264 = phi i32 [ 1, %1418 ], [ %1435, %1433 ]
  %.45204 = phi i32 [ %.0356, %1418 ], [ %1434, %1433 ]
  %.45144 = phi i32 [ 0, %1418 ], [ %1436, %1433 ]
  %.150 = phi i32 [ 0, %1418 ], [ %.151, %1433 ]
  %1420 = icmp slt i32 %.45144, 16
  br i1 %1420, label %1421, label %1437

1421:                                             ; preds = %1419
  %1422 = srem i32 %.45204, 2
  %1423 = icmp ne i32 %1422, 0
  br i1 %1423, label %1424, label %1433

1424:                                             ; preds = %1421
  %1425 = srem i32 %.45264, 2
  %1426 = icmp ne i32 %1425, 0
  br i1 %1426, label %1427, label %1433

1427:                                             ; preds = %1424
  %1428 = sext i32 %.45144 to i64
  %1429 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1428
  %1430 = load i32, i32* %1429, align 4
  %1431 = mul nsw i32 1, %1430
  %1432 = add nsw i32 %.150, %1431
  br label %1433

1433:                                             ; preds = %1427, %1424, %1421
  %.151 = phi i32 [ %1432, %1427 ], [ %.150, %1424 ], [ %.150, %1421 ]
  %1434 = sdiv i32 %.45204, 2
  %1435 = sdiv i32 %.45264, 2
  %1436 = add nsw i32 %.45144, 1
  br label %1419

1437:                                             ; preds = %1419
  %1438 = icmp ne i32 %.150, 0
  br i1 %1438, label %1439, label %1645

1439:                                             ; preds = %1437
  br label %1440

1440:                                             ; preds = %1643, %1439
  %.0360 = phi i32 [ %.0355, %1439 ], [ %.173, %1643 ]
  %.0359 = phi i32 [ %.0357, %1439 ], [ %.0340, %1643 ]
  %.0353 = phi i32 [ 0, %1439 ], [ %.1354, %1643 ]
  %1441 = icmp ne i32 %.0360, 0
  br i1 %1441, label %1442, label %1644

1442:                                             ; preds = %1440
  br label %1443

1443:                                             ; preds = %1457, %1442
  %.46265 = phi i32 [ 1, %1442 ], [ %1459, %1457 ]
  %.46205 = phi i32 [ %.0360, %1442 ], [ %1458, %1457 ]
  %.46145 = phi i32 [ 0, %1442 ], [ %1460, %1457 ]
  %.152 = phi i32 [ 0, %1442 ], [ %.153, %1457 ]
  %1444 = icmp slt i32 %.46145, 16
  br i1 %1444, label %1445, label %1461

1445:                                             ; preds = %1443
  %1446 = srem i32 %.46205, 2
  %1447 = icmp ne i32 %1446, 0
  br i1 %1447, label %1448, label %1457

1448:                                             ; preds = %1445
  %1449 = srem i32 %.46265, 2
  %1450 = icmp ne i32 %1449, 0
  br i1 %1450, label %1451, label %1457

1451:                                             ; preds = %1448
  %1452 = sext i32 %.46145 to i64
  %1453 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1452
  %1454 = load i32, i32* %1453, align 4
  %1455 = mul nsw i32 1, %1454
  %1456 = add nsw i32 %.152, %1455
  br label %1457

1457:                                             ; preds = %1451, %1448, %1445
  %.153 = phi i32 [ %1456, %1451 ], [ %.152, %1448 ], [ %.152, %1445 ]
  %1458 = sdiv i32 %.46205, 2
  %1459 = sdiv i32 %.46265, 2
  %1460 = add nsw i32 %.46145, 1
  br label %1443

1461:                                             ; preds = %1443
  %1462 = icmp ne i32 %.152, 0
  br i1 %1462, label %1463, label %1538

1463:                                             ; preds = %1461
  br label %1464

1464:                                             ; preds = %1536, %1463
  %.0350 = phi i32 [ %.0353, %1463 ], [ %.154, %1536 ]
  %.0343 = phi i32 [ %.0359, %1463 ], [ %.160, %1536 ]
  %1465 = icmp ne i32 %.0343, 0
  br i1 %1465, label %1466, label %1537

1466:                                             ; preds = %1464
  br label %1467

1467:                                             ; preds = %1492, %1466
  %.47266 = phi i32 [ %.0343, %1466 ], [ %1494, %1492 ]
  %.47206 = phi i32 [ %.0350, %1466 ], [ %1493, %1492 ]
  %.47146 = phi i32 [ 0, %1466 ], [ %1495, %1492 ]
  %.154 = phi i32 [ 0, %1466 ], [ %.157, %1492 ]
  %1468 = icmp slt i32 %.47146, 16
  br i1 %1468, label %1469, label %1496

1469:                                             ; preds = %1467
  %1470 = srem i32 %.47206, 2
  %1471 = icmp ne i32 %1470, 0
  br i1 %1471, label %1472, label %1482

1472:                                             ; preds = %1469
  %1473 = srem i32 %.47266, 2
  %1474 = icmp eq i32 %1473, 0
  br i1 %1474, label %1475, label %1481

1475:                                             ; preds = %1472
  %1476 = sext i32 %.47146 to i64
  %1477 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1476
  %1478 = load i32, i32* %1477, align 4
  %1479 = mul nsw i32 1, %1478
  %1480 = add nsw i32 %.154, %1479
  br label %1481

1481:                                             ; preds = %1475, %1472
  %.155 = phi i32 [ %1480, %1475 ], [ %.154, %1472 ]
  br label %1492

1482:                                             ; preds = %1469
  %1483 = srem i32 %.47266, 2
  %1484 = icmp ne i32 %1483, 0
  br i1 %1484, label %1485, label %1491

1485:                                             ; preds = %1482
  %1486 = sext i32 %.47146 to i64
  %1487 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1486
  %1488 = load i32, i32* %1487, align 4
  %1489 = mul nsw i32 1, %1488
  %1490 = add nsw i32 %.154, %1489
  br label %1491

1491:                                             ; preds = %1485, %1482
  %.156 = phi i32 [ %1490, %1485 ], [ %.154, %1482 ]
  br label %1492

1492:                                             ; preds = %1491, %1481
  %.157 = phi i32 [ %.155, %1481 ], [ %.156, %1491 ]
  %1493 = sdiv i32 %.47206, 2
  %1494 = sdiv i32 %.47266, 2
  %1495 = add nsw i32 %.47146, 1
  br label %1467

1496:                                             ; preds = %1467
  br label %1497

1497:                                             ; preds = %1511, %1496
  %.48267 = phi i32 [ %.0343, %1496 ], [ %1513, %1511 ]
  %.48207 = phi i32 [ %.0350, %1496 ], [ %1512, %1511 ]
  %.48147 = phi i32 [ 0, %1496 ], [ %1514, %1511 ]
  %.158 = phi i32 [ 0, %1496 ], [ %.159, %1511 ]
  %1498 = icmp slt i32 %.48147, 16
  br i1 %1498, label %1499, label %1515

1499:                                             ; preds = %1497
  %1500 = srem i32 %.48207, 2
  %1501 = icmp ne i32 %1500, 0
  br i1 %1501, label %1502, label %1511

1502:                                             ; preds = %1499
  %1503 = srem i32 %.48267, 2
  %1504 = icmp ne i32 %1503, 0
  br i1 %1504, label %1505, label %1511

1505:                                             ; preds = %1502
  %1506 = sext i32 %.48147 to i64
  %1507 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1506
  %1508 = load i32, i32* %1507, align 4
  %1509 = mul nsw i32 1, %1508
  %1510 = add nsw i32 %.158, %1509
  br label %1511

1511:                                             ; preds = %1505, %1502, %1499
  %.159 = phi i32 [ %1510, %1505 ], [ %.158, %1502 ], [ %.158, %1499 ]
  %1512 = sdiv i32 %.48207, 2
  %1513 = sdiv i32 %.48267, 2
  %1514 = add nsw i32 %.48147, 1
  br label %1497

1515:                                             ; preds = %1497
  %1516 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1517 = mul nsw i32 %.158, %1516
  br label %1518

1518:                                             ; preds = %1532, %1515
  %.49268 = phi i32 [ 65535, %1515 ], [ %1534, %1532 ]
  %.49208 = phi i32 [ %1517, %1515 ], [ %1533, %1532 ]
  %.49148 = phi i32 [ 0, %1515 ], [ %1535, %1532 ]
  %.160 = phi i32 [ 0, %1515 ], [ %.161, %1532 ]
  %1519 = icmp slt i32 %.49148, 16
  br i1 %1519, label %1520, label %1536

1520:                                             ; preds = %1518
  %1521 = srem i32 %.49208, 2
  %1522 = icmp ne i32 %1521, 0
  br i1 %1522, label %1523, label %1532

1523:                                             ; preds = %1520
  %1524 = srem i32 %.49268, 2
  %1525 = icmp ne i32 %1524, 0
  br i1 %1525, label %1526, label %1532

1526:                                             ; preds = %1523
  %1527 = sext i32 %.49148 to i64
  %1528 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1527
  %1529 = load i32, i32* %1528, align 4
  %1530 = mul nsw i32 1, %1529
  %1531 = add nsw i32 %.160, %1530
  br label %1532

1532:                                             ; preds = %1526, %1523, %1520
  %.161 = phi i32 [ %1531, %1526 ], [ %.160, %1523 ], [ %.160, %1520 ]
  %1533 = sdiv i32 %.49208, 2
  %1534 = sdiv i32 %.49268, 2
  %1535 = add nsw i32 %.49148, 1
  br label %1518

1536:                                             ; preds = %1518
  br label %1464

1537:                                             ; preds = %1464
  br label %1538

1538:                                             ; preds = %1537, %1461
  %.1354 = phi i32 [ %.0350, %1537 ], [ %.0353, %1461 ]
  br label %1539

1539:                                             ; preds = %1611, %1538
  %.0340 = phi i32 [ %.0359, %1538 ], [ %.162, %1611 ]
  %.0329 = phi i32 [ %.0359, %1538 ], [ %.168, %1611 ]
  %1540 = icmp ne i32 %.0329, 0
  br i1 %1540, label %1541, label %1612

1541:                                             ; preds = %1539
  br label %1542

1542:                                             ; preds = %1567, %1541
  %.50269 = phi i32 [ %.0329, %1541 ], [ %1569, %1567 ]
  %.50209 = phi i32 [ %.0340, %1541 ], [ %1568, %1567 ]
  %.50149 = phi i32 [ 0, %1541 ], [ %1570, %1567 ]
  %.162 = phi i32 [ 0, %1541 ], [ %.165, %1567 ]
  %1543 = icmp slt i32 %.50149, 16
  br i1 %1543, label %1544, label %1571

1544:                                             ; preds = %1542
  %1545 = srem i32 %.50209, 2
  %1546 = icmp ne i32 %1545, 0
  br i1 %1546, label %1547, label %1557

1547:                                             ; preds = %1544
  %1548 = srem i32 %.50269, 2
  %1549 = icmp eq i32 %1548, 0
  br i1 %1549, label %1550, label %1556

1550:                                             ; preds = %1547
  %1551 = sext i32 %.50149 to i64
  %1552 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1551
  %1553 = load i32, i32* %1552, align 4
  %1554 = mul nsw i32 1, %1553
  %1555 = add nsw i32 %.162, %1554
  br label %1556

1556:                                             ; preds = %1550, %1547
  %.163 = phi i32 [ %1555, %1550 ], [ %.162, %1547 ]
  br label %1567

1557:                                             ; preds = %1544
  %1558 = srem i32 %.50269, 2
  %1559 = icmp ne i32 %1558, 0
  br i1 %1559, label %1560, label %1566

1560:                                             ; preds = %1557
  %1561 = sext i32 %.50149 to i64
  %1562 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1561
  %1563 = load i32, i32* %1562, align 4
  %1564 = mul nsw i32 1, %1563
  %1565 = add nsw i32 %.162, %1564
  br label %1566

1566:                                             ; preds = %1560, %1557
  %.164 = phi i32 [ %1565, %1560 ], [ %.162, %1557 ]
  br label %1567

1567:                                             ; preds = %1566, %1556
  %.165 = phi i32 [ %.163, %1556 ], [ %.164, %1566 ]
  %1568 = sdiv i32 %.50209, 2
  %1569 = sdiv i32 %.50269, 2
  %1570 = add nsw i32 %.50149, 1
  br label %1542

1571:                                             ; preds = %1542
  br label %1572

1572:                                             ; preds = %1586, %1571
  %.51270 = phi i32 [ %.0329, %1571 ], [ %1588, %1586 ]
  %.51210 = phi i32 [ %.0340, %1571 ], [ %1587, %1586 ]
  %.51150 = phi i32 [ 0, %1571 ], [ %1589, %1586 ]
  %.166 = phi i32 [ 0, %1571 ], [ %.167, %1586 ]
  %1573 = icmp slt i32 %.51150, 16
  br i1 %1573, label %1574, label %1590

1574:                                             ; preds = %1572
  %1575 = srem i32 %.51210, 2
  %1576 = icmp ne i32 %1575, 0
  br i1 %1576, label %1577, label %1586

1577:                                             ; preds = %1574
  %1578 = srem i32 %.51270, 2
  %1579 = icmp ne i32 %1578, 0
  br i1 %1579, label %1580, label %1586

1580:                                             ; preds = %1577
  %1581 = sext i32 %.51150 to i64
  %1582 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1581
  %1583 = load i32, i32* %1582, align 4
  %1584 = mul nsw i32 1, %1583
  %1585 = add nsw i32 %.166, %1584
  br label %1586

1586:                                             ; preds = %1580, %1577, %1574
  %.167 = phi i32 [ %1585, %1580 ], [ %.166, %1577 ], [ %.166, %1574 ]
  %1587 = sdiv i32 %.51210, 2
  %1588 = sdiv i32 %.51270, 2
  %1589 = add nsw i32 %.51150, 1
  br label %1572

1590:                                             ; preds = %1572
  %1591 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1592 = mul nsw i32 %.166, %1591
  br label %1593

1593:                                             ; preds = %1607, %1590
  %.52271 = phi i32 [ 65535, %1590 ], [ %1609, %1607 ]
  %.52211 = phi i32 [ %1592, %1590 ], [ %1608, %1607 ]
  %.52151 = phi i32 [ 0, %1590 ], [ %1610, %1607 ]
  %.168 = phi i32 [ 0, %1590 ], [ %.169, %1607 ]
  %1594 = icmp slt i32 %.52151, 16
  br i1 %1594, label %1595, label %1611

1595:                                             ; preds = %1593
  %1596 = srem i32 %.52211, 2
  %1597 = icmp ne i32 %1596, 0
  br i1 %1597, label %1598, label %1607

1598:                                             ; preds = %1595
  %1599 = srem i32 %.52271, 2
  %1600 = icmp ne i32 %1599, 0
  br i1 %1600, label %1601, label %1607

1601:                                             ; preds = %1598
  %1602 = sext i32 %.52151 to i64
  %1603 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1602
  %1604 = load i32, i32* %1603, align 4
  %1605 = mul nsw i32 1, %1604
  %1606 = add nsw i32 %.168, %1605
  br label %1607

1607:                                             ; preds = %1601, %1598, %1595
  %.169 = phi i32 [ %1606, %1601 ], [ %.168, %1598 ], [ %.168, %1595 ]
  %1608 = sdiv i32 %.52211, 2
  %1609 = sdiv i32 %.52271, 2
  %1610 = add nsw i32 %.52151, 1
  br label %1593

1611:                                             ; preds = %1593
  br label %1539

1612:                                             ; preds = %1539
  %1613 = icmp sge i32 1, 15
  br i1 %1613, label %1614, label %1619

1614:                                             ; preds = %1612
  %1615 = icmp slt i32 %.0360, 0
  br i1 %1615, label %1616, label %1617

1616:                                             ; preds = %1614
  br label %1618

1617:                                             ; preds = %1614
  br label %1618

1618:                                             ; preds = %1617, %1616
  %.170 = phi i32 [ 65535, %1616 ], [ 0, %1617 ]
  br label %1643

1619:                                             ; preds = %1612
  %1620 = icmp sgt i32 1, 0
  br i1 %1620, label %1621, label %1641

1621:                                             ; preds = %1619
  %1622 = icmp sgt i32 %.0360, 32767
  br i1 %1622, label %1623, label %1635

1623:                                             ; preds = %1621
  %1624 = sext i32 1 to i64
  %1625 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1624
  %1626 = load i32, i32* %1625, align 4
  %1627 = sdiv i32 %.0360, %1626
  %1628 = add nsw i32 %1627, 65536
  %1629 = sub nsw i32 15, 1
  %1630 = add nsw i32 %1629, 1
  %1631 = sext i32 %1630 to i64
  %1632 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1631
  %1633 = load i32, i32* %1632, align 4
  %1634 = sub nsw i32 %1628, %1633
  br label %1640

1635:                                             ; preds = %1621
  %1636 = sext i32 1 to i64
  %1637 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1636
  %1638 = load i32, i32* %1637, align 4
  %1639 = sdiv i32 %.0360, %1638
  br label %1640

1640:                                             ; preds = %1635, %1623
  %.171 = phi i32 [ %1634, %1623 ], [ %1639, %1635 ]
  br label %1642

1641:                                             ; preds = %1619
  br label %1642

1642:                                             ; preds = %1641, %1640
  %.172 = phi i32 [ %.171, %1640 ], [ %.0360, %1641 ]
  br label %1643

1643:                                             ; preds = %1642, %1618
  %.173 = phi i32 [ %.170, %1618 ], [ %.172, %1642 ]
  br label %1440

1644:                                             ; preds = %1440
  br label %1645

1645:                                             ; preds = %1644, %1437
  %.1358 = phi i32 [ %.0353, %1644 ], [ %.0357, %1437 ]
  br label %1646

1646:                                             ; preds = %1849, %1645
  %.0326 = phi i32 [ %.0355, %1645 ], [ %.0294, %1849 ]
  %.0319 = phi i32 [ %.0355, %1645 ], [ %.195, %1849 ]
  %.0315 = phi i32 [ 0, %1645 ], [ %.1316, %1849 ]
  %1647 = icmp ne i32 %.0319, 0
  br i1 %1647, label %1648, label %1850

1648:                                             ; preds = %1646
  br label %1649

1649:                                             ; preds = %1663, %1648
  %.53272 = phi i32 [ 1, %1648 ], [ %1665, %1663 ]
  %.53212 = phi i32 [ %.0319, %1648 ], [ %1664, %1663 ]
  %.53152 = phi i32 [ 0, %1648 ], [ %1666, %1663 ]
  %.174 = phi i32 [ 0, %1648 ], [ %.175, %1663 ]
  %1650 = icmp slt i32 %.53152, 16
  br i1 %1650, label %1651, label %1667

1651:                                             ; preds = %1649
  %1652 = srem i32 %.53212, 2
  %1653 = icmp ne i32 %1652, 0
  br i1 %1653, label %1654, label %1663

1654:                                             ; preds = %1651
  %1655 = srem i32 %.53272, 2
  %1656 = icmp ne i32 %1655, 0
  br i1 %1656, label %1657, label %1663

1657:                                             ; preds = %1654
  %1658 = sext i32 %.53152 to i64
  %1659 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1658
  %1660 = load i32, i32* %1659, align 4
  %1661 = mul nsw i32 1, %1660
  %1662 = add nsw i32 %.174, %1661
  br label %1663

1663:                                             ; preds = %1657, %1654, %1651
  %.175 = phi i32 [ %1662, %1657 ], [ %.174, %1654 ], [ %.174, %1651 ]
  %1664 = sdiv i32 %.53212, 2
  %1665 = sdiv i32 %.53272, 2
  %1666 = add nsw i32 %.53152, 1
  br label %1649

1667:                                             ; preds = %1649
  %1668 = icmp ne i32 %.174, 0
  br i1 %1668, label %1669, label %1744

1669:                                             ; preds = %1667
  br label %1670

1670:                                             ; preds = %1742, %1669
  %.0304 = phi i32 [ %.0315, %1669 ], [ %.176, %1742 ]
  %.0301 = phi i32 [ %.0326, %1669 ], [ %.182, %1742 ]
  %1671 = icmp ne i32 %.0301, 0
  br i1 %1671, label %1672, label %1743

1672:                                             ; preds = %1670
  br label %1673

1673:                                             ; preds = %1698, %1672
  %.54273 = phi i32 [ %.0301, %1672 ], [ %1700, %1698 ]
  %.54213 = phi i32 [ %.0304, %1672 ], [ %1699, %1698 ]
  %.54153 = phi i32 [ 0, %1672 ], [ %1701, %1698 ]
  %.176 = phi i32 [ 0, %1672 ], [ %.179, %1698 ]
  %1674 = icmp slt i32 %.54153, 16
  br i1 %1674, label %1675, label %1702

1675:                                             ; preds = %1673
  %1676 = srem i32 %.54213, 2
  %1677 = icmp ne i32 %1676, 0
  br i1 %1677, label %1678, label %1688

1678:                                             ; preds = %1675
  %1679 = srem i32 %.54273, 2
  %1680 = icmp eq i32 %1679, 0
  br i1 %1680, label %1681, label %1687

1681:                                             ; preds = %1678
  %1682 = sext i32 %.54153 to i64
  %1683 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1682
  %1684 = load i32, i32* %1683, align 4
  %1685 = mul nsw i32 1, %1684
  %1686 = add nsw i32 %.176, %1685
  br label %1687

1687:                                             ; preds = %1681, %1678
  %.177 = phi i32 [ %1686, %1681 ], [ %.176, %1678 ]
  br label %1698

1688:                                             ; preds = %1675
  %1689 = srem i32 %.54273, 2
  %1690 = icmp ne i32 %1689, 0
  br i1 %1690, label %1691, label %1697

1691:                                             ; preds = %1688
  %1692 = sext i32 %.54153 to i64
  %1693 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1692
  %1694 = load i32, i32* %1693, align 4
  %1695 = mul nsw i32 1, %1694
  %1696 = add nsw i32 %.176, %1695
  br label %1697

1697:                                             ; preds = %1691, %1688
  %.178 = phi i32 [ %1696, %1691 ], [ %.176, %1688 ]
  br label %1698

1698:                                             ; preds = %1697, %1687
  %.179 = phi i32 [ %.177, %1687 ], [ %.178, %1697 ]
  %1699 = sdiv i32 %.54213, 2
  %1700 = sdiv i32 %.54273, 2
  %1701 = add nsw i32 %.54153, 1
  br label %1673

1702:                                             ; preds = %1673
  br label %1703

1703:                                             ; preds = %1717, %1702
  %.55274 = phi i32 [ %.0301, %1702 ], [ %1719, %1717 ]
  %.55214 = phi i32 [ %.0304, %1702 ], [ %1718, %1717 ]
  %.55154 = phi i32 [ 0, %1702 ], [ %1720, %1717 ]
  %.180 = phi i32 [ 0, %1702 ], [ %.181, %1717 ]
  %1704 = icmp slt i32 %.55154, 16
  br i1 %1704, label %1705, label %1721

1705:                                             ; preds = %1703
  %1706 = srem i32 %.55214, 2
  %1707 = icmp ne i32 %1706, 0
  br i1 %1707, label %1708, label %1717

1708:                                             ; preds = %1705
  %1709 = srem i32 %.55274, 2
  %1710 = icmp ne i32 %1709, 0
  br i1 %1710, label %1711, label %1717

1711:                                             ; preds = %1708
  %1712 = sext i32 %.55154 to i64
  %1713 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1712
  %1714 = load i32, i32* %1713, align 4
  %1715 = mul nsw i32 1, %1714
  %1716 = add nsw i32 %.180, %1715
  br label %1717

1717:                                             ; preds = %1711, %1708, %1705
  %.181 = phi i32 [ %1716, %1711 ], [ %.180, %1708 ], [ %.180, %1705 ]
  %1718 = sdiv i32 %.55214, 2
  %1719 = sdiv i32 %.55274, 2
  %1720 = add nsw i32 %.55154, 1
  br label %1703

1721:                                             ; preds = %1703
  %1722 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1723 = mul nsw i32 %.180, %1722
  br label %1724

1724:                                             ; preds = %1738, %1721
  %.56275 = phi i32 [ 65535, %1721 ], [ %1740, %1738 ]
  %.56215 = phi i32 [ %1723, %1721 ], [ %1739, %1738 ]
  %.56155 = phi i32 [ 0, %1721 ], [ %1741, %1738 ]
  %.182 = phi i32 [ 0, %1721 ], [ %.183, %1738 ]
  %1725 = icmp slt i32 %.56155, 16
  br i1 %1725, label %1726, label %1742

1726:                                             ; preds = %1724
  %1727 = srem i32 %.56215, 2
  %1728 = icmp ne i32 %1727, 0
  br i1 %1728, label %1729, label %1738

1729:                                             ; preds = %1726
  %1730 = srem i32 %.56275, 2
  %1731 = icmp ne i32 %1730, 0
  br i1 %1731, label %1732, label %1738

1732:                                             ; preds = %1729
  %1733 = sext i32 %.56155 to i64
  %1734 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1733
  %1735 = load i32, i32* %1734, align 4
  %1736 = mul nsw i32 1, %1735
  %1737 = add nsw i32 %.182, %1736
  br label %1738

1738:                                             ; preds = %1732, %1729, %1726
  %.183 = phi i32 [ %1737, %1732 ], [ %.182, %1729 ], [ %.182, %1726 ]
  %1739 = sdiv i32 %.56215, 2
  %1740 = sdiv i32 %.56275, 2
  %1741 = add nsw i32 %.56155, 1
  br label %1724

1742:                                             ; preds = %1724
  br label %1670

1743:                                             ; preds = %1670
  br label %1744

1744:                                             ; preds = %1743, %1667
  %.1316 = phi i32 [ %.0304, %1743 ], [ %.0315, %1667 ]
  br label %1745

1745:                                             ; preds = %1817, %1744
  %.0294 = phi i32 [ %.0326, %1744 ], [ %.184, %1817 ]
  %.0291 = phi i32 [ %.0326, %1744 ], [ %.190, %1817 ]
  %1746 = icmp ne i32 %.0291, 0
  br i1 %1746, label %1747, label %1818

1747:                                             ; preds = %1745
  br label %1748

1748:                                             ; preds = %1773, %1747
  %.57276 = phi i32 [ %.0291, %1747 ], [ %1775, %1773 ]
  %.57216 = phi i32 [ %.0294, %1747 ], [ %1774, %1773 ]
  %.57156 = phi i32 [ 0, %1747 ], [ %1776, %1773 ]
  %.184 = phi i32 [ 0, %1747 ], [ %.187, %1773 ]
  %1749 = icmp slt i32 %.57156, 16
  br i1 %1749, label %1750, label %1777

1750:                                             ; preds = %1748
  %1751 = srem i32 %.57216, 2
  %1752 = icmp ne i32 %1751, 0
  br i1 %1752, label %1753, label %1763

1753:                                             ; preds = %1750
  %1754 = srem i32 %.57276, 2
  %1755 = icmp eq i32 %1754, 0
  br i1 %1755, label %1756, label %1762

1756:                                             ; preds = %1753
  %1757 = sext i32 %.57156 to i64
  %1758 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1757
  %1759 = load i32, i32* %1758, align 4
  %1760 = mul nsw i32 1, %1759
  %1761 = add nsw i32 %.184, %1760
  br label %1762

1762:                                             ; preds = %1756, %1753
  %.185 = phi i32 [ %1761, %1756 ], [ %.184, %1753 ]
  br label %1773

1763:                                             ; preds = %1750
  %1764 = srem i32 %.57276, 2
  %1765 = icmp ne i32 %1764, 0
  br i1 %1765, label %1766, label %1772

1766:                                             ; preds = %1763
  %1767 = sext i32 %.57156 to i64
  %1768 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1767
  %1769 = load i32, i32* %1768, align 4
  %1770 = mul nsw i32 1, %1769
  %1771 = add nsw i32 %.184, %1770
  br label %1772

1772:                                             ; preds = %1766, %1763
  %.186 = phi i32 [ %1771, %1766 ], [ %.184, %1763 ]
  br label %1773

1773:                                             ; preds = %1772, %1762
  %.187 = phi i32 [ %.185, %1762 ], [ %.186, %1772 ]
  %1774 = sdiv i32 %.57216, 2
  %1775 = sdiv i32 %.57276, 2
  %1776 = add nsw i32 %.57156, 1
  br label %1748

1777:                                             ; preds = %1748
  br label %1778

1778:                                             ; preds = %1792, %1777
  %.58277 = phi i32 [ %.0291, %1777 ], [ %1794, %1792 ]
  %.58217 = phi i32 [ %.0294, %1777 ], [ %1793, %1792 ]
  %.58157 = phi i32 [ 0, %1777 ], [ %1795, %1792 ]
  %.188 = phi i32 [ 0, %1777 ], [ %.189, %1792 ]
  %1779 = icmp slt i32 %.58157, 16
  br i1 %1779, label %1780, label %1796

1780:                                             ; preds = %1778
  %1781 = srem i32 %.58217, 2
  %1782 = icmp ne i32 %1781, 0
  br i1 %1782, label %1783, label %1792

1783:                                             ; preds = %1780
  %1784 = srem i32 %.58277, 2
  %1785 = icmp ne i32 %1784, 0
  br i1 %1785, label %1786, label %1792

1786:                                             ; preds = %1783
  %1787 = sext i32 %.58157 to i64
  %1788 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1787
  %1789 = load i32, i32* %1788, align 4
  %1790 = mul nsw i32 1, %1789
  %1791 = add nsw i32 %.188, %1790
  br label %1792

1792:                                             ; preds = %1786, %1783, %1780
  %.189 = phi i32 [ %1791, %1786 ], [ %.188, %1783 ], [ %.188, %1780 ]
  %1793 = sdiv i32 %.58217, 2
  %1794 = sdiv i32 %.58277, 2
  %1795 = add nsw i32 %.58157, 1
  br label %1778

1796:                                             ; preds = %1778
  %1797 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 1), align 4
  %1798 = mul nsw i32 %.188, %1797
  br label %1799

1799:                                             ; preds = %1813, %1796
  %.59278 = phi i32 [ 65535, %1796 ], [ %1815, %1813 ]
  %.59218 = phi i32 [ %1798, %1796 ], [ %1814, %1813 ]
  %.59158 = phi i32 [ 0, %1796 ], [ %1816, %1813 ]
  %.190 = phi i32 [ 0, %1796 ], [ %.191, %1813 ]
  %1800 = icmp slt i32 %.59158, 16
  br i1 %1800, label %1801, label %1817

1801:                                             ; preds = %1799
  %1802 = srem i32 %.59218, 2
  %1803 = icmp ne i32 %1802, 0
  br i1 %1803, label %1804, label %1813

1804:                                             ; preds = %1801
  %1805 = srem i32 %.59278, 2
  %1806 = icmp ne i32 %1805, 0
  br i1 %1806, label %1807, label %1813

1807:                                             ; preds = %1804
  %1808 = sext i32 %.59158 to i64
  %1809 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1808
  %1810 = load i32, i32* %1809, align 4
  %1811 = mul nsw i32 1, %1810
  %1812 = add nsw i32 %.190, %1811
  br label %1813

1813:                                             ; preds = %1807, %1804, %1801
  %.191 = phi i32 [ %1812, %1807 ], [ %.190, %1804 ], [ %.190, %1801 ]
  %1814 = sdiv i32 %.59218, 2
  %1815 = sdiv i32 %.59278, 2
  %1816 = add nsw i32 %.59158, 1
  br label %1799

1817:                                             ; preds = %1799
  br label %1745

1818:                                             ; preds = %1745
  %1819 = icmp sge i32 1, 15
  br i1 %1819, label %1820, label %1825

1820:                                             ; preds = %1818
  %1821 = icmp slt i32 %.0319, 0
  br i1 %1821, label %1822, label %1823

1822:                                             ; preds = %1820
  br label %1824

1823:                                             ; preds = %1820
  br label %1824

1824:                                             ; preds = %1823, %1822
  %.192 = phi i32 [ 65535, %1822 ], [ 0, %1823 ]
  br label %1849

1825:                                             ; preds = %1818
  %1826 = icmp sgt i32 1, 0
  br i1 %1826, label %1827, label %1847

1827:                                             ; preds = %1825
  %1828 = icmp sgt i32 %.0319, 32767
  br i1 %1828, label %1829, label %1841

1829:                                             ; preds = %1827
  %1830 = sext i32 1 to i64
  %1831 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1830
  %1832 = load i32, i32* %1831, align 4
  %1833 = sdiv i32 %.0319, %1832
  %1834 = add nsw i32 %1833, 65536
  %1835 = sub nsw i32 15, 1
  %1836 = add nsw i32 %1835, 1
  %1837 = sext i32 %1836 to i64
  %1838 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1837
  %1839 = load i32, i32* %1838, align 4
  %1840 = sub nsw i32 %1834, %1839
  br label %1846

1841:                                             ; preds = %1827
  %1842 = sext i32 1 to i64
  %1843 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1842
  %1844 = load i32, i32* %1843, align 4
  %1845 = sdiv i32 %.0319, %1844
  br label %1846

1846:                                             ; preds = %1841, %1829
  %.193 = phi i32 [ %1840, %1829 ], [ %1845, %1841 ]
  br label %1848

1847:                                             ; preds = %1825
  br label %1848

1848:                                             ; preds = %1847, %1846
  %.194 = phi i32 [ %.193, %1846 ], [ %.0319, %1847 ]
  br label %1849

1849:                                             ; preds = %1848, %1824
  %.195 = phi i32 [ %.192, %1824 ], [ %.194, %1848 ]
  br label %1646

1850:                                             ; preds = %1646
  %1851 = icmp sge i32 1, 15
  br i1 %1851, label %1852, label %1857

1852:                                             ; preds = %1850
  %1853 = icmp slt i32 %.0356, 0
  br i1 %1853, label %1854, label %1855

1854:                                             ; preds = %1852
  br label %1856

1855:                                             ; preds = %1852
  br label %1856

1856:                                             ; preds = %1855, %1854
  %.196 = phi i32 [ 65535, %1854 ], [ 0, %1855 ]
  br label %1881

1857:                                             ; preds = %1850
  %1858 = icmp sgt i32 1, 0
  br i1 %1858, label %1859, label %1879

1859:                                             ; preds = %1857
  %1860 = icmp sgt i32 %.0356, 32767
  br i1 %1860, label %1861, label %1873

1861:                                             ; preds = %1859
  %1862 = sext i32 1 to i64
  %1863 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1862
  %1864 = load i32, i32* %1863, align 4
  %1865 = sdiv i32 %.0356, %1864
  %1866 = add nsw i32 %1865, 65536
  %1867 = sub nsw i32 15, 1
  %1868 = add nsw i32 %1867, 1
  %1869 = sext i32 %1868 to i64
  %1870 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1869
  %1871 = load i32, i32* %1870, align 4
  %1872 = sub nsw i32 %1866, %1871
  br label %1878

1873:                                             ; preds = %1859
  %1874 = sext i32 1 to i64
  %1875 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1874
  %1876 = load i32, i32* %1875, align 4
  %1877 = sdiv i32 %.0356, %1876
  br label %1878

1878:                                             ; preds = %1873, %1861
  %.197 = phi i32 [ %1872, %1861 ], [ %1877, %1873 ]
  br label %1880

1879:                                             ; preds = %1857
  br label %1880

1880:                                             ; preds = %1879, %1878
  %.198 = phi i32 [ %.197, %1878 ], [ %.0356, %1879 ]
  br label %1881

1881:                                             ; preds = %1880, %1856
  %.199 = phi i32 [ %.196, %1856 ], [ %.198, %1880 ]
  br label %1416

1882:                                             ; preds = %1416
  %1883 = sext i32 %.1280 to i64
  %1884 = getelementptr inbounds [16 x i32], [16 x i32]* @SHIFT_TABLE, i64 0, i64 %1883
  %1885 = load i32, i32* %1884, align 4
  %1886 = icmp ne i32 %1885, %.0357
  br i1 %1886, label %1887, label %1888

1887:                                             ; preds = %1882
  br label %1891

1888:                                             ; preds = %1882
  %1889 = add nsw i32 %.1280, 1
  br label %1413

1890:                                             ; preds = %1413
  br label %1891

1891:                                             ; preds = %1890, %1887
  %.0 = phi i32 [ 1, %1887 ], [ 0, %1890 ]
  ret i32 %.0
}

declare dso_local i32 @putint(...) #1

declare dso_local i32 @putch(...) #1

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = call i32 @long_func()
  ret i32 %1
}

attributes #0 = { noinline nounwind uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 11.0.1"}
