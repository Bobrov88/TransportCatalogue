#pragma once

#include <string>
#include <deque>
#include <iostream>
#include <set>
#include <algorithm>
#include <unordered_set>
#include <vector>
#include <string_view>
#include "geo.h"

class TransportCatalogue
{
    struct Bus;
    struct Stop;

public:
    void add_bus(std::string_view name, std::vector<std::string_view> stops);
    void add_stop(std::string_view name, Coordinates &&coordinates);
    const Bus *get_bus(std::string_view bus) const;
    const Stop *get_stop(std::string_view stop) const;
    size_t get_stop_count(std::string_view bus) const;
    size_t get_unique_stop_count(std::string_view bus) const;
    double get_route_length(std::string_view bus) const;

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
        std::deque<const Stop *> stops;

        bool operator==(const Bus &bus) const
        {
            return name == bus.name;
        }
    };

    struct Bus_Hash
    {
        size_t operator()(const Bus &bus) const
        {
            return std::hash<std::string>{}(bus.name);
        }
    };

    struct Stop_Hash
    {
        size_t operator()(const Stop &stop) const
        {
            return static_cast<size_t>(static_cast<int>((stop.coordinates.lat * stop.coordinates.lng * 100000.0)));
        }
    };

    std::unordered_set<Bus, Bus_Hash> buses_;
    std::unordered_set<Stop, Stop_Hash> stops_;
};