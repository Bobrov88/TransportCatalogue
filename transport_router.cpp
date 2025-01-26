#include "transport_router.h"

using namespace Data;

void TransportRouter::GiveIdToStops()
{
    size_t id = 0;
    for (const auto &[stop_name, _] : db_.GetStops())
    {
        stopsbyids_.insert({id, stop_name});
        idsbystops_.insert({stop_name, id});
        ++id;
    }
}

std::string_view TransportRouter::GetStopById(size_t id) const
{
    return stopsbyids_.at(id);
}

size_t TransportRouter::GetIdByStop(std::string_view stop_name) const
{
    return idsbystops_.at(stop_name);
}

const TransportRouter::Graph &TransportRouter::GetGraph() const
{
    return dw_graph_;
}
const std::unique_ptr<graph::Router<double>> &TransportRouter::GetInnerRouter() const
{
    return router_;
}

int TransportRouter::GetBusWaitingTime() const {
    return stats_.bus_wait_time;
}

void TransportRouter::InitializeGraph(routestats stats)
{
    stats_ = stats;
    GiveIdToStops();
    graph::DirectedWeightedGraph<double> dw_graph(db_.GetStops().size());

    for (const auto &[stops, distance] : db_.GetDistances())
    {
        dw_graph.AddEdge({GetIdByStop(stops.first->name),
                          GetIdByStop(stops.second->name),
                          distance / stats_.bus_velocity});
    }

    dw_graph_ = std::move(dw_graph);
    router_ = std::make_unique<graph::Router<double>>(dw_graph_);
}

std::tuple<std::vector<std::string_view>, std::vector<double>> TransportRouter::GetRouteVector(const graph::Router<double>::RouteInfo &routeInfo) const
{
    const auto &edges = routeInfo.edges;

    std::vector<std::string_view> route_vector;
    route_vector.reserve(edges.size() + 1);
    std::vector<double> weights_vector;
    weights_vector.reserve(edges.size());

    for (auto &edge : edges)
    {
        route_vector.push_back(GetStopById(dw_graph_.GetEdge(edge).from));
        weights_vector.push_back(dw_graph_.GetEdge(edge).weight);
    }
    route_vector.push_back(GetStopById(dw_graph_.GetEdge(edges.back()).to));

    return {route_vector, weights_vector};
}