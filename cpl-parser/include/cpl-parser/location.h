

#ifndef LOCATION_H
#define LOCATION_H
#include <iostream>

namespace AST
{
    struct Location
    {
        size_t line, col;

        Location(size_t line, size_t col)
        {
            this->line = line;
            this->col = col;
        }
    };

    struct LocationSpan
    {
        Location begin, end;

        LocationSpan(Location begin, Location end)
            : begin(begin), end(end) { }
    };
}

#endif // LOCATION_H

