#include <string>
#include <utility>

#include "Log.hh"

namespace pcc {
static LogLevel CurrentLogLevel = PCC_ERROR;

void SetLogLevel(LogLevel level) { CurrentLogLevel = level; }
LogLevel GetLogLevel() { return CurrentLogLevel; }

static std::string Executable("pcc");

void SetExecutableName(const char* name) { Executable = name; }
const char* GetExecutableName() { return Executable.c_str(); }
} // namespace pcc
