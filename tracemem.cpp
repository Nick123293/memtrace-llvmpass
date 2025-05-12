#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
 
using namespace llvm;
 
namespace {
struct MemoryTrace : public llvm::PassInfoMixin<MemoryTrace> {
	llvm::PreservedAnalyses run(llvm::Module &M,
				llvm::ModuleAnalysisManager &) {
		errs() << "In here\n\n";
		return llvm::PreservedAnalyses::all();
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