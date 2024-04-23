#include "transport_catalogue.h"

void TransportCatalogue::add_bus(std::string_view name, std::vector<std::string_view> &stops)
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