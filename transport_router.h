#pragma once
#include <string_view>
#include <unordered_map>
#include <memory>
#include "transport_catalogue.h"
#include "routes.h"

namespace Data
{
    class TransportRouter
    {
        using Graph = graph::DirectedWeightedGraph<double>;

        std::unordered_map<size_t, std::string_view> stopsbyids_;
        std::unordered_map<std::string_view, size_t> idsbystops_;
        Graph dw_graph_;
        std::unique_ptr<graph::Router<double>> router_;
        void GiveIdToStops(const TransportCatalogue &db);

    public:
        TransportRouter() = default;
        void InitializeGraph(const TransportCatalogue &db);
        const Graph& GetGraph() const;
        const std::unique_ptr<graph::Router<double>>& GetRouter() const;
        std::string_view GetStopById(size_t id) const;
        size_t GetIdByStop(std::string_view stop) const;
    };
}