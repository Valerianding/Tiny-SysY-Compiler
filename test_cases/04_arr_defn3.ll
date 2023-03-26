define dso_local i32 @main() #0{
 %1 = alloca [4 x [2 x i32]],align 16
 %2 = alloca [4 x [2 x i32]],align 16
 %3 = alloca [4 x [2 x i32]],align 16
 %4 = alloca [4 x [2 x i32]],align 16
 %5 = alloca [4 x [2 x i32]],align 16
 %6=bitcast [4 x [2 x i32]]* %1 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %6, i8 0, i64 32, i1 false)
 %7=bitcast [4 x [2 x i32]]* %5 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %7, i8 0, i64 32, i1 false)
 %8=bitcast i8* %7 to [4 x [2 x i32]]*
 %9=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %8, i32 0,i32 0
 %10=getelementptr inbounds [2 x i32],[2 x i32]* %9, i32 0,i32 0
 store i32 1,i32* %10,align 4
 %11=getelementptr inbounds [2 x i32],[2 x i32]* %9, i32 0,i32 1
 store i32 2,i32* %11,align 4
 %12=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %8, i32 0,i32 1
 %13=getelementptr inbounds [2 x i32],[2 x i32]* %12, i32 0,i32 0
 store i32 3,i32* %13,align 4
 %14=getelementptr inbounds [2 x i32],[2 x i32]* %12, i32 0,i32 1
 store i32 4,i32* %14,align 4
 %15=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %8, i32 0,i32 2
 %16=getelementptr inbounds [2 x i32],[2 x i32]* %15, i32 0,i32 0
 store i32 5,i32* %16,align 4
 %17=getelementptr inbounds [2 x i32],[2 x i32]* %15, i32 0,i32 1
 store i32 6,i32* %17,align 4
 %18=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %8, i32 0,i32 3
 %19=getelementptr inbounds [2 x i32],[2 x i32]* %18, i32 0,i32 0
 store i32 7,i32* %19,align 4
 %20=getelementptr inbounds [2 x i32],[2 x i32]* %18, i32 0,i32 1
 store i32 8,i32* %20,align 4
 %21=bitcast [4 x [2 x i32]]* %4 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %21, i8 0, i64 32, i1 false)
 %22=bitcast i8* %21 to [4 x [2 x i32]]*
 %23=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %22, i32 0,i32 0
 %24=getelementptr inbounds [2 x i32],[2 x i32]* %23, i32 0,i32 0
 store i32 1,i32* %24,align 4
 %25=getelementptr inbounds [2 x i32],[2 x i32]* %23, i32 0,i32 1
 store i32 2,i32* %25,align 4
 %26=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %22, i32 0,i32 1
 %27=getelementptr inbounds [2 x i32],[2 x i32]* %26, i32 0,i32 0
 store i32 3,i32* %27,align 4
 %28=getelementptr inbounds [2 x i32],[2 x i32]* %26, i32 0,i32 1
 store i32 4,i32* %28,align 4
 %29=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %22, i32 0,i32 2
 %30=getelementptr inbounds [2 x i32],[2 x i32]* %29, i32 0,i32 0
 store i32 5,i32* %30,align 4
 %31=getelementptr inbounds [2 x i32],[2 x i32]* %29, i32 0,i32 1
 store i32 6,i32* %31,align 4
 %32=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %22, i32 0,i32 3
 %33=getelementptr inbounds [2 x i32],[2 x i32]* %32, i32 0,i32 0
 store i32 7,i32* %33,align 4
 %34=getelementptr inbounds [2 x i32],[2 x i32]* %32, i32 0,i32 1
 store i32 8,i32* %34,align 4
 %35=bitcast [4 x [2 x i32]]* %2 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %35, i8 0, i64 32, i1 false)
 %36=bitcast i8* %35 to [4 x [2 x i32]]*
 %37=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %36, i32 0,i32 0
 %38=getelementptr inbounds [2 x i32],[2 x i32]* %37, i32 0,i32 0
 store i32 1,i32* %38,align 4
 %39=getelementptr inbounds [2 x i32],[2 x i32]* %37, i32 0,i32 1
 store i32 2,i32* %39,align 4
 %40=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %36, i32 0,i32 1
 %41=getelementptr inbounds [2 x i32],[2 x i32]* %40, i32 0,i32 0
 store i32 3,i32* %41,align 4
 %42=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %36, i32 0,i32 2
 %43=getelementptr inbounds [2 x i32],[2 x i32]* %42, i32 0,i32 0
 store i32 5,i32* %43,align 4
 %44=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %36, i32 0,i32 3
 %45=getelementptr inbounds [2 x i32],[2 x i32]* %44, i32 0,i32 0
 store i32 7,i32* %45,align 4
 %46=getelementptr inbounds [2 x i32],[2 x i32]* %44, i32 0,i32 1
 store i32 8,i32* %46,align 4
 %47=bitcast [4 x [2 x i32]]* %3 to i8*
 call void @llvm.memset.p0i8.i64(i8* align 16 %47, i8 0, i64 32, i1 false)
 %48=bitcast i8* %47 to [4 x [2 x i32]]*
 %49=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %48, i32 0,i32 0
 %50=getelementptr inbounds [2 x i32],[2 x i32]* %49, i32 0,i32 0
 %51=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %2, i32 0,i32 2
 %52=getelementptr inbounds [2 x i32],[2 x i32]* %51, i32 0,i32 1
 %53 = load i32,i32* %52,align 4
 store i32 %53,i32* %50,align 4
 %54=getelementptr inbounds [2 x i32],[2 x i32]* %49, i32 0,i32 1
 %55=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %4, i32 0,i32 2
 %56=getelementptr inbounds [2 x i32],[2 x i32]* %55, i32 0,i32 1
 %57 = load i32,i32* %56,align 4
 store i32 %57,i32* %54,align 4
 %58=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %48, i32 0,i32 1
 %59=getelementptr inbounds [2 x i32],[2 x i32]* %58, i32 0,i32 0
 store i32 3,i32* %59,align 4
 %60=getelementptr inbounds [2 x i32],[2 x i32]* %58, i32 0,i32 1
 store i32 4,i32* %60,align 4
 %61=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %48, i32 0,i32 2
 %62=getelementptr inbounds [2 x i32],[2 x i32]* %61, i32 0,i32 0
 store i32 5,i32* %62,align 4
 %63=getelementptr inbounds [2 x i32],[2 x i32]* %61, i32 0,i32 1
 store i32 6,i32* %63,align 4
 %64=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %48, i32 0,i32 3
 %65=getelementptr inbounds [2 x i32],[2 x i32]* %64, i32 0,i32 0
 store i32 7,i32* %65,align 4
 %66=getelementptr inbounds [2 x i32],[2 x i32]* %64, i32 0,i32 1
 store i32 8,i32* %66,align 4
 %67=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %3, i32 0,i32 3
 %68=getelementptr inbounds [2 x i32],[2 x i32]* %67, i32 0,i32 1
 %69 = load i32,i32* %68,align 4
 %70=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %3, i32 0,i32 0
 %71=getelementptr inbounds [2 x i32],[2 x i32]* %70, i32 0,i32 0
 %72 = load i32,i32* %71,align 4
 %73= add nsw i32 %69,%72
 %74=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %3, i32 0,i32 0
 %75=getelementptr inbounds [2 x i32],[2 x i32]* %74, i32 0,i32 1
 %76 = load i32,i32* %75,align 4
 %77= add nsw i32 %73,%76
 %78=getelementptr inbounds [4 x [2 x i32]],[4 x [2 x i32]]* %1, i32 0,i32 2
 %79=getelementptr inbounds [2 x i32],[2 x i32]* %78, i32 0,i32 0
 %80 = load i32,i32* %79,align 4
 %81= add nsw i32 %77,%80
 ret i32 %81
}


declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1 immarg) #1
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* noalias nocapture writeonly, i8* noalias nocapture readonly, i64, i1 immarg) #2
declare dso_local i32 @getint(...) #1
declare dso_local i32 @putint(...) #1
declare dso_local i32 @getch(...) #1
declare dso_local i32 @getarray(...) #1
declare dso_local i32 @getfloat(...) #1
declare dso_local i32 @getfarray(...) #1
declare dso_local i32 @putch(...) #1
declare dso_local i32 @putarray(...) #1
declare dso_local i32 @putfloat(...) #1
declare dso_local i32 @putfarray(...) #1
declare dso_local i32 @putf(...) #1
2 @putfarray(...) #1
declare dso_local i32 @putf(...) #1
