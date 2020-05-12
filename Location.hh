// A Bison parser, made by GNU Bison 3.5.1.

// Locations for Bison parsers in C++

// Copyright (C) 2002-2015, 2018-2020 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

#ifndef PCC_PARSER_LOCATION_H
#define PCC_PARSER_LOCATION_H

#include <iostream>
#include <string>

#ifndef YY_NULLPTR
#if defined __cplusplus
#if 201103L <= __cplusplus
#define YY_NULLPTR nullptr
#else
#define YY_NULLPTR 0
#endif
#else
#define YY_NULLPTR ((void*) 0)
#endif
#endif

namespace pcc {
/// A point in a source file.
class Position {
  public:
    /// Type for line and column numbers.
    typedef int counter_type;

    /// Construct a position.
    explicit Position(std::string* f = YY_NULLPTR, counter_type l = 1,
                      counter_type c = 1)
        : filename(f)
        , line(l)
        , column(c) {}

    /// Initialization.
    void initialize(std::string* fn = YY_NULLPTR, counter_type l = 1,
                    counter_type c = 1) {
        filename = fn;
        line = l;
        column = c;
    }

    /** \name Line and Column related manipulators
     ** \{ */
    /// (line related) Advance to the COUNT next lines.
    void lines(counter_type count = 1) {
        if (count) {
            column = 1;
            line = add_(line, count, 1);
        }
    }

    /// (column related) Advance to the COUNT next columns.
    void columns(counter_type count = 1) { column = add_(column, count, 1); }
    /** \} */

    /// File name to which this position refers.
    std::string* filename;
    /// Current line number.
    counter_type line;
    /// Current column number.
    counter_type column;

  private:
    /// Compute max (min, lhs+rhs).
    static counter_type add_(counter_type lhs, counter_type rhs,
                             counter_type min) {
        return lhs + rhs < min ? min : lhs + rhs;
    }
};

/// Add \a width columns, in place.
inline Position& operator+=(Position& res, Position::counter_type width) {
    res.columns(width);
    return res;
}

/// Add \a width columns.
inline Position operator+(Position res, Position::counter_type width) {
    return res += width;
}

/// Subtract \a width columns, in place.
inline Position& operator-=(Position& res, Position::counter_type width) {
    return res += -width;
}

/// Subtract \a width columns.
inline Position operator-(Position res, Position::counter_type width) {
    return res -= width;
}

/// Compare two position objects.
inline bool operator==(const Position& pos1, const Position& pos2) {
    return (
        pos1.line == pos2.line && pos1.column == pos2.column &&
        (pos1.filename == pos2.filename ||
         (pos1.filename && pos2.filename && *pos1.filename == *pos2.filename)));
}

/// Compare two position objects.
inline bool operator!=(const Position& pos1, const Position& pos2) {
    return !(pos1 == pos2);
}

/** \brief Intercept output stream redirection.
 ** \param ostr the destination output stream
 ** \param pos a reference to the position to redirect
 */
template <typename YYChar>
std::basic_ostream<YYChar>& operator<<(std::basic_ostream<YYChar>& ostr,
                                       const Position& pos) {
    if (pos.filename) ostr << *pos.filename << ':';
    return ostr << pos.line << '.' << pos.column;
};

/// Two points in a source file.
class Location {
  public:
    /// Type for line and column numbers.
    typedef Position::counter_type counter_type;

    /// Construct a location from \a b to \a e.
    Location(const Position& b, const Position& e)
        : begin(b)
        , end(e) {}

    /// Construct a 0-width location in \a p.
    explicit Location(const Position& p = Position())
        : begin(p)
        , end(p) {}

    /// Construct a 0-width location in \a f, \a l, \a c.
    explicit Location(std::string* f, counter_type l = 1, counter_type c = 1)
        : begin(f, l, c)
        , end(f, l, c) {}

    /// Initialization.
    void initialize(std::string* f = YY_NULLPTR, counter_type l = 1,
                    counter_type c = 1) {
        begin.initialize(f, l, c);
        end = begin;
    }

    /** \name Line and Column related manipulators
     ** \{ */
  public:
    /// Reset initial location to final location.
    void step() { begin = end; }

    /// Extend the current location to the COUNT next columns.
    void columns(counter_type count = 1) { end += count; }

    /// Extend the current location to the COUNT next lines.
    void lines(counter_type count = 1) { end.lines(count); }
    /** \} */

  public:
    /// Beginning of the located region.
    Position begin;
    /// End of the located region.
    Position end;
};

/// Join two locations, in place.
inline Location& operator+=(Location& res, const Location& end) {
    res.end = end.end;
    return res;
}

/// Join two locations.
inline Location operator+(Location res, const Location& end) {
    return res += end;
}

/// Add \a width columns to the end position, in place.
inline Location& operator+=(Location& res, Location::counter_type width) {
    res.columns(width);
    return res;
}

/// Add \a width columns to the end position.
inline Location operator+(Location res, Location::counter_type width) {
    return res += width;
}

/// Subtract \a width columns to the end position, in place.
inline Location& operator-=(Location& res, Location::counter_type width) {
    return res += -width;
}

/// Subtract \a width columns to the end position.
inline Location operator-(Location res, Location::counter_type width) {
    return res -= width;
}

/// Compare two location objects.
inline bool operator==(const Location& loc1, const Location& loc2) {
    return loc1.begin == loc2.begin && loc1.end == loc2.end;
}

/// Compare two location objects.
inline bool operator!=(const Location& loc1, const Location& loc2) {
    return !(loc1 == loc2);
}

/** \brief Intercept output stream redirection.
 ** \param ostr the destination output stream
 ** \param loc a reference to the location to redirect
 **
 ** Avoid duplicate information.
 */
template <typename YYChar>
std::basic_ostream<YYChar>& operator<<(std::basic_ostream<YYChar>& ostr,
                                       const Location& loc) {
    Location::counter_type end_col =
        0 < loc.end.column ? loc.end.column - 1 : 0;
    ostr << loc.begin;
    if (loc.end.filename &&
        (!loc.begin.filename || *loc.begin.filename != *loc.end.filename))
        ostr << '-' << loc.end.filename << ':' << loc.end.line << '.'
             << end_col;
    else if (loc.begin.line < loc.end.line)
        ostr << '-' << loc.end.line << '.' << end_col;
    else if (loc.begin.column < end_col)
        ostr << '-' << end_col;
    return ostr;
}

} // namespace pcc

#endif // !PCC_PARSER_LOCATION_H
