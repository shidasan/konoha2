/****************************************************************************
 * KONOHA COPYRIGHT, LICENSE NOTICE, AND DISCRIMER
 *
 * Copyright (c) 2011 Masahiro Ide <imasahiro9 at gmail.com>
 * All rights reserved.
 * You may choose one of the following two licenses when you use konoha.
 * If you want to use the latter license, please contact us.
 *
 * (1) GNU General Public License 3.0 (with K_UNDER_GPL)
 * (2) Konoha Non-Disclosure License 1.0
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

include "lib:gluelink";

@RawPtr class Type;
@RawPtr class IntegerType extends Type;
@RawPtr class PointerType extends Type;
@RawPtr class FunctionType extends Type;
@RawPtr class StructType extends Type;
@RawPtr class ArrayType extends Type;
@RawPtr class Value;
@RawPtr class Argument extends Value;
@RawPtr class Constant extends Value;
@RawPtr class ConstantInt extends Constant;
@RawPtr class ConstantFP  extends Constant;
@RawPtr class ConstantPointerNull extends Constant;
@RawPtr class ConstantStruct extends Constant;
@RawPtr class GlobalVariable extends Value;
@RawPtr class IRBuilder;
@RawPtr class Instruction extends Value;
@RawPtr class AllocaInst extends Instruction;
@RawPtr class ReturnInst extends Instruction;
@RawPtr class LoadInst   extends Instruction;
@RawPtr class StoreInst  extends Instruction;
@RawPtr class BranchInst extends Instruction;
@RawPtr class CallInst   extends Instruction;
@RawPtr class GetElementPtrInst extends Instruction;
@Rawptr @Native class BasicBlock;
@RawPtr class Module;
@RawPtr class Function extends Value;
@RawPtr class PHINode extends Value;
@RawPtr class ExecutionEngine;
@RawPtr class NativeFunction;

@Native class GlobalValue;
@Native class Intrinsic;
@Native class Attributes;

@RawPtr class LLVM;
@RawPtr class Pass;
@RawPtr class ModulePass extends Pass;
@RawPtr class FunctionPass extends Pass;
@RawPtr class LoopPass extends Pass;
@RawPtr class LibCallInfo;
@Rawptr class ImmutablePass extends Pass;
@Rawptr class TargetData extends ImmutablePass;
@Rawptr class PassManager;
@Rawptr class FunctionPassManager;
@Rawptr class PassManagerBuilder;

@Native @Static Type Type.getVoidTy();
@Native @Static Type Type.getLabelTy();
@Native @Static Type Type.getFloatTy();
@Native @Static Type Type.getDoubleTy();
@Native @Static Type Type.getMetadataTy();
@Native @Static Type Type.getX86_FP80Ty();
@Native @Static Type Type.getFP128Ty();
@Native @Static Type Type.getPPC_FP128Ty();
@Native @Static Type Type.getX86_MMXTy();
@Native @Static IntegerType Type.getInt1Ty();
@Native @Static IntegerType Type.getInt8Ty();
@Native @Static IntegerType Type.getInt16Ty();
@Native @Static IntegerType Type.getInt32Ty();
@Native @Static IntegerType Type.getInt64Ty();
@Native @Static PointerType PointerType.get(Type type);
@Native @Static PointerType Type.getFloatPtrTy();
@Native @Static PointerType Type.getDoublePtrTy();
@Native @Static PointerType Type.getX86_FP80PtrTy();
@Native @Static PointerType Type.getFP128PtrTy();
@Native @Static PointerType Type.getPPC_FP128PtrTy();
@Native @Static PointerType Type.getX86_MMXPtrTy();
@Native @Static PointerType Type.getInt1PtrTy();
@Native @Static PointerType Type.getInt8PtrTy();
@Native @Static PointerType Type.getInt16PtrTy();
@Native @Static PointerType Type.getInt32PtrTy();
@Native @Static PointerType Type.getInt64PtrTy();

@Native IRBuilder IRBuilder.new(BasicBlock bb);
@Native ReturnInst IRBuilder.createRetVoid();
@Native ReturnInst IRBuilder.createRet(Value v);
//@Native ReturnInst IRBuilder.createAggregateRet(Value retVals, int n);
@Native BranchInst IRBuilder.createBr(BasicBlock dest);
@Native BranchInst IRBuilder.createCondBr(Value cond, BasicBlock trueBB, BasicBlock falseBB);
@Native SwitchInst IRBuilder.createSwitch(Value v, BasicBlock dest);
@Native IndirectBrInst IRBuilder.createIndirectBr(Value addr);
@Native InvokeInst IRBuilder.createInvoke0(Value callee, BasicBlock normalDest, BasicBlock unwindDest);
@Native InvokeInst IRBuilder.createInvoke1(Value callee, BasicBlock normalDest, BasicBlock unwindDest, Value arg1);
@Native InvokeInst IRBuilder.createInvoke3(Value callee, BasicBlock normalDest, BasicBlock unwindDest, Value arg1, Value arg2, Value arg3);
//@Native InvokeInst IRBuilder.createInvoke(Value callee, BasicBlock normalDest, BasicBlock unwindDest, Array<Value> args);
//@Native ResumeInst IRBuilder.createResume(Value exn);
@Native UnreachableInst IRBuilder.createUnreachable();
@Native Value IRBuilder.createAdd(Value lhs, Value rhs);
@Native Value IRBuilder.createNSWAdd(Value lhs, Value rhs);
@Native Value IRBuilder.createNUWAdd(Value lhs, Value rhs);
@Native Value IRBuilder.createFAdd(Value lhs, Value rhs);
@Native Value IRBuilder.createSub(Value lhs, Value rhs);
@Native Value IRBuilder.createNSWSub(Value lhs, Value rhs);
@Native Value IRBuilder.createNUWSub(Value lhs, Value rhs);
@Native Value IRBuilder.createFSub(Value lhs, Value rhs);
@Native Value IRBuilder.createMul(Value lhs, Value rhs);
@Native Value IRBuilder.createNSWMul(Value lhs, Value rhs);
@Native Value IRBuilder.createNUWMul(Value lhs, Value rhs);
@Native Value IRBuilder.createFMul(Value lhs, Value rhs);
@Native Value IRBuilder.createUDiv(Value lhs, Value rhs);
@Native Value IRBuilder.createExactUDiv(Value lhs, Value rhs);
@Native Value IRBuilder.createSDiv(Value lhs, Value rhs);
@Native Value IRBuilder.createExactSDiv(Value lhs, Value rhs);
@Native Value IRBuilder.createFDiv(Value lhs, Value rhs);
@Native Value IRBuilder.createURem(Value lhs, Value rhs);
@Native Value IRBuilder.createSRem(Value lhs, Value rhs);
@Native Value IRBuilder.createFRem(Value lhs, Value rhs);
@Native Value IRBuilder.createShl(Value lhs, Value rhs);
@Native Value IRBuilder.createLShr(Value lhs, Value rhs);
@Native Value IRBuilder.createAShr(Value lhs, Value rhs);
@Native Value IRBuilder.createAnd(Value lhs, Value rhs);
@Native Value IRBuilder.createOr(Value lhs, Value rhs);
@Native Value IRBuilder.createXor(Value lhs, Value rhs);
@Native Value IRBuilder.createNeg(Value v);
@Native Value IRBuilder.createNSWNeg(Value v);
@Native Value IRBuilder.createNUWNeg(Value v);
@Native Value IRBuilder.createFNeg(Value v);
@Native Value IRBuilder.createNot(Value v);
@Native AllocaInst IRBuilder.createAlloca(Type ty, Value arraySize);
@Native AllocaInst AllocaInst.new(Type ty, Value arraySize);
@Native LoadInst IRBuilder.createLoad(Value ptr, boolean isVolatile);
@Native LoadInst LoadInst.new(Value ptr);
@Native StoreInst IRBuilder.createStore(Value val, Value ptr, boolean isVolatile);
@Native StoreInst StoreInst.new(Value val, Value ptr);
//@Native FenceInst IRBuilder.createFence(AtomicOrdering ordering, SynchronizationScope SynchScope);
//@Native AtomicCmpXchgInst IRBuilder.createAtomicCmpXchg(Value ptr, Value Cmp, Value New, AtomicOrdering ordering, SynchronizationScope SynchScope);
@Native @Static GetElementPtrInst GetElementPtrInst.create(Value ptr, Array<Value> idxList);
@Native @Static GetElementPtrInst GetElementPtrInst.createInBounds(Value ptr, Array<Value> idxList);
@Native Value IRBuilder.createGEP(Value ptr, Array<Value> idxList);
@Native Value IRBuilder.createInBoundsGEP(Value ptr, Array<Value> idxList);
@Native Value IRBuilder.createGEP1(Value ptr, Value idx);
@Native Value IRBuilder.createInBoundsGEP1(Value ptr, Value idx);
@Native Value IRBuilder.createConstGEP1_32(Value ptr, int idx0);
@Native Value IRBuilder.createConstInBoundsGEP1_32(Value ptr, int idx0);
@Native Value IRBuilder.createConstGEP2_32(Value ptr, int idx0, int idx1);
@Native Value IRBuilder.createConstInBoundsGEP2_32(Value ptr, int idx0, int idx1);
@Native Value IRBuilder.createConstGEP1_64(Value ptr, int idx0);
@Native Value IRBuilder.createConstInBoundsGEP1_64(Value ptr, int idx0);
@Native Value IRBuilder.createConstGEP2_64(Value ptr, int idx0, int idx1);
@Native Value IRBuilder.createConstInBoundsGEP2_64(Value ptr, int idx0, int idx1);
@Native Value IRBuilder.createStructGEP(Value ptr, int idx);
@Native Value IRBuilder.createGlobalString(String str);
@Native Value IRBuilder.createGlobalStringPtr(String str);
@Native Value IRBuilder.createTrunc(Value v, Type destTy);
@Native Value IRBuilder.createZExt(Value v, Type destTy);
@Native Value IRBuilder.createSExt(Value v, Type destTy);
@Native Value IRBuilder.createFPToUI(Value v, Type destTy);
@Native Value IRBuilder.createFPToSI(Value v, Type destTy);
@Native Value IRBuilder.createUIToFP(Value v, Type destTy);
@Native Value IRBuilder.createSIToFP(Value v, Type destTy);
@Native Value IRBuilder.createFPTrunc(Value v, Type destTy);
@Native Value IRBuilder.createFPExt(Value v, Type destTy);
@Native Value IRBuilder.createPtrToInt(Value v, Type destTy);
@Native Value IRBuilder.createIntToPtr(Value v, Type destTy);
@Native Value IRBuilder.createBitCast(Value v, Type destTy);
@Native Value IRBuilder.createZExtOrBitCast(Value v, Type destTy);
@Native Value IRBuilder.createSExtOrBitCast(Value v, Type destTy);
@Native Value IRBuilder.createTruncOrBitCast(Value v, Type destTy);
@Native Value IRBuilder.createPointerCast(Value v, Type destTy);
@Native Value IRBuilder.createIntCast(Value v, Type destTy, boolean isSigned);
@Native Value IRBuilder.createFPCast(Value v, Type destTy);
@Native Value IRBuilder.createICmpEQ(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpNE(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpUGT(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpUGE(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpULT(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpULE(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpSGT(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpSGE(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpSLT(Value lhs, Value rhs);
@Native Value IRBuilder.createICmpSLE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpOEQ(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpOGT(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpOGE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpOLT(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpOLE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpONE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpORD(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpUNO(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpUEQ(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpUGT(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpUGE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpULT(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpULE(Value lhs, Value rhs);
@Native Value IRBuilder.createFCmpUNE(Value lhs, Value rhs);
@Native PHINode IRBuilder.createPHI(Type ty, int numReservedValues);
@Native void PHINode.addIncoming(Value v, BasicBlock bb);
@Native CallInst IRBuilder.createCall1(Value callee, Value arg);
@Native CallInst IRBuilder.createCall2(Value callee, Value arg1, Value arg2);
@Native CallInst IRBuilder.createCall3(Value callee, Value arg1, Value arg2, Value arg3);
@Native CallInst IRBuilder.createCall4(Value callee, Value arg1, Value arg2, Value arg3, Value arg4);
@Native CallInst IRBuilder.createCall5(Value callee, Value arg1, Value arg2, Value arg3, Value arg4, Value arg5);
@Native CallInst IRBuilder.createCall(Value callee, Array<Value> args);
@Native Value IRBuilder.createSelect(Value c, Value trueV, Value falseV);
@Native VAArgInst IRBuilder.createVAArg(Value list, Type ty);
@Native Value IRBuilder.createExtractElement(Value vec, Value idx);
@Native Value IRBuilder.createInsertElement(Value vec, Value newElt, Value idx);
@Native Value IRBuilder.createShuffleVector(Value v1, Value v2, Value mask);
//@Native Value IRBuilder.createExtractValue(Value Agg, Array<int> Idxs);
//@Native Value IRBuilder.createInsertValue(Value Agg, Value val, Array<int> Idxs);
@Native Value IRBuilder.createIsNull(Value arg);
@Native Value IRBuilder.createIsNotNull(Value arg);
@Native Value IRBuilder.createPtrDiff(Value lhs, Value rhs);

@Native void IRBuilder.setInsertPoint(BasicBlock bb);
@Native BasicBlock IRBuilder.getInsertBlock();
@Native Function BasicBlock.getParent();
@Native Instruction BasicBlock.insertBefore(Instruction before, Instruction inst);
@Native Instruction BasicBlock.getLastInst();
@Native Instruction BasicBlock.getTerminator();
@Native void Instruction.setMetadata(Module m, String name, int value);
@Native void Function.dump();
@Native void Value.dump();
@Native void Type.dump();
@Native void BasicBlock.dump();
@Native @Static Function Function.create(String name, FunctionType fnTy, Module m, int linkage);
@Native void Function.addFnAttr(Int attributes);

//@Native iterator BasicBlock.begin();
//@Native iterator BasicBlock.end();
@Native int BasicBlock.size();
@Native boolean BasicBlock.empty();
@Native Module Module.new(String name);
@Native Type Module.getTypeByName(String name);

@Native void Module.dump();
@Native Function Module.getOrInsertFunction(String name, FunctionType fnTy);
@Native ExecutionEngine Module.createExecutionEngine(int optLevel);
@Native @Static BasicBlock BasicBlock.create(Function parent, String name);
@Native @Static FunctionType FunctionType.get(Type retTy, Array<Type> args, boolean b);
@Native @Static ArrayType ArrayType.get(Type t, int elemSize);
@Native @Static StructType StructType.get(Array<Type> args, boolean isPacked);
@Native @Static StructType StructType.create(Array<Type> args, String name, boolean isPacked);
@Native void StructType.setBody(Array<Type> args, boolean isPacked);
@Native boolean StructType.isOpaque();
@Native NativeFunction ExecutionEngine.getPointerToFunction(Function func);
@Native void ExecutionEngine.addGlobalMapping(GlobalVariable g, int addr);
@Native GlobalVariable GlobalVariable.new(Module m, Type ty, Constant c, int linkage, String name);

@Native PassManagerBuilder PassManagerBuilder.new();
@Native void PassManagerBuilder.populateModulePassManager(PassManager manager);
@Virual @Native void PassManager.run(Module m);
@Native PassManager PassManager.new();
@Native FunctionPassManager FunctionPassManager.new(Module m);
@Native void PassManager.addPass(Pass p);
@Native void PassManager.addImmutablePass(ImmutablePass p);
@Native void PassManager.addFunctionPass(FunctionPass p);
@Native void PassManager.addModulePass(ModulePass p);
@Overload void PassManager.add(ImmutablePass p) { addImmutablePass(p); }
@Overload void PassManager.add(FunctionPass p) { addFunctionPass(p); }
@Overload void PassManager.add(ModulePass p) { addModulePass(p); }
@Overload void PassManager.add(Pass p) { addPass(p); }

@Native void FunctionPassManager.add(Pass p);
@Native void FunctionPassManager.run(Function func);
@Native void FunctionPassManager.doInitialization();
@Native TargetData ExecutionEngine.getTargetData();

@Native Argument Argument.new(Type type);
@Native void Value.replaceAllUsesWith(Value v);
@Native Value Value.setName(String name);
@Native Type Value.getType();
@Native Array<Value> Function.getArguments();

@Native void LoadInst.setAlignment(Int align);
@Native void StoreInst.setAlignment(Int align);

@Native @Public void Method.setFunction(NativeFunction nf);

@Static @Native Constant ConstantInt.get(Type type, int v);
@Static @Native Constant ConstantFP.get(Type type, float v);
@Static @Native Constant ConstantPointerNull.get(Type type);
@Static @Native Constant ConstantStruct.get(Type type, Array<Constant> v);

@RawPtr class DynamicLibrary;
@Native @Static boolean DynamicLibrary.loadLibraryPermanently(String libname);
@Native @Static Int DynamicLibrary.searchForAddressOfSymbol(String fname);

@Static @Native FunctionPass LLVM.createDomPrinterPass();
@Static @Native FunctionPass LLVM.createDomOnlyPrinterPass();
@Static @Native FunctionPass LLVM.createDomViewerPass();
@Static @Native FunctionPass LLVM.createDomOnlyViewerPass();
@Static @Native FunctionPass LLVM.createPostDomPrinterPass();
@Static @Native FunctionPass LLVM.createPostDomOnlyPrinterPass();
@Static @Native FunctionPass LLVM.createPostDomViewerPass();
@Static @Native FunctionPass LLVM.createPostDomOnlyViewerPass();
@Static @Native Pass LLVM.createGlobalsModRefPass();
@Static @Native Pass LLVM.createAliasDebugger();
@Static @Native ModulePass LLVM.createAliasAnalysisCounterPass();
@Static @Native FunctionPass LLVM.createAAEvalPass();
@Static @Native FunctionPass LLVM.createLibCallAliasAnalysisPass(LibCallInfo lci);
@Static @Native FunctionPass LLVM.createScalarEvolutionAliasAnalysisPass();
@Static @Native ModulePass LLVM.createProfileLoaderPass();
@Static @Native FunctionPass LLVM.createProfileEstimatorPass();
@Static @Native FunctionPass LLVM.createProfileVerifierPass();
@Static @Native ModulePass LLVM.createPathProfileLoaderPass();
@Static @Native ModulePass LLVM.createPathProfileVerifierPass();
@Static @Native FunctionPass LLVM.createLazyValueInfoPass();
@Static @Native LoopPass LLVM.createLoopDependenceAnalysisPass();
@Static @Native FunctionPass LLVM.createInstCountPass();
@Static @Native FunctionPass LLVM.createDbgInfoPrinterPass();
@Static @Native FunctionPass LLVM.createRegionInfoPass();
@Static @Native ModulePass LLVM.createModuleDebugInfoPrinterPass();
@Static @Native FunctionPass LLVM.createMemDepPrinter();
@Static @Native FunctionPass LLVM.createPostDomTree();
@Static @Native FunctionPass LLVM.createRegionViewerPass();
@Static @Native FunctionPass LLVM.createRegionOnlyViewerPass();
@Static @Native FunctionPass LLVM.createRegionPrinterPass();
@Static @Native FunctionPass LLVM.createRegionOnlyPrinterPass();
@Static @Native FunctionPass LLVM.createLintPass();
//@Static @Native ModulePass LLVM.createPrintModulePass(raw_ostream *OS);
//@Static @Native FunctionPass LLVM.createPrintFunctionPass(String banner, OutputStream os, boolean deleteStream);
//@Static @Native ModulePass LLVM.createEdgeProfilerPass();
//@Static @Native ModulePass LLVM.createOptimalEdgeProfilerPass();
//@Static @Native ModulePass LLVM.createPathProfilerPass();
//@Static @Native ModulePass LLVM.createGCOVProfilerPass(boolean emitNotes, boolean emitData, boolean use402Format);
@Static @Native ModulePass LLVM.createStripSymbolsPass(boolean onlyDebugInfo);
@Static @Native ModulePass LLVM.createStripNonDebugSymbolsPass();
@Static @Native ModulePass LLVM.createStripDeadDebugInfoPass();
@Static @Native ModulePass LLVM.createConstantMergePass();
@Static @Native ModulePass LLVM.createGlobalOptimizerPass();
@Static @Native ModulePass LLVM.createGlobalDCEPass();
@Static @Native Pass LLVM.createFunctionInliningPass(int threshold);
@Static @Native Pass LLVM.createAlwaysInlinerPass();
@Static @Native Pass LLVM.createPruneEHPass();
@Static @Native ModulePass LLVM.createInternalizePass(boolean allButMain);
@Static @Native ModulePass LLVM.createDeadArgEliminationPass();
@Static @Native Pass LLVM.createArgumentPromotionPass(int maxElements);
@Static @Native ModulePass LLVM.createIPConstantPropagationPass();
@Static @Native ModulePass LLVM.createIPSCCPPass();
@Static @Native Pass LLVM.createLoopExtractorPass();
@Static @Native Pass LLVM.createSingleLoopExtractorPass();
@Static @Native ModulePass LLVM.createBlockExtractorPass();
@Static @Native ModulePass LLVM.createStripDeadPrototypesPass();
@Static @Native Pass LLVM.createFunctionAttrsPass();
@Static @Native ModulePass LLVM.createMergeFunctionsPass();
@Static @Native ModulePass LLVM.createPartialInliningPass();
@Static @Native FunctionPass LLVM.createConstantPropagationPass();
@Static @Native FunctionPass LLVM.createSCCPPass();
@Static @Native Pass LLVM.createDeadInstEliminationPass();
@Static @Native FunctionPass LLVM.createDeadCodeEliminationPass();
@Static @Native FunctionPass LLVM.createDeadStoreEliminationPass();
@Static @Native FunctionPass LLVM.createAggressiveDCEPass();
@Static @Native FunctionPass LLVM.createScalarReplAggregatesPass(int threshold);
@Static @Native Pass LLVM.createIndVarSimplifyPass();
@Static @Native FunctionPass LLVM.createInstructionCombiningPass();
@Static @Native Pass LLVM.createLICMPass();
@Static @Native Pass LLVM.createLoopUnswitchPass(boolean optimizeForSize);
@Static @Native Pass LLVM.createLoopInstSimplifyPass();
@Static @Native Pass LLVM.createLoopUnrollPass(int threshold, int count, int allowPartial);
@Static @Native Pass LLVM.createLoopRotatePass();
@Static @Native Pass LLVM.createLoopIdiomPass();
@Static @Native FunctionPass LLVM.createPromoteMemoryToRegisterPass();
@Static @Native FunctionPass LLVM.createDemoteRegisterToMemoryPass();
@Static @Native FunctionPass LLVM.createReassociatePass();
@Static @Native FunctionPass LLVM.createJumpThreadingPass();
@Static @Native FunctionPass LLVM.createCFGSimplificationPass();
@Static @Native FunctionPass LLVM.createBreakCriticalEdgesPass();
@Static @Native Pass LLVM.createLoopSimplifyPass();
@Static @Native FunctionPass LLVM.createTailCallEliminationPass();
@Static @Native FunctionPass LLVM.createLowerSwitchPass();
@Static @Native FunctionPass LLVM.createBlockPlacementPass();
@Static @Native Pass LLVM.createLCSSAPass();
@Static @Native FunctionPass LLVM.createEarlyCSEPass();
@Static @Native FunctionPass LLVM.createGVNPass(boolean noLoads);
@Static @Native FunctionPass LLVM.createMemCpyOptPass();
@Static @Native Pass LLVM.createLoopDeletionPass();
@Static @Native FunctionPass LLVM.createSimplifyLibCallsPass();
@Static @Native FunctionPass LLVM.createInstructionNamerPass();
@Static @Native FunctionPass LLVM.createSinkingPass();
@Static @Native Pass LLVM.createLowerAtomicPass();
@Static @Native Pass LLVM.createCorrelatedValuePropagationPass();
@Static @Native Pass LLVM.createObjCARCExpandPass();
@Static @Native Pass LLVM.createObjCARCContractPass();
@Static @Native Pass LLVM.createObjCARCOptPass();
@Static @Native FunctionPass LLVM.createInstructionSimplifierPass();
@Static @Native FunctionPass LLVM.createLowerExpectIntrinsicPass();
@Static @Native Pass LLVM.createUnifyFunctionExitNodesPass();
@Static @Native ImmutablePass LLVM.createTypeBasedAliasAnalysisPass();
@Static @Native ImmutablePass LLVM.createBasicAliasAnalysisPass();
@Static @Native FunctionPass LLVM.createVerifierPass();

@Static @Native FunctionType Intrinsic.getType(int id, Type[] args);
@Static @Native Function     Intrinsic.getDeclaration(Module m, int id, Type[] args);
@Static @Native Module LLVM.parseBitcodeFile(String bcfile);
