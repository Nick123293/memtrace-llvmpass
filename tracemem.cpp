#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
 
using namespace llvm;
 
namespace {
struct MemoryTrace : public llvm::PassInfoMixin<MemoryTrace> {
private:
  const std::string FprintfLoadFormatStringVarName = "_FprintfLoadFormatString";
  const std::string FprintfStoreFormatStringVarName =
      "_FprintfStoreFormatString";

  void AddGlobalExternalStringToModule(llvm::Module &M,
                                       const std::string &VariableName) {
    M.getOrInsertGlobal(
        VariableName, PointerType::getUnqual(Type::getInt8Ty(M.getContext())));

    GlobalVariable *namedGlobal = M.getNamedGlobal(VariableName);
    namedGlobal->setLinkage(GlobalValue::ExternalLinkage);
  }

  void InitializeGlobalFormatString(llvm::Module &M, const std::string &VarName,
                                    const std::string &StringVal) {
    auto &CTX = M.getContext();

    std::vector<llvm::Constant *> formatStrVector(StringVal.size());
    for (unsigned int i = 0; i < StringVal.size(); i++) {
      formatStrVector[i] =
          llvm::ConstantInt::get(Type::getInt8Ty(CTX), StringVal[i]);
    }

    formatStrVector.push_back(llvm::ConstantInt::get(Type::getInt8Ty(CTX), 0));
    auto StringType =
        llvm::ArrayType::get(Type::getInt8Ty(CTX), formatStrVector.size());

    Constant *Var = M.getOrInsertGlobal(VarName, StringType);

    dyn_cast<GlobalVariable>(Var)->setInitializer(
        llvm::ConstantArray::get(StringType, formatStrVector));

    GlobalVariable *namedGlobal = M.getNamedGlobal(VarName);
    namedGlobal->setLinkage(GlobalValue::ExternalLinkage);
  }

  void addMemoryTraceFPInitialization(llvm::Module &M,
                                      llvm::Function &MainFunc) {
    auto &CTX = M.getContext();

    std::vector<llvm::Type *> FopenArgs{
        PointerType::getUnqual(Type::getInt8Ty(CTX)),
        PointerType::getUnqual(Type::getInt8Ty(CTX))};

    FunctionType *FopenTy = FunctionType::get(
        PointerType::getUnqual(Type::getInt8Ty(CTX)), FopenArgs, false);

    FunctionCallee Fopen = M.getOrInsertFunction("fopen", FopenTy);

    Constant *FopenFileNameStr =
        llvm::ConstantDataArray::getString(CTX, "memory-traces.log");
    Constant *FopenFilenameStrVar =
        M.getOrInsertGlobal("FopenFileNameStr", FopenFileNameStr->getType());
    dyn_cast<GlobalVariable>(FopenFilenameStrVar)
        ->setInitializer(FopenFileNameStr);

    Constant *FopenModeStr = llvm::ConstantDataArray::getString(CTX, "w+");
    Constant *FopenModeStrVar =
        M.getOrInsertGlobal("FopenModeStr", FopenModeStr->getType());
    dyn_cast<GlobalVariable>(FopenModeStrVar)->setInitializer(FopenModeStr);

    IRBuilder<> Builder(&*MainFunc.getEntryBlock().getFirstInsertionPt());
    llvm::Value *FopenFilenameStrPtr = Builder.CreatePointerCast(
        FopenFilenameStrVar, FopenArgs[0], "fileNameStr");
    llvm::Value *FopenModeStrPtr =
        Builder.CreatePointerCast(FopenModeStrVar, FopenArgs[0], "modeStr");
    llvm::Value *FopenReturn =
        Builder.CreateCall(Fopen, {FopenFilenameStrPtr, FopenModeStrPtr});

    GlobalVariable *FPGlobal = M.getNamedGlobal(FilePointerVarName);
    Builder.CreateStore(FopenReturn, FPGlobal);
  }

  FunctionCallee AddFprintfToModule(llvm::Module &M) {
    auto &CTX = M.getContext();

    std::vector<llvm::Type *> FprintfArgs{
        PointerType::getUnqual(Type::getInt8Ty(CTX)),
        PointerType::getUnqual(Type::getInt8Ty(CTX))};

    FunctionType *FprintfTy =
        FunctionType::get(Type::getInt32Ty(CTX), FprintfArgs, true);

    return M.getOrInsertFunction("fprintf", FprintfTy);
  }

public:
  const std::string FilePointerVarName = "_MemoryTraceFP";


