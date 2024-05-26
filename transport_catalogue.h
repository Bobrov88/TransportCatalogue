#pragma once

#include <string>
#include <deque>
#include <iostream>
#include <set>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <string_view>
#include "geo.h"

using Geo::Coordinates;

namespace Data
{
    class TransportCatalogue
    {
        struct Bus;
        struct Stop;
        struct Bus_Hash;
        struct Stop_Hash;

    public:
        void add_bus(std::string_view name, std::vector<std::string_view> stops);
        void add_stop(std::string_view name, Coordinates &&coordinates);
        void add_distances(std::vector<std::string_view> distances);
        const Bus *get_bus(std::string_view bus) const;
        const Stop *get_stop(std::string_view stop) const;
        const std::unordered_map<std::string_view, const Bus *, Bus_Hash> &get_buses() const;
        const std::unordered_map<std::string_view, const Stop *, Stop_Hash> &get_stops() const;
        size_t get_stop_count(std::string_view bus) const;
        size_t get_unique_stop_count(std::string_view bus) const;
        double get_route_length(std::string_view bus) const;

    private:
        struct Stop
        {
            std::string name;
            Coordinates coordinates;
            std::unordered_map<std::string_view, int> distances_to;

            bool operator==(const Stop &stop) const
            {
                return name == stop.name && coordinates == stop.coordinates;
            }
        };

        struct Bus
        {
            std::string name;
            std::deque<std::string_view> stops;

            bool operator==(const Bus &bus) const
            {
                return name == bus.name;
            }
        };

        struct Bus_Hash
        {
            size_t operator()(std::string_view name) const
            {
                return std::hash<std::string_view>{}(name);
            }
        };

        struct Stop_Hash
        {
            size_t operator()(std::string_view name) const
            {
                // return static_cast<size_t>(static_cast<int>((stop.coordinates.lat * stop.coordinates.lng * 100000.0)));
                return std::hash<std::string_view>{}(name);
            }
        };

        std::deque<Bus> b_;
        std::deque<Stop> st_;
        std::unordered_map<std::string_view, const Bus *, Bus_Hash> buses_;
        std::unordered_map<std::string_view, const Stop *, Stop_Hash> stops_;
    };
}