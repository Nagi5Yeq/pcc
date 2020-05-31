; ModuleID = 'a'
source_filename = "../../../t/trivial.pas"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-windows-msvc"

%record = type { i32, [10 x i32], float }
%record.1 = type { i32, i8, [5 x float], [2 x %record.0] }
%record.0 = type { [3 x i32], i1 }

@0 = private unnamed_addr constant i1 true
@1 = private unnamed_addr constant i8 97
@2 = private unnamed_addr constant i32 5
@3 = private unnamed_addr constant float 0x400921CAC0000000
@4 = private unnamed_addr constant [5 x i8] c"nagi\00"
@5 = private unnamed_addr constant i8* getelementptr inbounds ([5 x i8], [5 x i8]* @4, i32 0, i32 0)
@6 = private unnamed_addr global i32 0
@7 = private unnamed_addr global float 0.000000e+00
@8 = private unnamed_addr global i8* null
@9 = private unnamed_addr global [5 x i32] zeroinitializer
@10 = private unnamed_addr constant [12 x i8] c"hello world\00"
@11 = private unnamed_addr constant [9 x i8] c"testabcd\00"
@12 = private unnamed_addr constant i1 false
@13 = private unnamed_addr constant i8 65
@14 = private unnamed_addr constant i32 10
@15 = private unnamed_addr constant float 0x4005BF09A0000000
@16 = private unnamed_addr constant [10 x i8] c"Hanjie Wu\00"
@17 = private unnamed_addr constant i8* getelementptr inbounds ([10 x i8], [10 x i8]* @16, i32 0, i32 0)

define i32 @arithmetic_test(i32 %0, i8 %1, float %2) {
arithmetic_test:
  %3 = alloca i32
  store i32 %0, i32* %3
  %4 = alloca i8
  store i8 %1, i8* %4
  %5 = alloca float
  store float %2, float* %5
  %6 = alloca i32
  %7 = alloca i8
  %8 = alloca float
  %9 = alloca i32
  store i32 1, i32* %3
  store i32 -2, i32* %6
  store i8 66, i8* %4
  store i8 70, i8* %7
  store float 1.500000e+00, float* %5
  store float 0xC0091EB860000000, float* %8
  %10 = load i32, i32* %3
  %11 = load i8, i8* %4
  %12 = sext i8 %11 to i32
  %13 = add i32 %10, %12
  %14 = load float, float* %5
  %15 = sitofp i32 %13 to float
  %16 = fsub float %15, %14
  %17 = load i32, i32* %6
  %18 = sitofp i32 %17 to float
  %19 = fadd float %16, %18
  %20 = fptosi float %19 to i32
  store i32 %20, i32* %3
  %21 = load i8, i8* %4
  %22 = load float, float* %5
  %23 = sitofp i8 %21 to float
  %24 = fmul float %23, %22
  %25 = load i32, i32* %6
  %26 = load i8, i8* %7
  %27 = sext i8 %26 to i32
  %28 = mul i32 %25, %27
  %29 = sitofp i32 %28 to float
  %30 = fsub float %24, %29
  %31 = fptosi float %30 to i8
  store i8 %31, i8* %4
  %32 = load float, float* %5
  %33 = load i32, i32* %6
  %34 = load i8, i8* %7
  %35 = sext i8 %34 to i32
  %36 = mul i32 %33, %35
  %37 = sitofp i32 %36 to float
  %38 = fadd float %32, %37
  %39 = load float, float* %8
  %40 = fadd float %38, %39
  store float %40, float* %5
  store i8 1, i8* %4
  store i8 2, i8* %7
  %41 = load i8, i8* %4
  %42 = load i8, i8* %7
  %43 = sitofp i8 %41 to float
  %44 = sitofp i8 %42 to float
  %45 = fdiv float %43, %44
  %46 = fadd float 5.000000e-01, %45
  %47 = fptosi float %46 to i32
  store i32 %47, i32* %3
  %48 = load i8, i8* %4
  %49 = load i8, i8* %7
  %50 = sdiv i8 %48, %49
  %51 = sext i8 %50 to i32
  %52 = add i32 0, %51
  store i32 %52, i32* %6
  %53 = load i32, i32* %3
  %54 = load i8, i8* %7
  %55 = sext i8 %54 to i32
  %56 = and i32 %53, %55
  %57 = load i8, i8* %4
  %58 = load i32, i32* %6
  %59 = sext i8 %57 to i32
  %60 = xor i32 %59, %58
  %61 = or i32 %56, %60
  store i32 %61, i32* %3
  %62 = load i32, i32* %3
  %63 = shl i32 %62, 3
  %64 = load i32, i32* %3
  %65 = xor i32 %64, -1
  %66 = load i8, i8* %4
  %67 = sext i8 %66 to i32
  %68 = ashr i32 %65, %67
  %69 = xor i32 %63, %68
  store i32 %69, i32* %6
  %70 = load i32, i32* %3
  %71 = load i32, i32* %6
  %72 = add i32 %70, %71
  store i32 %72, i32* %9
  %73 = load i32, i32* %9
  ret i32 %73
}

