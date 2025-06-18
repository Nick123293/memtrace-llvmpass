; ModuleID = 'test-file.ll'
source_filename = "test-file.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

@_MemoryTraceFP = global ptr null
@FopenFileNameStr = global [18 x i8] c"memory-traces.log\00"
@FopenModeStr = global [3 x i8] c"w+\00"
@TraceLoadStr = global [41 x i8] c"[Read] Read value 0x%lx from address %p\0A\00"
@TraceStoreStr = global [41 x i8] c"[Write] Wrote value 0x%lx to address %p\0A\00"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = call ptr @fopen(ptr @FopenFileNameStr, ptr @FopenModeStr)
  store ptr %1, ptr @_MemoryTraceFP, align 8
  %castTo64 = ptrtoint ptr %1 to i64
  call void @_TraceMemory(ptr @_MemoryTraceFP, i64 %castTo64, i32 0)
  %2 = alloca i32, align 4
  store i32 0, ptr %2, align 4
  call void @_TraceMemory(ptr %2, i64 0, i32 0)
  ret i32 0
}

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

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (https://github.com/llvm/llvm-project.git cd708029e0b2869e80abe31ddb175f7c35361f90)"}
