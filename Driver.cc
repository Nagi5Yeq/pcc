#include "Driver.hh"
#include "Parser.hh"

namespace pcc {
Driver::Driver(const char* name)
    : context_(name) {}

Driver::~Driver() {}

void Driver::Parse(std::string& filename) {
    location_.initialize(&filename);
    if (StartLexer(filename) != 0) {
        return;
    }
    Parser parser(*this, &context_);
    parser.parse();
    StopLexer();
}
} // namespace pcc
