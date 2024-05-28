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
        struct Dist_Hash;

    public:
        void AddBus(std::string_view name, std::vector<std::string_view> stops);
        void AddStop(std::string_view name, Coordinates &&coordinates);
        void AddDistances(std::string_view stop, std::unordered_map<std::string_view, int> distances);
        const Bus *GetBus(std::string_view bus) const;
        const Stop *GetStop(std::string_view stop) const;
        const std::unordered_map<std::string_view, const Bus *, Bus_Hash> &GetBuses() const;
        const std::unordered_map<std::string_view, const Stop *, Stop_Hash> &GetStops() const;
        size_t GetStopCount(std::string_view bus) const;
        size_t GetUniqueStopCount(std::string_view bus) const;
        double GetRouteLength(std::string_view bus) const;
        int GetRealRouteLength(std::string_view bus) const;
        int GetDistanceBetweenStops(std::string_view from, std::string_view to) const;

    private:
        struct Stop
        {
            std::string name;
            Coordinates coordinates;

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

        struct Dist_Hash {
            size_t operator()(std::pair<const Stop*, const Stop*> pair_of_names) const {
                return  std::hash<const void*>{}(pair_of_names.first) +
                        std::hash<const void*>{}(pair_of_names.second);
            }
        };

        std::deque<Bus> b_;
        std::deque<Stop> st_;
        std::unordered_map<std::string_view, const Bus *, Bus_Hash> buses_;
        std::unordered_map<std::string_view, const Stop *, Stop_Hash> stops_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, Dist_Hash> distances_;
        
    };
}