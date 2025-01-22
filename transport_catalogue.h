#pragma once

#include <string>
#include <iostream>
#include <list>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <vector>
#include <string_view>
#include "domain.h"

namespace Data
{
    using geo::Coordinates;
    using namespace entity;
    using namespace entity::hashers;

    class TransportCatalogue
    {
        std::list<Bus> b_;
        std::list<Stop> st_;
        std::unordered_map<std::string_view, const Bus *, Bus_Hash> buses_;
        std::unordered_map<std::string_view, const Stop *, Stop_Hash> stops_;
        std::unordered_map<std::pair<const Stop *, const Stop *>, int, Dist_Hash> distances_;

    public:
        void AddBus(std::string_view name, std::vector<std::string_view> stops, bool is_round_trip);
        void AddStop(std::string_view name, Coordinates &&coordinates);
        void AddDistances(std::unordered_map<std::string_view, std::map<std::string, int>> &&distances);
        const Bus *GetBus(std::string_view bus) const;
        const Stop *GetStop(std::string_view stop) const;
        const std::unordered_map<std::string_view, const Bus *, Bus_Hash> &GetBuses() const;
        const std::unordered_map<std::string_view, const Stop *, Stop_Hash> &GetStops() const;
        const std::unordered_map<std::pair<const Stop *, const Stop *>, int, Dist_Hash>& GetDistances() const;
        size_t GetStopCount(std::string_view bus) const;
        size_t GetUniqueStopCount(std::string_view bus) const;
        double GetRouteLength(std::string_view bus) const;
        int GetRealRouteLength(std::string_view bus) const;
        int GetDistanceBetweenStops(std::string_view from, std::string_view to) const;
    };
}