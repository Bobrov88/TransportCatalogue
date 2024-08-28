#pragma once
#include <string>
#include "geo.h"

namespace entity
{
    struct Stop
    {
        std::string name;
        geo::Coordinates coordinates;
        bool operator==(const Stop &stop) const;
    };

    struct Bus
    {
        std::string name;
        bool operator==(const Bus &bus) const;
    };

    namespace hashers
    {
        struct Bus_Hash
        {
            size_t operator()(std::string_view name) const;
        };

        struct Stop_Hash
        {
            size_t operator()(std::string_view name) const;
        };

        struct Dist_Hash
        {
            size_t operator()(std::pair<const Stop *, const Stop *> pair_of_names) const;
        };
    }
}
