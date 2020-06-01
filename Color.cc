#ifdef _MSC_VER
#include <Windows.h>
#endif

#include <map>

#include "Color.hh"

namespace pcc {
#if _MSC_VER
static const std::map<Color, WORD> ColorMap = {
    {Color::BLACK, 0},
    {Color::RED, FOREGROUND_INTENSITY | FOREGROUND_RED},
    {Color::GREEN, FOREGROUND_INTENSITY | FOREGROUND_GREEN},
    {Color::BLUE, FOREGROUND_INTENSITY | FOREGROUND_BLUE},
    {Color::YELLOW, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN},
    {Color::CYAN, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE},
    {Color::MAGENTA, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED},
    {Color::WHITE, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |
                       FOREGROUND_BLUE},
    {Color::DEFAULT, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE}};
#else
static const std::map<Color, const char*> ColorMap = {
    {Color::BLACK, "\e[30;1m"},   {Color::RED, "\e[31;1m"},
    {Color::GREEN, "\e[32;1m"},   {Color::BLUE, "\e[34;1m"},
    {Color::YELLOW, "\e[33;1m"},  {Color::CYAN, "\e[36;1m"},
    {Color::MAGENTA, "\e[35;1m"}, {Color::WHITE, "\e[37;1m"},
    {Color::DEFAULT, "\e[0m"}};
#endif // _MSC_VER

std::ostream& operator<<(std::ostream& out, Color color) {
#ifdef _MSC_VER
    HANDLE h = GetStdHandle(STD_ERROR_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(h, &info);
    WORD attr = info.wAttributes;
    attr &= ~(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN |
              FOREGROUND_BLUE);
    SetConsoleTextAttribute(h, attr | ColorMap.at(color));
#else
    out << ColorMap.at(color);
#endif // _MSC_VER
    return out;
}
} // namespace pcc
