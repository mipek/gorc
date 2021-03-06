#include "diagnostic_context_location.hpp"

gorc::diagnostic_context_location::diagnostic_context_location()
    : filename(nothing)
    , first_line(0)
    , first_col(0)
    , last_line(0)
    , last_col(0)
{
    return;
}

gorc::diagnostic_context_location::diagnostic_context_location(maybe<char const *> filename,
                                                               int first_line,
                                                               int first_col,
                                                               int last_line,
                                                               int last_col)
    : filename(filename)
    , first_line(first_line)
    , first_col(first_col)
    , last_line(last_line)
    , last_col(last_col)
{
    return;
}

bool gorc::diagnostic_context_location::operator==(diagnostic_context_location const &loc) const
{
    return (filename == loc.filename) &&
           (first_line == loc.first_line) &&
           (first_col == loc.first_col) &&
           (last_line == loc.last_line) &&
           (last_col == loc.last_col);
}
