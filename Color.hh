#ifndef PCC_COLOR_HH
#define PCC_COLOR_HH

#include <iostream>

namespace pcc {
enum class Color {
    BLACK,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    CYAN,
    MAGENTA,
    WHITE,
    DEFAULT
};

std::ostream& operator<<(std::ostream& out, Color color);
} // namespace pcc
#endif // !PCC_COLOR_HH
