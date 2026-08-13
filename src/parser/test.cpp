#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

int main()
{
    llvm::LLVMContext context;
    llvm::Module module("KarettaModule", context);
    llvm::IRBuilder<> builder(context);

    // printf: int printf(const char*, ...)
    llvm::FunctionType* printfType =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context),
            {llvm::PointerType::getUnqual(context)},
            true
        );

    llvm::Function* printfFunc =
        llvm::Function::Create(
            printfType,
            llvm::Function::ExternalLinkage,
            "printf",
            module
        );

    // int main()
    llvm::FunctionType* mainType =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context),
            false
        );

    llvm::Function* mainFunc =
        llvm::Function::Create(
            mainType,
            llvm::Function::ExternalLinkage,
            "main",
            module
        );

    // entry:
    llvm::BasicBlock* entry =
        llvm::BasicBlock::Create(
            context,
            "entry",
            mainFunc
        );

    builder.SetInsertPoint(entry);

    // "Hello, world!\n"
    llvm::Value* hello =
        builder.CreateGlobalStringPtr(
            "Hello, world!\n",
            "hello"
        );

    // printf(hello)
    builder.CreateCall(
        printfFunc,
        {hello}
    );

    // return 0
    builder.CreateRet(
        llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context),
            0
        )
    );

    // Check that we didn't produce invalid IR.
    if (llvm::verifyModule(module, &llvm::errs()))
        return 1;

    module.print(llvm::outs(), nullptr);
}