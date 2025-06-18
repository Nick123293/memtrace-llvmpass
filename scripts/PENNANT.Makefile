########################################################################
#  Directories, filenames
########################################################################
SRCDIR    := src
BUILDDIR  := build
IRDIR     := $(BUILDDIR)/ir# holds .bc and .opt.bc

PRODUCT   := pennant
BINARY    := $(BUILDDIR)/$(PRODUCT)

# --------------------------------------------------------------
#  Derivative file lists (computed once, up front)
# --------------------------------------------------------------
SRCS      := $(wildcard $(SRCDIR)/*.cc)

BCS       := $(patsubst $(SRCDIR)/%.cc,$(IRDIR)/%.bc,$(SRCS))
BCS_OPT   := $(patsubst $(SRCDIR)/%.cc,$(IRDIR)/%.opt.bc,$(SRCS))
OBJS      := $(patsubst $(SRCDIR)/%.cc,$(BUILDDIR)/%.o,$(SRCS))

########################################################################
#  Toolchain & flags
########################################################################
MPI_CXX   := /home/and21829/mpich_clang_ucx_install/bin/mpicxx
CXX       := $(MPI_CXX)# use the MPI wrapper everywhere
LD        := $(MPI_CXX)
OPT       := opt# LLVM optimizer

# common optimisation + OpenMP + MPI macro
#COMMONFLAGS := -O3 -march=native -DUSE_MPI
CXXFLAGS_IR  := -emit-llvm -c
CXXFLAGS_OBJ := -c
LDFLAGS      := -fopenmp

########################################################################
#  LLVM pass plug-in  (*** EDIT THESE TWO ***)
########################################################################
PASS_SO   ?= /home/and21829/llvm/memtrace-llvmpass/MemoryTrace.so# shared library containing your pass
PASS_NAME ?= memory-trace# the pipeline name you registered
OPTPASSFLAGS := -load-pass-plugin=$(PASS_SO) -passes=$(PASS_NAME)

########################################################################
#  Default target
########################################################################
all : $(BINARY)

########################################################################
#  Phase 1 : C++  →  raw bit-code  (.bc)
########################################################################
$(IRDIR)/%.bc : $(SRCDIR)/%.cc
	@echo "[BC ]  $< → $@"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_IR) -o $@ $<

########################################################################
#  Phase 2 : run the custom opt pass  (.bc → .opt.bc)
########################################################################
$(IRDIR)/%.opt.bc : $(IRDIR)/%.bc
	@echo "[OPT]  $< → $@"
	@mkdir -p $(dir $@)
	$(OPT) $(OPTPASSFLAGS) -o $@ $<

########################################################################
#  Phase 3 : modified IR  →  object file  (.opt.bc → .o)
########################################################################
$(BUILDDIR)/%.o : $(IRDIR)/%.opt.bc
	@echo "[OBJ]  $< → $@"
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS_OBJ) -o $@ $<

########################################################################
#  Phase 4 : final link
########################################################################
$(BINARY) : $(OBJS)
	@echo "[LINK] → $@"
	@mkdir -p $(dir $@)
	$(LD) $(LDFLAGS) -o $@ $^

########################################################################
#  House-keeping
########################################################################
.PHONY : clean
clean :
	rm -rf $(BUILDDIR)
