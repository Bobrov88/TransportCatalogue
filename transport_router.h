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
        std::unique_ptr<graph::Router<double>> router_;
        const TransportCatalogue &db_;
        routestats stats_;
        Graph dw_graph_;
        void GiveIdToStops();

    public:
        TransportRouter(const TransportCatalogue &db) : db_(db) {}
        void InitializeGraph(routestats stats);
        const Graph &GetGraph() const;
        const std::unique_ptr<graph::Router<double>> &GetInnerRouter() const;
        std::string_view GetStopById(size_t id) const;
        size_t GetIdByStop(std::string_view stop) const;
        int GetBusWaitingTime() const;
        std::tuple<std::vector<std::string_view>, std::vector<double>> GetRouteVector(const graph::Router<double>::RouteInfo &) const;

        template <typename It>
        std::string_view FindMostUsedBusInStopsChain(It &it, It end, const std::unordered_set<BusPtr> &buses) const
        {
            if (it == end)
                return {};
            size_t max_chain_length = 0;
            std::string_view used_bus;
            auto farthest_it = it;
            for (const auto &bus : buses)
            {
                const auto &current_bus = db_.GetBus(bus);
                const auto &stops = current_bus->stops;
                auto it2 = it;
                size_t current_chain_length = 0;
                if (auto found = std::find(stops.cbegin(), stops.cend(), *it); found != stops.cend())
                {
                    if (current_bus->is_round_trip)
                    {
                        while (it2 != end && found != stops.cend() && *found == *it2)
                        {
                            ++current_chain_length;
                            ++found;
                            ++it2;
                        }
                    }
                    else
                    {
                        if (std::next(it2) != end && std::next(found) != stops.cend() && *std::next(it2) == *std::next(found))
                        {
                            do
                            {
                                ++current_chain_length;
                                ++found;
                                ++it2;
                            } while (it2 != end && found != stops.cend() && *it2 == *found);
                        }
                        else
                        {
                            if (std::next(it2) != end && std::prev(found) != std::prev(stops.begin()) && *std::next(it2) == *std::prev(found))
                                do
                                {
                                    ++current_chain_length;
                                    --found;
                                    ++it2;
                                } while (it2 != end && found != std::prev(stops.begin()) && *it2 == *found);
                        }
                    }
                }
                if (current_chain_length > max_chain_length)
                {
                    max_chain_length = current_chain_length;
                    used_bus = bus;
                    farthest_it = it2;
                }
            }
            it = farthest_it - 1;
            return used_bus;
        }
    };
}