	void addGlobalMemoryTraceFP(llvm::Module &M)
	{
		auto &CTX = M.getContext();
		// the type of FILE* (we're using i8* for simplicity)
		PointerType *PtrTy = PointerType::getUnqual(Type::getInt8Ty(CTX));

		// This will create the global if it doesn't exist, or return the existing one
		Constant *maybeGV = M.getOrInsertGlobal(FilePointerVarName, PtrTy);
		auto *GV = dyn_cast<GlobalVariable>(maybeGV);
		assert(GV && "getOrInsertGlobal didn't give us a GlobalVariable!");

		// Make it externally‐linkable
		GV->setLinkage(GlobalValue::ExternalLinkage);
		// **This line actually *defines* it** as a null pointer:
		GV->setInitializer(ConstantPointerNull::get(PtrTy));
	}

  const std::string TraceMemoryFunctionName = "_TraceMemory";

  void addMemoryTraceToBB(IRBuilder<> &Builder, llvm::Function &Function,
                          llvm::Module &M, bool IsLoad) {
    auto &CTX = M.getContext();

    std::vector<llvm::Type *> FprintfArgs{
        PointerType::getUnqual(Type::getInt8Ty(CTX)),
        PointerType::getUnqual(Type::getInt8Ty(CTX))};

    FunctionType *FprintfTy =
        FunctionType::get(Type::getInt32Ty(CTX), FprintfArgs, true);

    FunctionCallee Fprintf = M.getOrInsertFunction("fprintf", FprintfTy);

    Constant *TraceLoadStr = llvm::ConstantDataArray::getString(
        CTX, "[Read] Read value 0x%lx from address %p\n");
    Constant *TraceLoadStrVar =
        M.getOrInsertGlobal("TraceLoadStr", TraceLoadStr->getType());
    dyn_cast<GlobalVariable>(TraceLoadStrVar)->setInitializer(TraceLoadStr);

    Constant *TraceStoreStr = llvm::ConstantDataArray::getString(
        CTX, "[Write] Wrote value 0x%lx to address %p\n");
    Constant *TraceStoreStrVar =
        M.getOrInsertGlobal("TraceStoreStr", TraceStoreStr->getType());
    dyn_cast<GlobalVariable>(TraceStoreStrVar)->setInitializer(TraceStoreStr);

    llvm::Value *StrPtr;
    if (IsLoad)
      StrPtr = Builder.CreatePointerCast(TraceLoadStrVar, FprintfArgs[1],
                                         "loadStrPtr");
    else
      StrPtr = Builder.CreatePointerCast(TraceStoreStrVar, FprintfArgs[1],
                                         "storeStrPtr");

    GlobalVariable *FPGlobal = M.getNamedGlobal(FilePointerVarName);
    llvm::LoadInst *FP = Builder.CreateLoad(
        PointerType::getUnqual(Type::getInt8Ty(CTX)), FPGlobal);
    Builder.CreateCall(Fprintf,
                       {FP, StrPtr, Function.getArg(1), Function.getArg(0)});
  }

