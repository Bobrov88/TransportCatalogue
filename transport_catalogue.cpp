#include "transport_catalogue.h"

using namespace Data;

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view> stops)
{
    Bus bus;
    bus.name = name;
    auto &ref = b_.emplace_back(std::move(bus));
    buses_[ref.name] = &ref;
    for (const auto &stop : stops)
        stops_at_bus_[b_.back().name].push_back(stops_.at(stop));
}

void TransportCatalogue::AddStop(std::string_view name, Coordinates &&coordinates)
{
    Stop stop;
    stop.name = name;
    stop.coordinates = std::move(coordinates);
    auto &ref = st_.emplace_back(std::move(stop));
    stops_[ref.name] = &ref;
}

void TransportCatalogue::AddBusesToStop()
{
    for (const auto &[bus, stops] : stops_at_bus_)
    {
        for (const auto &stop : stops)
        {
            buses_at_stop_[stop->name].push_back(buses_.at(bus));
        }
    }
}

void TransportCatalogue::AddDistances(std::unordered_map<std::string_view, std::map<std::string, int>> distances)
{
    for (auto &[key1, value1] : distances)
    {
        for (auto &[key2, value2] : value1)
        {
            distances_.emplace(std::pair(stops_.at(key1), stops_.at(key2)), value2);
        }
    }
}

const TransportCatalogue::Bus *TransportCatalogue::GetBus(std::string_view bus) const
{
    if (buses_.find(bus) != buses_.end())
        return buses_.at(bus);
    else
        return nullptr;
}

const TransportCatalogue::Stop *TransportCatalogue::GetStop(std::string_view stop) const
{
    if (stops_.find(stop) != stops_.end())
        return stops_.at(stop);
    else
        return nullptr;
}

size_t TransportCatalogue::GetStopCount(std::string_view bus) const
{
    return stops_at_bus_.at(bus).size();
}

size_t TransportCatalogue::GetUniqueStopCount(std::string_view bus) const
{
    auto &stops = stops_at_bus_.at(bus);
    std::set<const Stop *> unique_stop{stops.cbegin(), stops.cend()};
    return unique_stop.size();
}

double TransportCatalogue::GetRouteLength(std::string_view bus) const
{
    auto &stops = stops_at_bus_.at(bus);
    double route_length = 0.0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
    {
        route_length += ComputeDistance(stops_.at(stops[i]->name)->coordinates,
                                        stops_.at(stops[i + 1]->name)->coordinates);
    }
    return route_length;
}

int TransportCatalogue::GetRealRouteLength(std::string_view bus) const
{
    auto &stops = stops_at_bus_.at(bus);
    int real_route_length = 0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
        real_route_length += GetDistanceBetweenStops(stops[i]->name, stops[i + 1]->name);
    return real_route_length;
}

int TransportCatalogue::GetDistanceBetweenStops(std::string_view from, std::string_view to) const
{
    auto found = distances_.find({stops_.at(from), stops_.at(to)});
    if (found != distances_.end())
        return found->second;
    return distances_.find({stops_.at(to), stops_.at(from)})->second;
}

const std::unordered_map<std::string_view, const TransportCatalogue::Bus *, TransportCatalogue::Bus_Hash> &TransportCatalogue::GetBuses() const
{
    return buses_;
}

const std::unordered_map<std::string_view, const TransportCatalogue::Stop *, TransportCatalogue::Stop_Hash> &TransportCatalogue::GetStops() const
{
    return stops_;
}

const std::unordered_map<std::string_view, std::deque<const TransportCatalogue::Bus *>, TransportCatalogue::Stop_Hash> &TransportCatalogue::GetBusesAtStop() const
{
    return buses_at_stop_;
}

const std::unordered_map<std::string_view, std::deque<const TransportCatalogue::Stop *>, TransportCatalogue::Bus_Hash> &TransportCatalogue::GetStopsAtBus() const
{
    return stops_at_bus_;
}
