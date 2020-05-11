#ifndef PCC_LOG_H
#define PCC_LOG_H

#include <cstdarg>
#include <cstdio>

namespace pcc {
enum LogLevel { PCC_ERROR, PCC_WARNING, PCC_INFO, PCC_DEBUG };

extern LogLevel CurrentLogLevel;

void SetLogLevel(LogLevel level);

void Log(LogLevel level, const char* format, ...);
} // namespace pcc

#endif
