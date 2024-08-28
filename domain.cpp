#include "domain.h"

namespace entity
{
    bool Stop::operator==(const Stop &stop) const
    {
        return name == stop.name && coordinates == stop.coordinates;
    }

    bool Bus::operator==(const Bus &bus) const
    {
        return name == bus.name;
    }
    namespace hashers
    {
        size_t Bus_Hash::operator()(std::string_view name) const
        {
            return std::hash<std::string_view>{}(name);
        }

        size_t Stop_Hash::operator()(std::string_view name) const
        {
            return std::hash<std::string_view>{}(name);
        }

        size_t Dist_Hash::operator()(std::pair<const Stop *, const Stop *> pair_of_names) const
        {
            return std::hash<const void *>{}(pair_of_names.first) +
                   std::hash<const void *>{}(pair_of_names.second);
        }
    }
}