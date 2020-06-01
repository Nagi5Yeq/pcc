#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include <stdarg.h>
#include <stdio.h>

int Pascal_puts(const char* s) { return puts(s); }

int Pascal_printf(const char* fmt, ...) {
    int rv;
    va_list va;
    va_start(va, fmt);
    rv = vprintf(fmt, va);
    va_end(va);
    return rv;
}

int Pascal_scanf(const char* fmt, ...) {
    int rv;
    va_list va;
    va_start(va, fmt);
    rv = vscanf(fmt, va);
    va_end(va);
    return rv;
}
