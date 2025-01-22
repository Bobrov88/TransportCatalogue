#include "transport_router.h"

using namespace Data;

void TransportRouter::GiveIdToStops(const TransportCatalogue &db)
{
    size_t id = 0;
    for (const auto &[stop_name, _] : db.GetStops())
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
const std::unique_ptr<graph::Router<double>> &TransportRouter::GetRouter() const
{
    return router_;
}

void TransportRouter::InitializeGraph(const TransportCatalogue &db)
{
    GiveIdToStops(db);
    graph::DirectedWeightedGraph<double> dw_graph(db.GetStops().size());

    for (const auto &[stops, distance] : db.GetDistances())
        dw_graph.AddEdge({GetIdByStop(stops.first->name),
                          GetIdByStop(stops.second->name),
                          distance / routestats::bus_velocity});

    dw_graph_ = std::move(dw_graph);
    router_ = std::make_unique<graph::Router<double>>(dw_graph_);
}