  void addTraceMemoryFunction(llvm::Module &M) {
    auto &CTX = M.getContext();

    std::vector<llvm::Type *> TraceMemoryArgs{
        PointerType::getUnqual(Type::getInt8Ty(CTX)), Type::getInt64Ty(CTX),
        Type::getInt32Ty(CTX)};

    FunctionType *TraceMemoryTy =
        FunctionType::get(Type::getVoidTy(CTX), TraceMemoryArgs, false);

    FunctionCallee TraceMemory =
        M.getOrInsertFunction(TraceMemoryFunctionName, TraceMemoryTy);

    llvm::Function *TraceMemoryFunction =
        dyn_cast<llvm::Function>(TraceMemory.getCallee());
    TraceMemoryFunction->setLinkage(GlobalValue::ExternalLinkage);

    llvm::BasicBlock *BB =
        llvm::BasicBlock::Create(CTX, "entry", TraceMemoryFunction);
    IRBuilder<> Builder(BB);

    llvm::Value *CmpResult = Builder.CreateICmpNE(
        TraceMemoryFunction->getArg(2), Builder.getInt32(0));

    llvm::BasicBlock *TraceLoadBB =
        llvm::BasicBlock::Create(CTX, "traceLoad", TraceMemoryFunction);

    llvm::BasicBlock *TraceStoreBB =
        llvm::BasicBlock::Create(CTX, "traceStore", TraceMemoryFunction);
    Builder.CreateCondBr(CmpResult, TraceLoadBB, TraceStoreBB);

    Builder.SetInsertPoint(TraceLoadBB);
    addMemoryTraceToBB(Builder, *TraceMemoryFunction, M, /*IsLoad=*/true);
    Builder.CreateRetVoid();

    Builder.SetInsertPoint(TraceStoreBB);
    addMemoryTraceToBB(Builder, *TraceMemoryFunction, M, /*IsLoad=*/false);
    Builder.CreateRetVoid();
  }

  void addMemoryTraceToInstruction(llvm::Module &M,
                                   llvm::Instruction &Instruction) {
    auto &CTX = M.getContext();

    std::vector<llvm::Type *> TraceMemoryArgs{
        PointerType::getUnqual(Type::getInt8Ty(CTX)), Type::getInt64Ty(CTX),
        Type::getInt32Ty(CTX)};

    FunctionType *TraceMemoryTy =
        FunctionType::get(Type::getVoidTy(CTX), TraceMemoryArgs, false);

    FunctionCallee TraceMemory =
        M.getOrInsertFunction(TraceMemoryFunctionName, TraceMemoryTy);

    IRBuilder<> Builder(Instruction.getNextNode());
    llvm::LoadInst *LoadInst = dyn_cast<llvm::LoadInst>(&Instruction);
    llvm::StoreInst *StoreInst = dyn_cast<llvm::StoreInst>(&Instruction);

    llvm::Value *MemoryAddress;
    if (LoadInst)
      MemoryAddress = Builder.CreatePointerCast(
          LoadInst->getPointerOperand(), TraceMemoryArgs[0], "memoryAddress");
    else
      MemoryAddress = Builder.CreatePointerCast(
          StoreInst->getPointerOperand(), TraceMemoryArgs[0], "memoryAddress");

    llvm::Value *CastTo64;
    llvm::Value *ValueToPrint =
        LoadInst ? &Instruction : StoreInst->getOperand(0);
    bool ShouldConvertPointer = ValueToPrint->getType()->isPointerTy();

    if (ShouldConvertPointer)
      CastTo64 =
          Builder.CreatePtrToInt(ValueToPrint, TraceMemoryArgs[1], "castTo64");
    else
      CastTo64 = Builder.CreateIntCast(ValueToPrint, TraceMemoryArgs[1], false,
                                       "castTo64");

    Builder.CreateCall(
        TraceMemory,
        {MemoryAddress, CastTo64,
         Builder.getInt32(Instruction.getOpcode() == llvm::Instruction::Load)});
  }

  llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &) {
    Function *main = M.getFunction("main");
    if (main) {
      addGlobalMemoryTraceFP(M);
      addMemoryTraceFPInitialization(M, *main);
      addTraceMemoryFunction(M);
      errs() << "Found main in module " << M.getName() << "\n";
    } else {
      errs() << "Did not find main in " << M.getName() << "\n";
    }

    for (llvm::Function &F : M) {
      if (F.getName() == TraceMemoryFunctionName) {
        continue;
      }

      for (llvm::BasicBlock &BB : F) {
        for (llvm::Instruction &Instruction : BB) {
          if (Instruction.getOpcode() == llvm::Instruction::Load ||
              Instruction.getOpcode() == llvm::Instruction::Store) {
            addMemoryTraceToInstruction(M, Instruction);
          }
        }
      }
    }

    return llvm::PreservedAnalyses::none();
  }

  bool runOnModule(llvm::Module &M) { return true; }
};
} // namespace

llvm::PassPluginLibraryInfo getMemoryTracePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "MemoryTrace", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "memory-trace") {
                    MPM.addPass(MemoryTrace());
                    return true;
                  }

                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMemoryTracePluginInfo();
}