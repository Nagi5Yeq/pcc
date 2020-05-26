#ifndef PCC_LOG_H
#define PCC_LOG_H

#include <cstdarg>
#include <cstdio>

namespace pcc {
enum LogLevel { PCC_DEBUG, PCC_INFO, PCC_WARNING, PCC_ERROR };

extern LogLevel CurrentLogLevel;

void SetLogLevel(LogLevel level);

void Log(LogLevel level, const char* format, ...);
} // namespace pcc

#endif