define i8 @string_test() {
string_test:
  %0 = alloca i8*
  %1 = alloca i8
  %2 = alloca i8
  %3 = alloca i8
  store i8* getelementptr inbounds ([12 x i8], [12 x i8]* @10, i32 0, i32 0), i8** %0
  %4 = load i8*, i8** %0
  %5 = getelementptr inbounds i8, i8* %4, i32 3
  %6 = load i8, i8* %5
  store i8 %6, i8* %1
  %7 = load i8*, i8** %0
  %8 = getelementptr inbounds i8, i8* %7, i32 6
  %9 = load i8, i8* %8
  store i8 %9, i8* %2
  %10 = load i8, i8* %2
  %11 = load i8, i8* %1
  %12 = sub i8 %10, %11
  %13 = load i8*, i8** %0
  %14 = sext i8 %12 to i32
  %15 = getelementptr inbounds i8, i8* %13, i32 %14
  %16 = load i8, i8* %15
  store i8 %16, i8* %3
  %17 = load i8, i8* %3
  ret i8 %17
}

define i32 @array_test() {
array_test:
  %0 = alloca [26 x i32]
  %1 = alloca [26 x i8]
  %2 = alloca i32
  %3 = alloca i8
  %4 = alloca i32
  store i32 0, i32* %2
  store i8 122, i8* %3
  br label %loop

loop:                                             ; preds = %check, %array_test
  %5 = load i32, i32* %2
  %6 = getelementptr inbounds [26 x i8], [26 x i8]* %1, i32 0, i32 %5
  %7 = load i8, i8* %3
  store i8 %7, i8* %6
  %8 = load i8, i8* %3
  %9 = sub i8 %8, 97
  %10 = getelementptr inbounds [26 x i32], [26 x i32]* %0, i32 0, i8 %9
  %11 = load i32, i32* %2
  store i32 %11, i32* %10
  %12 = load i32, i32* %2
  %13 = add i32 %12, 1
  store i32 %13, i32* %2
  %14 = load i8, i8* %3
  %15 = sext i8 %14 to i32
  %16 = sub i32 %15, 1
  %17 = trunc i32 %16 to i8
  store i8 %17, i8* %3
  br label %check

check:                                            ; preds = %loop
  %18 = load i32, i32* %2
  %19 = icmp sgt i32 %18, 25
  br i1 %19, label %done, label %loop

done:                                             ; preds = %check
  %20 = getelementptr inbounds [26 x i32], [26 x i32]* %0, i32 0, i8 5
  %21 = load i32, i32* %20
  %22 = getelementptr inbounds [26 x i8], [26 x i8]* %1, i32 0, i32 5
  %23 = load i8, i8* %22
  %24 = sext i8 %23 to i32
  %25 = sub i32 %21, %24
  store i32 %25, i32* %4
  %26 = load i32, i32* %4
  ret i32 %26
}

