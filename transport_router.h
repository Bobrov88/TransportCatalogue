#pragma once
#include <string_view>
#include <unordered_map>
#include <memory>
#include "transport_catalogue.h"
#include "router.h"

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

        template <typename It1, typename It2, typename Operation>
        [[maybe_unused]] bool FindHelper(It1 &it, It1 it_end, It2 stop_border, It2 &found, Operation op, int direction, size_t &current_chain_length) const
        {
            std::cerr << __LINE__ << "\n";
            std::cerr << std::flush;
            if (it == it_end)
            {
                std::cerr << __LINE__ << "\n";
                std::cerr << std::flush;
                return false;
            }
            if (found == op(stop_border))
            {
                std::cerr << __LINE__ << "\n";
                std::cerr << std::flush;
                return false;
            }
            std::cerr << __LINE__ << "\n";
            std::cerr << std::flush;
            if (std::next(it) == it_end)
            {
                std::cerr << __LINE__ << "\n";
                std::cerr << std::flush;
                return false;
            }
            if (op(found) == op(stop_border))
            {
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                return false;
            }
            if (*std::next(it) != *op(found))
            {
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                return false;
            }
            std::cerr << __LINE__ << std::endl;
            std::cerr << std::flush;
            do
            {
                std::cerr << "It=" << *it << std::endl;
                std::cerr << std::flush;
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                ++current_chain_length;
                std::advance(found, direction);
                ++it;
                if (it == it_end)
                {
                    std::cerr << __LINE__ << std::endl;
                    std::cerr << std::flush;
                    std::cerr << std::flush;
                    break;
                }
                if (found == op(stop_border))
                {
                    std::cerr << __LINE__ << std::endl;
                    std::cerr << std::flush;
                    break;
                }
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                std::cerr << *it;
                std::cerr << std::flush;
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                std::cerr << *found;
                std::cerr << std::flush;

                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
            } while (*it == *found);
            std::cerr << __LINE__ << std::endl;
            std::cerr << std::flush;
            return true;
        }

    public:
        TransportRouter(const TransportCatalogue &db) : db_(db)
        {
        }
        void InitializeGraph(routestats stats);
        const Graph &GetGraph() const;
        const std::unique_ptr<graph::Router<double>> &GetInnerRouter() const;
        std::string_view GetStopById(size_t id) const;
        size_t GetIdByStop(std::string_view stop) const;
        int GetBusWaitingTime() const;
        std::pair<std::vector<std::string_view>, std::vector<double>> GetRouteVector(const graph::Router<double>::RouteInfo &) const;

        template <typename It>
        std::string_view FindMostUsedBusInStopsChain(It &it, It end, const std::unordered_set<BusPtr> &buses) const
        {
            std::cerr << __LINE__ << std::endl;
            std::cerr << std::flush;
            size_t max_chain_length = 0;
            std::string_view used_bus;
            auto farthest_it = it;
            for (const auto &bus : buses)
            {
                std::cerr << __LINE__ << std::endl;
                std::cerr << std::flush;
                std::cerr << bus << std::endl;
                std::cerr << *it << std::endl;
                std::cerr << std::flush;
                const auto &current_bus = db_.GetBus(bus);
                const auto &stops = current_bus->stops;
                auto it2 = it;
                size_t current_chain_length = 0;
                if (auto found = std::find(stops.begin(), stops.end(), *it); found != stops.end())
                {
                    std::cerr << __LINE__ << std::endl;
                    std::cerr << std::flush;
                    if (!FindHelper(it2, end, std::prev(stops.end()), found, [](const auto &i)
                                    { return std::next(i); }, 1, current_chain_length))
                    {
                        std::cerr << __LINE__ << std::endl;
                        std::cerr << std::flush;
                        if (!current_bus->is_round_trip)
                        {
                            std::cerr << __LINE__ << std::endl;
                            std::cerr << std::flush;
                            FindHelper(it2, end, std::next(stops.begin()), found, [](const auto &i)
                                       { return std::prev(i); }, -1, current_chain_length);
                        }
                        else
                        {
                            std::cerr << __LINE__ << std::endl;
                            std::cerr << std::flush;
                            found = std::find(std::next(found), stops.end(), *it);
                            if (found != stops.end())
                            {
                                std::cerr << __LINE__ << std::endl;
                                std::cerr << std::flush;
                                FindHelper(it2, end, std::prev(stops.end()), found, [](const auto &i)
                                           { return std::next(i); }, 1, current_chain_length);
                            }
                        }
                    }
                }
                if (current_chain_length > max_chain_length)
                {
                    std::cerr << __LINE__ << std::endl;
                    std::cerr << current_chain_length << ">" << max_chain_length << std::endl;
                    std::cerr << std::flush;
                    max_chain_length = current_chain_length;
                    used_bus = bus;
                    std::cerr << bus << std::endl;
                    std::cerr << std::flush;
                    farthest_it = it2;
                }
            }
            std::cerr << __LINE__ << std::endl;
            std::cerr << std::flush;
            it = farthest_it - 1;
            return used_bus;
        }
    };
}