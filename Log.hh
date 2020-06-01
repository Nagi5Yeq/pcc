#ifndef PCC_LOG_H
#define PCC_LOG_H

#include <cstdarg>
#include <cstdio>

namespace pcc {
enum LogLevel { PCC_DEBUG, PCC_INFO, PCC_WARNING, PCC_ERROR };

void SetLogLevel(LogLevel level);
LogLevel GetLogLevel();

void SetExecutableName(const char* name);
const char* GetExecutableName();

void Log(LogLevel level, const char* format, ...);
} // namespace pcc

#endif
