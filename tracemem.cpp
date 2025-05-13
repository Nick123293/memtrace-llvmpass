#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
 
using namespace llvm;
 
namespace {
struct MemoryTrace : public llvm::PassInfoMixin<MemoryTrace> {
	llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &) 
	{
		Function *main = M.getFunction("main");
		if (main) {
			addGlobalMemoryTraceFP(M);
			addMemoryTraceFPInitialization(M, *main);
			errs() << "Found main in module " << M.getName() << "\n";
			return llvm::PreservedAnalyses::none();
		} else {
			errs() << "Did not find main in " << M.getName() << "\n";
			return llvm::PreservedAnalyses::all();
		}
	}
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

	void addMemoryTraceFPInitialization(llvm::Module& M, llvm::Function &MainFunc) {
		auto &CTX = M.getContext();
	 
		std::vector<llvm::Type*> FopenArgs{
										PointerType::getUnqual(Type::getInt8Ty(CTX)),
										PointerType::getUnqual(Type::getInt8Ty(CTX))
										};
	 
		FunctionType *FopenTy = FunctionType::get(PointerType::getUnqual(Type::getInt8Ty(CTX)),
										FopenArgs,
										false);
	 
		FunctionCallee Fopen = M.getOrInsertFunction("fopen", FopenTy);
	 
		Constant *FopenFileNameStr = llvm::ConstantDataArray::getString(CTX, "memory-traces.log");
		Constant *FopenFilenameStrVar = M.getOrInsertGlobal("FopenFileNameStr", FopenFileNameStr->getType());
		dyn_cast<GlobalVariable>(FopenFilenameStrVar)->setInitializer(FopenFileNameStr);
	 
		Constant *FopenModeStr = llvm::ConstantDataArray::getString(CTX, "w+");
		Constant *FopenModeStrVar = M.getOrInsertGlobal("FopenModeStr", FopenModeStr->getType());
		dyn_cast<GlobalVariable>(FopenModeStrVar)->setInitializer(FopenModeStr);
	 
		IRBuilder<> Builder(&*MainFunc.getEntryBlock().getFirstInsertionPt());
		llvm::Value *FopenFilenameStrPtr = Builder.CreatePointerCast(FopenFilenameStrVar, FopenArgs[0],
																		"fileNameStr");
		llvm::Value *FopenModeStrPtr = Builder.CreatePointerCast(FopenModeStrVar, FopenArgs[0],
																		"modeStr");
		llvm::Value *FopenReturn = Builder.CreateCall(Fopen, {FopenFilenameStrPtr, FopenModeStrPtr});
	 
		GlobalVariable *FPGlobal = M.getNamedGlobal(FilePointerVarName);
		Builder.CreateStore(FopenReturn, FPGlobal);
	}
 
	bool runOnModule(llvm::Module &M) {
		return true;
	}
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
				}
			);
		}};
}
 
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getMemoryTracePluginInfo();
}