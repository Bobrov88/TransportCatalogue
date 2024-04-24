#include "stat_reader.h"
#include <iomanip>

void ParseAndPrintStat(const TransportCatalogue &transport_catalogue, std::string_view request,
                       std::ostream &output)
{
    using namespace std::string_view_literals;
    auto parsed_request = request.substr(3, request.size()-3);
    output << "Bus "sv;
    output << parsed_request << ": "sv;
    size_t stop_on_route = transport_catalogue.get_stop_count(parsed_request);
    size_t unique_stop_on_route = transport_catalogue.get_unique_stop_count(parsed_request);
    double route_length = transport_catalogue.get_route_length(parsed_request);
    output << stop_on_route << " stop on route, "sv;
    output << unique_stop_on_route << " unique stop on route, "sv;
    output << std::setprecision(2) << route_length << " route length";
}