#include "stat_reader.h"
#include <iomanip>
#include <set>

using namespace std::string_view_literals;

Request ParseRequest(const std::string_view request)
{
    Request result;
    auto not_space = request.find_first_not_of(' ');
    if (not_space != std::string_view::npos)
        return {};
    auto pos = request.find(' ');
    result.request = request.substr(not_space, pos - not_space);
    pos = request.find_first_not_of(' ');
    auto last_pos = request.find_last_not_of(' ');
    result.what = request.substr(pos, last_pos - pos);
    return result;
}

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    auto parsed = ParseRequest(request);
    output << parsed.request << " "sv << parsed.what << ": "sv;
    if (parsed.request == "Bus"sv)
    {
        if (transport_catalogue.get_bus(parsed.what) == nullptr)
        {
            output << "not found\n"sv;
        }
        else
        {
            size_t stop_on_route = transport_catalogue.get_stop_count(parsed.what);
            size_t unique_stop_on_route = transport_catalogue.get_unique_stop_count(parsed.what);
            double route_length = transport_catalogue.get_route_length(parsed.what);
            output << stop_on_route << " stops on route, "sv;
            output << unique_stop_on_route << " unique stops, "sv;
            output << std::fixed << route_length << " route length";
            output << "\n"sv;
        }
        return;
    }
    if (parsed.request == "Stop"sv)
    {
        auto stop = transport_catalogue.get_stop(parsed.what);
        if (stop == nullptr)
        {
            output << "not found\n"sv;
        }
        else
        {
            std::set<std::string_view> bus_list;
            for (const auto& bus : transport_catalogue.get_buses()) {
                if (std::find(bus.stops.cbegin(), bus.stops.cend(), stop) != bus.stops.cend())
                bus_list.insert(stop->name);
            }
            if (bus_list.empty()) {
                output <<"not buses\n";
            }
            
        }
    }
