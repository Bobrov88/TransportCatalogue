#pragma once
#include <string>
#include <string_view>
#include <deque>
#include <variant>
#include "geo.h"

namespace entity
{
    struct Stop
    {
        std::string name;
        geo::Coordinates coordinates;
        bool operator==(const Stop &stop) const;
        mutable bool is_in_route = false;
    };

    struct Bus
    {
        std::string name;
        std::deque<std::string_view> stops;
        bool operator==(const Bus &bus) const;
        bool is_round_trip = false;
    };

    struct BusStat
    {
        double curvature_;
        int route_length_;
        size_t stop_count_;
        size_t unique_stop_count_;
    };

    using WaitingOnStop = std::string_view;

    struct UsingBus
    {
        std::string_view bus_name;
        int span_count;
        double used_time;
    };

    using RouteItems = std::variant<WaitingOnStop, UsingBus>;

    using BusPtr = std::string_view;

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
