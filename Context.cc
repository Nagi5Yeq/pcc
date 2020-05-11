#include <llvm/IR/LLVMContext.h>

#include "Context.hh"

namespace pcc {
llvm::LLVMContext GlobalContext;

Context::Context(const char* name)
    : builder_(GlobalContext)
    , module_(name, GlobalContext) {}
} // namespace pcc
