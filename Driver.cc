#include "Driver.hh"
#include "Parser.hh"

namespace pcc {
Driver::Driver(const char* name)
    : context_(name) {}

Driver::~Driver() {}

std::shared_ptr<ProgramNode> Driver::Parse(std::string& filename) {
    location_.initialize(&filename);
    if (StartLexer(filename) != 0) {
        return nullptr;
    }
    Parser parser(*this, &context_);
    parser.parse();
    StopLexer();
    return root_;
}

void Driver::SetRoot(std::shared_ptr<ProgramNode> root) { root_ = root; }

Context* Driver::GetContext() { return &context_; }
} // namespace pcc