define i32 @pointer_test() {
pointer_test:
  %0 = alloca i32
  %1 = alloca i32
  %2 = alloca [101 x i32]
  %3 = alloca i32*
  %4 = alloca i32*
  %5 = alloca i8*
  %6 = alloca i8*
  %7 = alloca i32
  store i32 1, i32* %0
  %8 = getelementptr inbounds [101 x i32], [101 x i32]* %2, i32 0, i32 0
  store i32* %8, i32** %3
  %9 = getelementptr inbounds [101 x i32], [101 x i32]* %2, i32 0, i32 4
  store i32* %9, i32** %4
  %10 = load i32*, i32** %4
  %11 = load i32*, i32** %3
  %12 = ptrtoint i32* %10 to i32
  %13 = ptrtoint i32* %11 to i32
  %14 = sub i32 %12, %13
  store i32 %14, i32* %1
  %15 = load i32*, i32** %3
  %16 = getelementptr inbounds i32, i32* %15, i32 0
  %17 = load i32, i32* %0
  store i32 %17, i32* %16
  %18 = load i32, i32* %1
  %19 = load i32*, i32** %4
  %20 = getelementptr inbounds i32, i32* %19, i32 %18
  %21 = load i32, i32* %0
  store i32 %21, i32* %20
  %22 = load i32*, i32** %3
  %23 = getelementptr inbounds i32, i32* %22, i32 1
  %24 = getelementptr inbounds i32, i32* %23, i32 0
  %25 = load i32, i32* %0
  %26 = add i32 %25, 1
  store i32 %26, i32* %24
  %27 = load i32*, i32** %3
  %28 = getelementptr inbounds i32, i32* %27, i32 0
  %29 = load i32, i32* %28
  %30 = load i32*, i32** %3
  %31 = getelementptr inbounds i32, i32* %30, i32 1
  %32 = load i32, i32* %31
  %33 = add i32 %29, %32
  store i32 %33, i32* %1
  store i8* getelementptr inbounds ([9 x i8], [9 x i8]* @11, i32 0, i32 0), i8** %5
  %34 = load i8*, i8** %5
  %35 = getelementptr inbounds i8, i8* %34, i32 2
  store i8* %35, i8** %6
  %36 = load i32, i32* %1
  %37 = load i8*, i8** %6
  %38 = getelementptr inbounds i8, i8* %37, i32 3
  %39 = load i8, i8* %38
  %40 = sext i8 %39 to i32
  %41 = add i32 %36, %40
  store i32 %41, i32* %7
  %42 = load i32, i32* %7
  ret i32 %42
}

define i32 @recursion_test(i32 %0) {
recursion_test:
  %1 = alloca i32
  store i32 %0, i32* %1
  %2 = alloca i32
  %3 = load i32, i32* %1
  %4 = icmp eq i32 %3, 0
  %5 = load i32, i32* %1
  %6 = icmp eq i32 %5, 1
  %7 = or i1 %4, %6
  br i1 %7, label %then, label %else

then:                                             ; preds = %recursion_test
  store i32 1, i32* %2
  br label %endif

else:                                             ; preds = %recursion_test
  %8 = load i32, i32* %1
  %9 = sub i32 %8, 1
  %10 = call i32 @recursion_test(i32 %9)
  %11 = load i32, i32* %1
  %12 = sub i32 %11, 2
  %13 = call i32 @recursion_test(i32 %12)
  %14 = add i32 %10, %13
  store i32 %14, i32* %2
  br label %endif

endif:                                            ; preds = %else, %then
  %15 = load i32, i32* %2
  ret i32 %15
}

