#include "Driver.hh"

namespace pcc {
Driver::Driver(Context* context)
    : context_(context) {}

Driver::~Driver() {}

void Driver::Parse(const std::string& filename) {
    if (StartLexer(filename) != 0) {
        return;
    }
    Parser parser(*this);
    parser.parse();
    StopLexer();
}

llvm::Module* Driver::GetModule() { return &context_->module_; }

llvm::IRBuilder<>* Driver::GetBuilder() { return &context_->builder_; }
} // namespace pcc
