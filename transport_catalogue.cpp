#include "transport_catalogue.h"

void TransportCatalogue::add_bus(std::string_view name, std::vector<std::string_view> stops)
{
    Bus bus;
    bus.name = name;
    for (const auto &stop : stops)
        bus.stops.push_back(get_stop(stop));
    buses_.insert(std::move(bus));
}

void TransportCatalogue::add_stop(std::string_view name, Coordinates &&coordinates)
{
    Stop stop;
    stop.name = std::move(name);
    stop.coordinates = std::move(coordinates);
    stops_.insert(std::move(stop));
}

const TransportCatalogue::Bus *TransportCatalogue::get_bus(std::string_view bus) const
{
    return &(*std::find_if(buses_.cbegin(), buses_.cend(),
                           [&bus](const auto &lhs)
                           {
                               return lhs.name == bus;
                           }));
}

const TransportCatalogue::Stop *TransportCatalogue::get_stop(std::string_view stop) const
{
    return &(*std::find_if(stops_.cbegin(), stops_.cend(),
                           [&stop](const auto &lhs)
                           {
                               return lhs.name == stop;
                           }));
}

size_t TransportCatalogue::get_stop_count(std::string_view bus) const
{
    return get_bus(bus)->stops.size();
}
size_t TransportCatalogue::get_unique_stop_count(std::string_view bus) const
{
    auto &stops = get_bus(bus)->stops;
    std::set<const Stop *> unique_stop{stops.cbegin(), stops.cend()};
    return unique_stop.size();
}
double TransportCatalogue::get_route_length(std::string_view bus) const
{
    auto &stops = get_bus(bus)->stops;
    double route_length = 0.0;
    for (size_t i = 0; i < stops.size() - 1; ++i)
    {
        route_length += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
    }
    return route_length;
}