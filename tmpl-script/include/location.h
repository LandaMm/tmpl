

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
}

#endif // LOCATION_H

