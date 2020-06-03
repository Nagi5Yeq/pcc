#include <iostream>
#include <string>

#include "Color.hh"
#include "Log.hh"

namespace pcc {
void Log(LogLevel level, const char* format, ...) {
    char buf[1024];
    va_list va;
    va_start(va, format);
    if (GetLogLevel() <= level) {
        std::cerr << GetExecutableName() << ": ";
        switch (level) {
        case PCC_ERROR:
            std::cerr << Color::RED << "error: ";
            break;
        case PCC_WARNING:
            std::cerr << Color::MAGENTA << "warning: ";
            break;
        case PCC_INFO:
            std::cerr << Color::CYAN << "info: ";
            break;
        case PCC_DEBUG:
            std::cerr << Color::GREEN << "remark: ";
            break;
        }
        std::vsnprintf(buf, sizeof(buf), format, va);
        std::cerr << Color::WHITE << buf << Color::DEFAULT << std::endl;
    }
    va_end(va);
}

static LogLevel CurrentLogLevel = PCC_ERROR;

void SetLogLevel(LogLevel level) { CurrentLogLevel = level; }
LogLevel GetLogLevel() { return CurrentLogLevel; }

static std::string Executable("pcc");

void SetExecutableName(const char* name) { Executable = name; }
const char* GetExecutableName() { return Executable.c_str(); }
} // namespace pcc
