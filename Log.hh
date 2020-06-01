#ifndef PCC_LOG_H
#define PCC_LOG_H

#include <cstdarg>
#include <cstdio>

#include <llvm/Support/FormatVariadic.h>
#include <llvm/Support/WithColor.h>

namespace pcc {
enum LogLevel { PCC_DEBUG, PCC_INFO, PCC_WARNING, PCC_ERROR };

void SetLogLevel(LogLevel level);
LogLevel GetLogLevel();

void SetExecutableName(const char* name);
const char* GetExecutableName();

template <typename... Args>
void Log(LogLevel level, const char* format, Args&&... args) {
    auto msg = llvm::formatv(format, std::forward<Args>(args)...);
    const char* name = GetExecutableName();
    switch (level) {
    case PCC_DEBUG:
        llvm::WithColor::remark(llvm::errs(), name) << msg << '\n';
        break;
    case PCC_INFO:
        llvm::WithColor::note(llvm::errs(), name) << msg << '\n';
        break;
    case PCC_WARNING:
        llvm::WithColor::warning(llvm::errs(), name) << msg << '\n';
        break;
    case PCC_ERROR:
        llvm::WithColor::error(llvm::errs(), name) << msg << '\n';
        break;
    default:
        break;
    }
}
} // namespace pcc

#endif
