#include "stat_reader.h"
#include <iomanip>

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    using namespace std::string_view_literals;
    auto parsed_request = request.substr(4, request.size() - 3);
    output << "Bus "sv;
    output << parsed_request << ": "sv;
    if (transport_catalogue.get_bus(parsed_request) == nullptr)
    {
        output << "not found\n"sv;
        return;
    }
    size_t stop_on_route = transport_catalogue.get_stop_count(parsed_request);
    size_t unique_stop_on_route = transport_catalogue.get_unique_stop_count(parsed_request);
    double route_length = transport_catalogue.get_route_length(parsed_request);
    output << stop_on_route << " stops on route, "sv;
    output << unique_stop_on_route << " unique stops, "sv;
    output << std::fixed << route_length << " route length";
    output << "\n"sv;
}