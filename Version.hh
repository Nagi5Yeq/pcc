#ifndef PCC_VERSION_H
#define PCC_VERSION_H

#include <cstdlib>
#include <iostream>

#define PCC_NAME "pcc"
#define PCC_VERSION_STRING "0.1.1"

#ifndef PCC_OSNAME
#define PCC_OSNAME "unknown platform"
#endif

#ifdef _MSC_VER
#define PCC_NORETURN __declspec(noreturn)
#else
#define PCC_NORETURN __attribute__((noreturn))
#endif

namespace pcc {

PCC_NORETURN void ShowVersion();
PCC_NORETURN void ShowHelp();

} // namespace pcc

#endif
