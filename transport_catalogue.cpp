#include "transport_catalogue.h"

using namespace Data;

void TransportCatalogue::add_bus(std::string_view name, std::vector<std::string_view> stops)
{
    Bus bus;
    bus.name = name;
    for (const auto &stop : stops)
        bus.stops.push_back(stop);
    b_.push_back(std::move(bus));
    buses_.insert({std::move(name), &b_.back()});
}

void TransportCatalogue::add_stop(std::string_view name, Coordinates &&coordinates)
{
    Stop stop;
    stop.name = name;
    stop.coordinates = std::move(coordinates);
    st_.push_back(std::move(stop));
    stops_.insert({std::move(name), &st_.back()});
}

const TransportCatalogue::Bus *TransportCatalogue::get_bus(std::string_view bus) const
{
    if (buses_.find(bus) != buses_.end())
        return buses_.at(bus);
    else
        return nullptr;
}

const TransportCatalogue::Stop *TransportCatalogue::get_stop(std::string_view stop) const
{
    if (stops_.find(stop) != stops_.end())
        return stops_.at(stop);
    else
        return nullptr;
}

size_t TransportCatalogue::get_stop_count(std::string_view bus) const
{
    return get_bus(bus)->stops.size();
}

size_t TransportCatalogue::get_unique_stop_count(std::string_view bus) const
{
    auto &stops = get_bus(bus)->stops;
    std::set<std::string_view> unique_stop{stops.cbegin(), stops.cend()};
    return unique_stop.size();
}

double TransportCatalogue::get_route_length(std::string_view bus) const
{
    auto &stops = get_bus(bus)->stops;
    double route_length = 0.0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
    {
        route_length += ComputeDistance(get_stop(stops[i])->coordinates,
                                        get_stop(stops[i + 1])->coordinates);
    }
    return route_length;
}

const std::unordered_map<std::string_view, const TransportCatalogue::Bus *, TransportCatalogue::Bus_Hash> &TransportCatalogue::get_buses() const
{
    return buses_;
}

const std::unordered_map<std::string_view, const TransportCatalogue::Stop *, TransportCatalogue::Stop_Hash> &TransportCatalogue::get_stops() const
{
    return stops_;
}