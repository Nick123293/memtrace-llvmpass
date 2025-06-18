; ModuleID = '../tests/seq_add.ll'
source_filename = "../tests/seq_add.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@_MemoryTraceFP = global ptr null
@FopenFileNameStr = global [18 x i8] c"memory-traces.log\00"
@FopenModeStr = global [3 x i8] c"w+\00"
@TraceLoadStr = global [41 x i8] c"[Read] Read value 0x%lx from address %p\0A\00"
@TraceStoreStr = global [41 x i8] c"[Write] Wrote value 0x%lx to address %p\0A\00"

; Function Attrs: mustprogress noinline norecurse optnone uwtable
define dso_local noundef i32 @main() #0 {
  %1 = call ptr @fopen(ptr @FopenFileNameStr, ptr @FopenModeStr)
  store ptr %1, ptr @_MemoryTraceFP, align 8
  %castTo64 = ptrtoint ptr %1 to i64
  call void @_TraceMemory(ptr @_MemoryTraceFP, i64 %castTo64, i32 0)
  %2 = alloca i32, align 4
  %3 = alloca ptr, align 8
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  call void @_TraceMemory(ptr %2, i64 0, i32 0)
  %7 = call noalias noundef nonnull ptr @_Znam(i64 noundef 40) #2
  store ptr %7, ptr %3, align 8
  %castTo641 = ptrtoint ptr %7 to i64
  call void @_TraceMemory(ptr %3, i64 %castTo641, i32 0)
  %8 = call noalias noundef nonnull ptr @_Znam(i64 noundef 40) #2
  store ptr %8, ptr %4, align 8
  %castTo642 = ptrtoint ptr %8 to i64
  call void @_TraceMemory(ptr %4, i64 %castTo642, i32 0)
  store i32 0, ptr %5, align 4
  call void @_TraceMemory(ptr %5, i64 0, i32 0)
  br label %9

9:                                                ; preds = %21, %0
  %10 = load i32, ptr %5, align 4
  %castTo643 = zext i32 %10 to i64
  call void @_TraceMemory(ptr %5, i64 %castTo643, i32 1)
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %12, label %24

12:                                               ; preds = %9
  %13 = load ptr, ptr %3, align 8
  %castTo644 = ptrtoint ptr %13 to i64
  call void @_TraceMemory(ptr %3, i64 %castTo644, i32 1)
  %14 = load i32, ptr %5, align 4
  %castTo645 = zext i32 %14 to i64
  call void @_TraceMemory(ptr %5, i64 %castTo645, i32 1)
  %15 = sext i32 %14 to i64
  %16 = getelementptr inbounds i32, ptr %13, i64 %15
  store i32 0, ptr %16, align 4
  call void @_TraceMemory(ptr %16, i64 0, i32 0)
  %17 = load ptr, ptr %4, align 8
  %castTo646 = ptrtoint ptr %17 to i64
  call void @_TraceMemory(ptr %4, i64 %castTo646, i32 1)
  %18 = load i32, ptr %5, align 4
  %castTo647 = zext i32 %18 to i64
  call void @_TraceMemory(ptr %5, i64 %castTo647, i32 1)
  %19 = sext i32 %18 to i64
  %20 = getelementptr inbounds i32, ptr %17, i64 %19
  store i32 1, ptr %20, align 4
  call void @_TraceMemory(ptr %20, i64 1, i32 0)
  br label %21

21:                                               ; preds = %12
  %22 = load i32, ptr %5, align 4
  %castTo648 = zext i32 %22 to i64
  call void @_TraceMemory(ptr %5, i64 %castTo648, i32 1)
  %23 = add nsw i32 %22, 1
  store i32 %23, ptr %5, align 4
  %castTo649 = zext i32 %23 to i64
  call void @_TraceMemory(ptr %5, i64 %castTo649, i32 0)
  br label %9, !llvm.loop !6

24:                                               ; preds = %9
  store i32 0, ptr %6, align 4
  call void @_TraceMemory(ptr %6, i64 0, i32 0)
  br label %25

25:                                               ; preds = %39, %24
  %26 = load i32, ptr %6, align 4
  %castTo6410 = zext i32 %26 to i64
  call void @_TraceMemory(ptr %6, i64 %castTo6410, i32 1)
  %27 = icmp slt i32 %26, 10
  br i1 %27, label %28, label %42

28:                                               ; preds = %25
  %29 = load ptr, ptr %4, align 8
  %castTo6411 = ptrtoint ptr %29 to i64
  call void @_TraceMemory(ptr %4, i64 %castTo6411, i32 1)
  %30 = load i32, ptr %6, align 4
  %castTo6412 = zext i32 %30 to i64
  call void @_TraceMemory(ptr %6, i64 %castTo6412, i32 1)
  %31 = sext i32 %30 to i64
  %32 = getelementptr inbounds i32, ptr %29, i64 %31
  %33 = load i32, ptr %32, align 4
  %castTo6413 = zext i32 %33 to i64
  call void @_TraceMemory(ptr %32, i64 %castTo6413, i32 1)
  %34 = add nsw i32 %33, 1
  %35 = load ptr, ptr %3, align 8
  %castTo6414 = ptrtoint ptr %35 to i64
  call void @_TraceMemory(ptr %3, i64 %castTo6414, i32 1)
  %36 = load i32, ptr %6, align 4
  %castTo6415 = zext i32 %36 to i64
  call void @_TraceMemory(ptr %6, i64 %castTo6415, i32 1)
  %37 = sext i32 %36 to i64
  %38 = getelementptr inbounds i32, ptr %35, i64 %37
  store i32 %34, ptr %38, align 4
  %castTo6416 = zext i32 %34 to i64
  call void @_TraceMemory(ptr %38, i64 %castTo6416, i32 0)
  br label %39

39:                                               ; preds = %28
  %40 = load i32, ptr %6, align 4
  %castTo6417 = zext i32 %40 to i64
  call void @_TraceMemory(ptr %6, i64 %castTo6417, i32 1)
  %41 = add nsw i32 %40, 1
  store i32 %41, ptr %6, align 4
  %castTo6418 = zext i32 %41 to i64
  call void @_TraceMemory(ptr %6, i64 %castTo6418, i32 0)
  br label %25, !llvm.loop !8

42:                                               ; preds = %25
  ret i32 100
}

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znam(i64 noundef) #1

declare ptr @fopen(ptr, ptr)

define void @_TraceMemory(ptr %0, i64 %1, i32 %2) {
entry:
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %traceLoad, label %traceStore

traceLoad:                                        ; preds = %entry
  %4 = load ptr, ptr @_MemoryTraceFP, align 8
  %5 = call i32 (ptr, ptr, ...) @fprintf(ptr %4, ptr @TraceLoadStr, i64 %1, ptr %0)
  ret void

traceStore:                                       ; preds = %entry
  %6 = load ptr, ptr @_MemoryTraceFP, align 8
  %7 = call i32 (ptr, ptr, ...) @fprintf(ptr %6, ptr @TraceStoreStr, i64 %1, ptr %0)
  ret void
}

declare i32 @fprintf(ptr, ptr, ...)

attributes #0 = { mustprogress noinline norecurse optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nobuiltin allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { builtin allocsize(0) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (https://github.com/llvm/llvm-project.git cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
