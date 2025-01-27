#include <numeric>
#include "request_handler.h"
#include "transport_router.h"

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

std::optional<std::unordered_set<BusPtr>> RequestHandler::GetBusesByStop(const std::string_view &stop_name) const
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
    const auto &stops = db_.GetStops();
    const std::map<std::string_view, const entity::Bus *> sorted_buses{buses.cbegin(), buses.cend()};
    const std::map<std::string_view, const entity::Stop *> sorted_stop{stops.cbegin(), stops.cend()};
    std::vector<svg::Polyline> polylines;
    std::vector<svg::Text> bus_names;
    std::vector<svg::Circle> stop_rounds;
    std::vector<svg::Text> stop_names;
    size_t color_pallete_order = 0;
    for (const auto &[route_name, bus] : sorted_buses)
    {
        const auto &route_stops = bus->stops;
        if (route_stops.size() != 0)
        {
            std::vector<geo::Coordinates> coordinates;
            coordinates.reserve(route_stops.size());
            for_each(route_stops.begin(), route_stops.end(), [&coordinates, this](const auto &stop)
                     { coordinates.push_back(this->db_.GetStop(stop)->coordinates); });
            if (!bus->is_round_trip)
                for_each(std::next(route_stops.rbegin()), route_stops.rend(), [&coordinates, this](const auto &stop)
                         { coordinates.push_back(this->db_.GetStop(stop)->coordinates); });

            polylines.push_back(std::move(renderer_.CreatePolyline(coordinates, color_pallete_order)));
            bus_names.push_back(std::move(renderer_.CreateStrokeUnderBusName(std::string(route_name), db_.GetStop(bus->stops.front())->coordinates)));
            bus_names.push_back(std::move(renderer_.CreateBusName(std::string(route_name), db_.GetStop(bus->stops.front())->coordinates, color_pallete_order)));
            if (!bus->is_round_trip && bus->stops.front() != bus->stops.back())
            {
                bus_names.push_back(std::move(renderer_.CreateStrokeUnderBusName(std::string(route_name), db_.GetStop(bus->stops.back())->coordinates)));
                bus_names.push_back(std::move(renderer_.CreateBusName(std::string(route_name), db_.GetStop(bus->stops.back())->coordinates, color_pallete_order)));
            }
            ++color_pallete_order;
        }
    }
    for (const auto &[stop_name, stop] : sorted_stop)
    {
        if (stop->is_in_route)
        {
            stop_rounds.push_back(std::move(renderer_.CreateCircle(stop->coordinates)));
            stop_names.push_back(std::move(renderer_.CreateStrokeUnderStopName(stop->name, stop->coordinates)));
            stop_names.push_back(std::move(renderer_.CreateStopName(stop->name, stop->coordinates)));
        }
    }

    svg::Document doc;
    for (const auto &p : polylines)
        doc.Add(p);
    for (const auto &b : bus_names)
        doc.Add(b);
    for (const auto &s : stop_rounds)
        doc.Add(s);
    for (const auto &s : stop_names)
        doc.Add(s);

    return doc;
}

void RequestHandler::InitializeRouter(routestats stats)
{
    router_.InitializeGraph(stats);
}

std::pair<double, std::optional<std::vector<RouteItems>>> RequestHandler::GetRouteItems(const std::string_view &from_stop, const std::string_view &to_stop) const
{
    if (!db_.GetStop(from_stop)->is_in_route ||
        !db_.GetStop(to_stop)->is_in_route)
        return {0., std::nullopt};

    using namespace graph;
    const auto &router = router_.GetInnerRouter();

    auto route_info = router->BuildRoute(router_.GetIdByStop(from_stop),
                                         router_.GetIdByStop(to_stop));
    if (route_info->edges.empty())
        return {0., std::nullopt};

    const auto [stops_chain, weights] = router_.GetRouteVector(*route_info);
    int waiting_time = router_.GetBusWaitingTime();

    std::vector<RouteItems> route_items;
    auto it = stops_chain.begin();
    while (it != stops_chain.end())
    {
        route_items.push_back(WaitingOnStop{*it, waiting_time});
        auto diff = it;
        std::string_view bus = router_.FindMostUsedBusInStopsChain(it, stops_chain.cend(), *GetBusesByStop(*it));
        if (bus.empty())
            break;
        int w_begin_diff = static_cast<int>(std::distance(stops_chain.begin(), diff));
        int w_end_diff = static_cast<int>(std::distance(stops_chain.begin(), it));
        double weight_of_chain = std::accumulate(weights.begin() + w_begin_diff, weights.begin() + w_end_diff, 0.);
        route_items.push_back(UsingBus{bus, w_end_diff - w_begin_diff, weight_of_chain});
        route_info->weight += waiting_time;
    }
    route_items.pop_back();
    return {route_info->weight, route_items};
}