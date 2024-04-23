#pragma once

#include <string>
#include <deque>
#include <iostream>
#include <set>
#include <unordered_set>
#include "geo.h"

class TransportCatalogue
{
    struct Bus;
    struct Stop;
    
    public:
    void add_bus(std::string&& name, std::vector<std::string_view>& stops);
    const Stop* get_stop(std::string_view stop) const;
    const Bus* get_Bus(std::string_view bus) const;
    void add_stop(std::string&& name, std::vector<std::string_view>& buses);

private:

    struct Stop
    {
        std::string name;
        Coordinates coordinates;
        std::set<const Bus *> buses;
    };

    struct Bus
    {
        std::string name;
        std::deque<const Stop *> stops;
    };

    struct Bus_Hash
    {
        size_t operator()(const Bus &bus)
        {
            return static_cast<size_t>(std::stoi(bus.name));
        }
    };

    struct Stop_Hash
    {
        size_t operator()(const Stop &stop)
        {
            return static_cast<size_t>(stop.coordinates.lat * stop.coordinates.lng * 1000000);
        }
    };

    std::unordered_set<Bus, Bus_Hash> buses_;
    std::unordered_set<Stop, Stop_Hash> stops_;
};