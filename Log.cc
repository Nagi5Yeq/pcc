#include "Log.hh"

namespace pcc {
void Log(LogLevel level, const char* format, ...) {
    va_list va;
    va_start(va, format);
    if (CurrentLogLevel <= level) {
        switch (level) {
        case PCC_ERROR:
            fprintf(stderr, "[ERROR] ");
            break;
        case PCC_WARNING:
            fprintf(stderr, "[WARNING] ");
            break;
        case PCC_INFO:
            fprintf(stderr, "[INFO] ");
            break;
        case PCC_DEBUG:
            fprintf(stderr, "[DEBUG] ");
            break;
        }
        std::vfprintf(stderr, format, va);
        std::fputc('\n', stderr);
    }
    va_end(va);
}

LogLevel CurrentLogLevel = PCC_ERROR;

void SetLogLevel(LogLevel level) { CurrentLogLevel = level; }
} // namespace pcc
