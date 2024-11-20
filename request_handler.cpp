#include "request_handler.h"

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view &bus_name) const
{
    if (db_.GetBus(bus_name) == nullptr)
    {
        return std::nullopt;
    }
    BusStat busstat;
    busstat.stop_count_ = db_.GetStopCount(bus_name);
    busstat.unique_stop_count_ = db_.GetUniqueStopCount(bus_name);
    int real_route = db_.GetRealRouteLength(bus_name);
    busstat.route_length_ = real_route;
    busstat.curvature_ = static_cast<double>(real_route) / db_.GetRouteLength(bus_name);
    return busstat;
}

const std::optional<std::unordered_set<BusPtr>> RequestHandler::GetBusesByStop(const std::string_view &stop_name) const
{
    if (db_.GetStop(stop_name) == nullptr)
    {
        return std::nullopt;
    }
    std::unordered_set<BusPtr> buses;
    auto b = db_.GetBuses().cbegin();
    auto e = db_.GetBuses().cend();
    for_each(b, e,
             [&buses, &stop_name](const auto &bus)
             {
                 const auto &stops_ref = (bus.second)->stops;
                 if (std::find(stops_ref.cbegin(),
                               stops_ref.cend(), stop_name) != stops_ref.cend())
                     buses.insert(bus.first);
             });
    return buses;
}

svg::Document RequestHandler::RenderMap() const
{
    const auto &buses = db_.GetBuses();
    const std::map<std::string_view, const entity::Bus *> sorted_buses{buses.cbegin(), buses.cend()};
    svg::Document doc;
    for (const auto &bus : sorted_buses)
    {
        const auto &stops = bus.second->stops;
        std::vector<geo::Coordinates> coordinates;
        coordinates.reserve(stops.size());
        for_each(stops.begin(), stops.end(), [&coordinates, this](const auto &stop)
                 { coordinates.push_back(this->db_.GetStop(stop)->coordinates); });
        doc.Add(renderer_.CreatePolyline(coordinates));
    }
    return doc;
}