define i32 @branch_test(i32 %0) {
branch_test:
  %1 = alloca i32
  store i32 %0, i32* %1
  %2 = alloca i32
  %3 = alloca i32
  %4 = alloca i32
  %5 = alloca i32
  store i32 3, i32* %2
  store i32 0, i32* %3
  store i32 4, i32* %4
  %6 = load i32, i32* %1
  %7 = load i32, i32* %2
  %8 = icmp slt i32 %6, %7
  br i1 %8, label %then, label %else

then:                                             ; preds = %branch_test
  store i32 -1, i32* %3
  br label %endif2

else:                                             ; preds = %branch_test
  %9 = load i32, i32* %1
  %10 = load i32, i32* %2
  %11 = icmp sgt i32 %9, %10
  br i1 %11, label %then1, label %endif

then1:                                            ; preds = %else
  store i32 1, i32* %3
  br label %endif

endif:                                            ; preds = %then1, %else
  br label %endif2

endif2:                                           ; preds = %endif, %then
  br label %start

start:                                            ; preds = %done, %endif2
  %12 = load i32, i32* %2
  %13 = icmp sgt i32 %12, 0
  br i1 %13, label %loop, label %done7

loop:                                             ; preds = %start
  br label %start3

start3:                                           ; preds = %endif6, %loop
  %14 = load i32, i32* %4
  %15 = icmp sgt i32 %14, 0
  br i1 %15, label %loop4, label %done

loop4:                                            ; preds = %start3
  %16 = load i32, i32* %4
  %17 = sub i32 %16, 1
  store i32 %17, i32* %4
  %18 = load i32, i32* %4
  %19 = icmp eq i32 %18, 3
  br i1 %19, label %then5, label %endif6

then5:                                            ; preds = %loop4
  store i32 2, i32* %4
  br label %done

endif6:                                           ; preds = %loop4
  %20 = load i32, i32* %1
  %21 = load i32, i32* %4
  %22 = mul i32 %20, %21
  %23 = load i32, i32* %2
  %24 = add i32 %22, %23
  store i32 %24, i32* %1
  br label %start3

done:                                             ; preds = %then5, %start3
  %25 = load i32, i32* %2
  %26 = sub i32 %25, 1
  store i32 %26, i32* %2
  br label %start

done7:                                            ; preds = %start
  br label %loop8

loop8:                                            ; preds = %check13, %done7
  br label %loop9

loop9:                                            ; preds = %check, %loop8
  %27 = load i32, i32* %4
  %28 = add i32 %27, 1
  store i32 %28, i32* %4
  %29 = load i32, i32* %4
  %30 = icmp eq i32 %29, 3
  br i1 %30, label %then10, label %endif11

then10:                                           ; preds = %loop9
  store i32 4, i32* %4
  br label %check

endif11:                                          ; preds = %loop9
  %31 = load i32, i32* %1
  %32 = load i32, i32* %3
  %33 = mul i32 %31, %32
  %34 = load i32, i32* %4
  %35 = add i32 %33, %34
  store i32 %35, i32* %1
  br label %check

check:                                            ; preds = %endif11, %then10
  %36 = load i32, i32* %4
  %37 = icmp sgt i32 %36, 5
  br i1 %37, label %done12, label %loop9

done12:                                           ; preds = %check
  %38 = load i32, i32* %1
  %39 = load i32, i32* %3
  %40 = mul i32 %38, %39
  store i32 %40, i32* %1
  %41 = load i32, i32* %2
  %42 = add i32 %41, 1
  store i32 %42, i32* %2
  br label %check13

check13:                                          ; preds = %done12
  %43 = load i32, i32* %2
  %44 = icmp sgt i32 %43, 5
  br i1 %44, label %done14, label %loop8

done14:                                           ; preds = %check13
  %45 = load i32, i32* %1
  store i32 %45, i32* %5
  %46 = load i32, i32* %5
  ret i32 %46
}

