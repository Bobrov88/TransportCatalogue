#include "transport_catalogue.h"
#include <set>

using namespace Data;

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view> stops, bool is_round_trip)
{
    Bus bus;
    bus.name = name;
    bus.is_round_trip = is_round_trip;
    for (const auto &stop : stops)
    {
        bus.stops.push_back(stops_.at(stop)->name);
        stops_.at(stop)->is_in_route = true;
    }
    auto &ref = b_.emplace_back(std::move(bus));
    buses_[ref.name] = &ref;
}

void TransportCatalogue::AddStop(std::string_view name, Coordinates &&coordinates)
{
    Stop stop;
    stop.name = name;
    stop.coordinates = std::move(coordinates);
    auto &ref = st_.emplace_back(std::move(stop));
    stops_[ref.name] = &ref;
}

void TransportCatalogue::AddDistances(std::unordered_map<std::string_view, std::map<std::string, int>> &&distances)
{
    for (const auto &[key1, value1] : distances)
    {
        for (const auto &[key2, value2] : value1)
        {
            distances_.emplace(std::pair(stops_.at(key1), stops_.at(key2)), value2);
        }
    }
}

const Bus *TransportCatalogue::GetBus(std::string_view bus) const
{
    if (buses_.find(bus) != buses_.end())
        return buses_.at(bus);
    else
        return nullptr;
}

const Stop *TransportCatalogue::GetStop(std::string_view stop) const
{
    if (stops_.find(stop) != stops_.end())
        return stops_.at(stop);
    else
        return nullptr;
}

size_t TransportCatalogue::GetStopCount(std::string_view bus) const
{
    size_t count = GetBus(bus)->stops.size();
    if (GetBus(bus)->is_round_trip)
        return count;
    else
        return count * 2 - 1;
}

size_t TransportCatalogue::GetUniqueStopCount(std::string_view bus) const
{
    auto &stops = GetBus(bus)->stops;
    if (!GetBus(bus)->is_round_trip)
        return stops.size() - 1;
    std::set<std::string_view> unique_stop{stops.cbegin(), stops.cend()};
    return unique_stop.size();
}

double TransportCatalogue::GetRouteLength(std::string_view bus) const
{
    auto &stops = GetBus(bus)->stops;
    double route_length = 0.0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
    {
        route_length += ComputeDistance(stops_.at(stops[i])->coordinates,
                                        stops_.at(stops[i + 1])->coordinates);
    }
    if (!GetBus(bus)->is_round_trip)
        route_length *= 2;
    return route_length;
}

int TransportCatalogue::GetRealRouteLength(std::string_view bus) const
{
    auto &stops = GetBus(bus)->stops;
    int real_route_length = 0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
        real_route_length += GetDistanceBetweenStops(stops[i], stops[i + 1]);
    if (!GetBus(bus)->is_round_trip)
    {
        for (size_t i = stops.size() - 1; i > 0; --i)
        {
            real_route_length += GetDistanceBetweenStops(stops[i], stops[i - 1]);
        }
    }
    return real_route_length;
}

int TransportCatalogue::GetDistanceBetweenStops(std::string_view from, std::string_view to) const
{
    auto found = distances_.find({stops_.at(from), stops_.at(to)});
    if (found != distances_.end())
        return found->second;
    return distances_.find({stops_.at(to), stops_.at(from)})->second;
}

const std::unordered_map<std::string_view, const Bus *, Bus_Hash> &TransportCatalogue::GetBuses() const
{
    return buses_;
}

const std::unordered_map<std::string_view, const Stop *, Stop_Hash> &TransportCatalogue::GetStops() const
{
    return stops_;
}