define i32 @name_test_global() {
name_test_global:
  %0 = alloca i32
  store i32 1, i32* @6
  %1 = load i32, i32* @6
  %2 = load i1, i1* @0
  %3 = sext i1 %2 to i32
  %4 = add i32 %1, %3
  %5 = load i8, i8* @1
  %6 = sext i8 %5 to i32
  %7 = add i32 %4, %6
  %8 = load i32, i32* @2
  %9 = add i32 %7, %8
  %10 = load float, float* @3
  %11 = sitofp i32 %9 to float
  %12 = fadd float %11, %10
  %13 = load i8*, i8** @5
  %14 = getelementptr inbounds i8, i8* %13, i32 3
  %15 = load i8, i8* %14
  %16 = sitofp i8 %15 to float
  %17 = fadd float %12, %16
  %18 = fptosi float %17 to i32
  store i32 %18, i32* %0
  %19 = load i32, i32* %0
  ret i32 %19
}

define i32 @name_test_local() {
name_test_local:
  %0 = alloca i32
  %1 = alloca i32
  store i32 0, i32* %0
  %2 = load i32, i32* %0
  %3 = load i1, i1* @12
  %4 = sext i1 %3 to i32
  %5 = add i32 %2, %4
  %6 = load i8, i8* @13
  %7 = sext i8 %6 to i32
  %8 = add i32 %5, %7
  %9 = load i32, i32* @14
  %10 = add i32 %8, %9
  %11 = load float, float* @15
  %12 = sitofp i32 %10 to float
  %13 = fadd float %12, %11
  %14 = load i8*, i8** @17
  %15 = getelementptr inbounds i8, i8* %14, i32 3
  %16 = load i8, i8* %15
  %17 = sitofp i8 %16 to float
  %18 = fadd float %13, %17
  %19 = fptosi float %18 to i32
  store i32 %19, i32* %1
  %20 = load i32, i32* %1
  ret i32 %20
}

define i1 @name_test() {
name_test:
  %0 = alloca i1
  %1 = call i32 @name_test_global()
  %2 = call i32 @name_test_local()
  %3 = icmp ne i32 %1, %2
  store i1 %3, i1* %0
  %4 = load i1, i1* %0
  ret i1 %4
}

define void @void_test() {
void_test:
  %0 = alloca i8*
  %1 = alloca i8*
  ret void
}

define i32 @alias_test() {
alias_test:
  %0 = alloca i32
  %1 = alloca [10 x [10 x i32]]
  %2 = alloca [10 x i32]*
  %3 = alloca [10 x [100 x [10 x i32]]]
  %4 = alloca %record
  %5 = alloca i32
  store i32 4, i32* %0
  %6 = load i32, i32* %0
  %7 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* %1, i32 0, i32 %6
  %8 = load i32, i32* %0
  %9 = getelementptr inbounds [10 x i32], [10 x i32]* %7, i32 0, i32 %8
  %10 = load i32, i32* %0
  store i32 %10, i32* %9
  %11 = load i32, i32* %0
  %12 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* %1, i32 0, i32 %11
  store [10 x i32]* %12, [10 x i32]** %2
  %13 = load [10 x i32]*, [10 x i32]** %2
  %14 = getelementptr inbounds [10 x i32], [10 x i32]* %13, i32 0
  %15 = load i32, i32* %0
  %16 = getelementptr inbounds [10 x i32], [10 x i32]* %14, i32 0, i32 %15
  %17 = load [10 x i32]*, [10 x i32]** %2
  %18 = getelementptr inbounds [10 x i32], [10 x i32]* %17, i32 0
  %19 = load i32, i32* %0
  %20 = getelementptr inbounds [10 x i32], [10 x i32]* %18, i32 0, i32 %19
  %21 = load i32, i32* %20
  %22 = add i32 %21, 1
  store i32 %22, i32* %16
  %23 = getelementptr inbounds [10 x [100 x [10 x i32]]], [10 x [100 x [10 x i32]]]* %3, i32 0, i32 0
  %24 = getelementptr inbounds [100 x [10 x i32]], [100 x [10 x i32]]* %23, i32 0, i32 1
  %25 = getelementptr inbounds [10 x i32], [10 x i32]* %24, i32 0, i32 2
  store i32 123, i32* %25
  %26 = getelementptr inbounds [10 x [100 x [10 x i32]]], [10 x [100 x [10 x i32]]]* %3, i32 0, i32 0
  %27 = getelementptr inbounds [100 x [10 x i32]], [100 x [10 x i32]]* %26, i32 0, i32 1
  %28 = getelementptr inbounds [10 x i32], [10 x i32]* %27, i32 0, i32 2
  %29 = load i32, i32* %28
  %30 = load i32, i32* %0
  %31 = getelementptr inbounds [10 x [10 x i32]], [10 x [10 x i32]]* %1, i32 0, i32 %30
  %32 = load i32, i32* %0
  %33 = getelementptr inbounds [10 x i32], [10 x i32]* %31, i32 0, i32 %32
  %34 = load i32, i32* %33
  %35 = load i32, i32* %0
  %36 = mul i32 %34, %35
  %37 = add i32 %29, %36
  store i32 %37, i32* %5
  %38 = load i32, i32* %5
  ret i32 %38
}

define i32 @record_test() {
record_test:
  %0 = alloca %record.1
  %1 = alloca %record.1*
  %2 = alloca i32
  %3 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 0
  store i32 2, i32* %3
  %4 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 1
  store i8 116, i8* %4
  %5 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 2
  %6 = getelementptr inbounds [5 x float], [5 x float]* %5, i32 0, i32 0
  store float 0x401AA3D700000000, float* %6
  store %record.1* %0, %record.1** %1
  %7 = load %record.1*, %record.1** %1
  %8 = getelementptr inbounds %record.1, %record.1* %7, i32 0
  %9 = getelementptr inbounds %record.1, %record.1* %8, i32 0, i32 2
  %10 = getelementptr inbounds [5 x float], [5 x float]* %9, i32 0, i32 4
  store float 0x401F147AE0000000, float* %10
  %11 = load %record.1*, %record.1** %1
  %12 = getelementptr inbounds %record.1, %record.1* %11, i32 0
  %13 = getelementptr inbounds %record.1, %record.1* %12, i32 0, i32 3
  %14 = getelementptr inbounds [2 x %record.0], [2 x %record.0]* %13, i32 0, i32 0
  %15 = getelementptr inbounds %record.0, %record.0* %14, i32 0, i32 0
  %16 = getelementptr inbounds [3 x i32], [3 x i32]* %15, i32 0, i32 2
  store i32 1234, i32* %16
  %17 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 2
  %18 = getelementptr inbounds [5 x float], [5 x float]* %17, i32 0, i32 4
  %19 = load float, float* %18
  %20 = load %record.1*, %record.1** %1
  %21 = getelementptr inbounds %record.1, %record.1* %20, i32 0
  %22 = getelementptr inbounds %record.1, %record.1* %21, i32 0, i32 2
  %23 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 0
  %24 = load i32, i32* %23
  %25 = sub i32 %24, 2
  %26 = getelementptr inbounds [5 x float], [5 x float]* %22, i32 0, i32 %25
  %27 = load float, float* %26
  %28 = fadd float %19, %27
  %29 = getelementptr inbounds %record.1, %record.1* %0, i32 0, i32 3
  %30 = getelementptr inbounds [2 x %record.0], [2 x %record.0]* %29, i32 0, i32 0
  %31 = getelementptr inbounds %record.0, %record.0* %30, i32 0, i32 0
  %32 = getelementptr inbounds [3 x i32], [3 x i32]* %31, i32 0, i32 2
  %33 = load i32, i32* %32
  %34 = sitofp i32 %33 to float
  %35 = fadd float %28, %34
  %36 = fptosi float %35 to i32
  store i32 %36, i32* %2
  %37 = load i32, i32* %2
  ret i32 %37
}
